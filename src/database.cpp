#include "database.h"
#include "schemahelpers.h"
#include <WDBReader/WoWDBDefs.hpp>
#include <WDBReader/Database/DBCFile.hpp>
#include <WDBReader/Database/DB2File.hpp>
#include <fstream>

Database::Database(const std::string& name) :
	_name(name), _schema({}, {})
{
	auto ext = std::find(_name.rbegin(), _name.rend(), '.');
	if (ext != _name.rend()) {
		_type = std::string(ext.base(), _name.end());
		std::transform(_type.begin(), _type.end(), _type.begin(), [](char c) {
			return std::tolower(c);
		});

		if (_type != "dbc" && _type != "db2") {
			throw std::runtime_error("Unsupported database type.");
		}
	}
	else {
		throw std::runtime_error("Unknown database type.");
	}

}

void Database::load(const WDBReader::ClientInfo& info, const std::filesystem::path& wow_defs_dir, std::unique_ptr<WDBReader::Filesystem::FileSource> source)
{
	assert(source != nullptr);
	_file_size = source->size();

	std::filesystem::path dbd_path = wow_defs_dir / (schemaPlainName(_name) + ".dbd");

	if (!std::filesystem::exists(dbd_path)) {
		throw std::runtime_error("DBD file doesnt exist.");
	}

	std::ifstream stream(dbd_path);
	auto definition = WDBReader::WoWDBDefs::DBDReader::read(stream);
	auto schema = WDBReader::WoWDBDefs::makeSchema(definition, info.version);

	if (!schema.has_value()) {
		throw std::runtime_error("Unable to create schema for version.");
	}

	_schema = std::move(*schema);

	if (_type == "dbc") {
		//TODO better way.
		auto temp = std::make_unique<WDBReader::Database::DBCFile<decltype(_schema), WDBReader::Database::RuntimeRecord, decltype(source)::element_type, false>>(
			_schema, WDBReader::Database::getDBCVersion(info.version), WDBReader::Database::DBCLocaleConvert(info.locales[0])
		);

		temp->open(std::move(source));
		temp->load();

		_data_source = std::move(temp);
	} else if(_type == "db2") {
		auto temp = WDBReader::Database::makeDB2File<decltype(source)::element_type>(_schema, std::move(source));
		_data_source = std::move(temp);
	}
	else {
		throw std::logic_error("Unsupported DB type.");
	}
}
