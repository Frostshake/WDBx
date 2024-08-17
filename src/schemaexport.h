#pragma once

#include "databasemodel.h"
#include <WDBReader/Database.hpp>
#include <atomic>
#include <functional>
#include <memory>
#include <QString>
#include <QTextStream>


class SchemaExport {
public:
	static QString asCSV(const std::string& db_name, const WDBReader::Database::RuntimeSchema& schema);
	static QString asJSON(const std::string& db_name, const WDBReader::Database::RuntimeSchema& schema);
	static QString asSQL(const std::string& db_name, const WDBReader::Database::RuntimeSchema& schema);
};


class DataExport {
public:

	explicit DataExport(std::shared_ptr<DatabaseModel> model, QTextStream& stream) :
		_model(model), _stream(stream), _db(_model->_db), 
		_is_cancelled(false), _progress_callback(nullptr)
	{
	}


	//TODO method to set column formats.
	//TODO method to set column visiblity
	virtual void exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes = std::nullopt) = 0;

	QString formatValue(const auto& value, char quote = '\0') {
		QString result;

		//TODO column formatting

		std::visit([&result, &quote](const auto& val) {
			using val_t = std::decay_t<decltype(val)>;
			if constexpr (std::is_same_v<WDBReader::Database::string_data_t, val_t>) {
				std::string_view view(val.get());
				if (quote != '\0') {
					result = QString("%2%1%2")
						.arg(QString::fromUtf8(view.data(), view.size()))
						.arg(quote);
				}
				else {
					result = QString::fromUtf8(view.data(), view.size());
				}
			}
			else if constexpr (std::is_same_v<WDBReader::Database::string_data_ref_t, val_t>) {
				std::string_view view(val);
				if (quote != '\0') {
					result = QString("%2%1%2")
						.arg(QString::fromUtf8(view.data(), view.size()))
						.arg(quote);
				}
				else {
					result = QString::fromUtf8(view.data(), view.size());
				}
			}
			else if constexpr (std::is_floating_point_v<val_t>) {
				result = QString::number(val);
			}
			else {
				result = QString::number(val);
			}

		}, value);


		return result;
	}

	void cancel() {
		_is_cancelled = true;
	}

	void onProgressed(std::function<void(size_t, size_t)> callback) {
		_progress_callback = callback;
	}

protected:

	void handleRows(const std::optional<std::pair<uint64_t, uint64_t>>& indexes, auto callback) {

		auto* src = _db->getDataSource();
		auto it = src->begin();
		size_t i = 0;
		size_t end = 0;
		if (indexes) {
			std::advance(it, indexes->first);
			i = indexes->first;
			end = indexes->second;
		}
		else {
			end = _db->getRowCount() - 1;
		}

		for (; it != src->end(); ++it) {
			if (indexes) {
				if (i > indexes->second) {
					break;
				}
			}

			WDBReader::Database::RuntimeRecord rec{ std::move(*it) };
			if (rec.encryptionState != WDBReader::Database::RecordEncryption::ENCRYPTED) {
				callback(std::move(rec));
			}

			if (_is_cancelled) {
				break;
			} else if (_progress_callback) {
				_progress_callback(i++, end);
			}
		}
	}

	std::shared_ptr<DatabaseModel> _model;
	std::shared_ptr<Database> _db;
	QTextStream& _stream;
	std::atomic<bool> _is_cancelled;
	std::function<void(size_t, size_t)> _progress_callback;


	//TODO not this will need some kind of locking mechanism, as the underlying db/pagination/columns could be changed in the GUI while export happens.
};


class CSVDataExport final : public DataExport {
public:
	using DataExport::DataExport;
	void exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes = std::nullopt) override;
};

class JSONDataExport final : public DataExport {
public:
	using DataExport::DataExport;
	void exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes = std::nullopt) override;
};

class SQLInsertDataExport final : public DataExport {
public:
	using DataExport::DataExport;
	void exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes = std::nullopt) override;
};

class SQLUpdateDataExport final : public DataExport {
public:
	using DataExport::DataExport;
	void exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes = std::nullopt) override;
};