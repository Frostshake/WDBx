#pragma once

#include <WDBReader/Filesystem.hpp>
#include <WDBReader/Filesystem/CASCFilesystem.hpp>
#include <WDBReader/Filesystem/MPQFilesystem.hpp>
#include <WDBReader/Filesystem/NativeFilesystem.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class Filesystem {
public:
    Filesystem(const std::filesystem::path& root) : _root(root) {};
    virtual ~Filesystem() = default;
    const std::vector<std::string>& discovered() {
        return _discovered;
    }
    virtual std::unique_ptr<WDBReader::Filesystem::FileSource> open(const std::string&) = 0;

protected:
    std::filesystem::path _root;
    std::vector<std::string> _discovered;
};


class NativeFilesystem final : public Filesystem {
public:
    NativeFilesystem(const std::filesystem::path& root);
    virtual std::unique_ptr<WDBReader::Filesystem::FileSource> open(const std::string&) override;

protected:
    void findDbs();
    std::unique_ptr<WDBReader::Filesystem::NativeFilesystem> _fs;
};


class MPQFilesystem final : public Filesystem { 
   public: 
    MPQFilesystem(const std::filesystem::path& root);
    virtual std::unique_ptr<WDBReader::Filesystem::FileSource> open(const std::string&) override;
protected:
    void findDBCs();

    std::unique_ptr<WDBReader::Filesystem::MPQFilesystem> _fs;
};

class CASCFilesystem final : public Filesystem {
public:
   CASCFilesystem(const std::filesystem::path& root, const std::string& locale, const std::string& product);
    virtual std::unique_ptr<WDBReader::Filesystem::FileSource> open(const std::string&) override;

protected:
    void loadTactKeys();
    void loadListFile();
    std::unique_ptr<WDBReader::Filesystem::CASCFilesystem> _fs;
    std::vector<WDBReader::Filesystem::CASCFileUri> _fileIds;
};