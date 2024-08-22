#include "filesystem.h"
#include <map>
#include <ranges>

NativeFilesystem::NativeFilesystem(const std::filesystem::path& root) : Filesystem(root)
{
    _fs = std::make_unique<WDBReader::Filesystem::NativeFilesystem>();
    findDbs();
}

std::unique_ptr<WDBReader::Filesystem::FileSource> NativeFilesystem::open(const std::string& name)
{
    return _fs->open(_root / name);
}

void NativeFilesystem::findDbs()
{
    const auto db_types = {
        ".dbc",
        ".DBC",
        ".db2",
        ".DB2"
    };

    for (const auto& entry : std::filesystem::recursive_directory_iterator(_root)) {
        if (entry.is_regular_file()) {
            if (std::find(db_types.begin(), db_types.end(), entry.path().extension()) != db_types.end()) {
                const auto relative = entry.path().lexically_relative(_root);
                _discovered.push_back(relative.string());
            }
        }
    }
}

MPQFilesystem::MPQFilesystem(const std::filesystem::path& root) : Filesystem(root)
{
    auto mpq_names = WDBReader::Filesystem::discoverMPQArchives(_root);
    _fs = std::make_unique<WDBReader::Filesystem::MPQFilesystem>(_root, std::move(mpq_names));
    findDBCs();
}

std::unique_ptr<WDBReader::Filesystem::FileSource> MPQFilesystem::open(const std::string& name)
{
    return _fs->open(name);
}

void MPQFilesystem::findDBCs()
{
    std::vector<std::string> found;

    const std::string listfile_name("(listfile)");
    for (auto& mpq : _fs->getHandles()) {
        HANDLE temp;
        if (SFileOpenFileEx(mpq.second, listfile_name.c_str(), SFILE_OPEN_FROM_MPQ, &temp)) {
            auto source = std::make_unique<WDBReader::Filesystem::MPQFileSource>(temp);

            const auto size = source->size();
            auto buffer = std::make_unique_for_overwrite<uint8_t[]>(size);
            source->read(buffer.get(), size);

            uint8_t* p = buffer.get();
            uint8_t* end = buffer.get() + size;

            while (p < end) {
                uint8_t* q = p;
                do {
                    if (*q == '\r' || *q == '\n') // carriage return or new line
                        break;
                } while (q++ <= end);

                auto line = std::string_view((char*)p, q - p);

                p = q + 2;

                if (line.length() == 0) {
                    break;
                }

                if (line.ends_with(".dbc") || line.ends_with(".DBC")) {
                    found.emplace_back(line);
                }
            }
        }
    }

    std::sort(found.begin(), found.end());
    found.erase(std::unique(found.begin(), found.end()), found.end());
    _discovered = std::move(found);
}

CASCFilesystem::CASCFilesystem(const std::filesystem::path& root, const std::string& locale, const std::string& product) : Filesystem(root)
{
    auto casc_locale = WDBReader::Filesystem::CASCLocaleConvert(locale);
    _fs = std::make_unique<WDBReader::Filesystem::CASCFilesystem>(root, casc_locale, product);
    loadTactKeys();
    loadListFile();
}

std::unique_ptr<WDBReader::Filesystem::FileSource> CASCFilesystem::open(const std::string& name)
{
    auto found_name = std::find(_discovered.begin(), _discovered.end(), name);

    if (found_name != _discovered.end()) {
        const auto index = std::distance(_discovered.begin(), found_name);
        return _fs->open(_fileIds[index]);
    }

	return nullptr;
}

void CASCFilesystem::loadTactKeys()
{
    std::ifstream stream("tact-keys.txt");
    std::string line;

    // format is '{16 char key}{space}{32 char key}'

    while (std::getline(stream, line)) {
        if (line.size() < (16 + 1 + 32)) {
            continue;
        }

        std::string_view key_str(line.data(), 16);
        std::string_view val_str(line.data() + 16 + 1, 32);
        uint64_t key;
        const auto key_end = key_str.data() + key_str.size();
        const auto res = std::from_chars(key_str.data(), key_end, key, 16);

        if (res.ec == std::errc{} && res.ptr == key_end)
        {
            const bool ok = CascAddStringEncryptionKey(_fs->getHandle(), key, std::string(val_str).c_str());
            assert(ok);
        }
    }

}

void CASCFilesystem::loadListFile()
{
    std::ifstream stream("listfile.csv");
    std::string line;

    std::map<std::string, WDBReader::Filesystem::CASCFileUri> content;

    while (std::getline(stream, line)) {
        const auto sep = line.find_first_of(';');

        if (sep == std::string::npos) {
            continue;
        }

        if (line.length() <= (sep + 1)) {
            continue;
        }

        char* num_end = line.data() + sep;
        WDBReader::Filesystem::CASCFileUri file_id;
        auto result = std::from_chars(line.data(), num_end, file_id);
            
        if (result.ec == std::error_code() && result.ptr == num_end) {
            bool exists = false;

            try {
                exists = _fs->open(file_id) != nullptr;
            }
            catch (...) {
                exists = false;
            }
            
            if (exists) {
                content.emplace(std::string(line.begin() + sep + 1, line.end()), file_id);
            }
        }
    }

    for (auto& entry : content) {
        _fileIds.push_back(entry.second);
        _discovered.push_back(entry.first);
    }
}
