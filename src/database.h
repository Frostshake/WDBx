#pragma once

#include <WDBReader/Detection.hpp>
#include <WDBReader/Database.hpp>
#include <WDBReader/Filesystem.hpp>
#include <filesystem>
#include <string>

class Database {
public:
    Database(const std::string& name);
    void load(const WDBReader::ClientInfo& info, const std::filesystem::path& wow_defs_dir, std::unique_ptr<WDBReader::Filesystem::FileSource> source);

    const std::string& getName() const {
        return _name;
    }

    const std::string& getType() const {
        return _type;
    }

    const WDBReader::Database::RuntimeSchema& getSchema() const {
        return _schema;
    }

    uint64_t getFileSize() const {
        return _file_size;
    }

    uint64_t getRowCount() const {
        return _data_source->size();
    }

    WDBReader::Database::DataSource<WDBReader::Database::RuntimeRecord>* getDataSource() const {
        return _data_source.get();
    }



protected:
    std::string _name;
    std::string _type;
    std::filesystem::path _defs_dir;
    uint64_t _file_size;

    WDBReader::Database::RuntimeSchema _schema;
    std::unique_ptr<WDBReader::Database::DataSource<WDBReader::Database::RuntimeRecord>> _data_source;
};