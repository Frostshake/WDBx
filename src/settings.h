#pragma once
#include <QSettings>

namespace config {
	namespace app {
		constexpr const char* last_save_location = "app/last_save_location";
		constexpr const char* definitions_location = "app/definitions_location";
	}
}


class Settings 
{
public:
	static void init(QObject* parent) {
		_instance = new QSettings("wdbx.ini", QSettings::IniFormat, parent);
	}

	static QSettings* instance() {
		return _instance;
	}

	template<typename T = QString>
	static T get(const char* key) {
		return _instance->value(key).value<T>();
	}

	template<typename T = QString>
	static T get(const char* key, auto defaultval) {
		return _instance->value(key, defaultval).value<T>();
	}

	template<typename T>
	requires (!std::is_scalar_v<T>)
	static void set(const char* key, const T& val) {
		_instance->setValue(key, val);
	}

	template<typename T>
	requires (std::is_scalar_v<T>)
	static void set(const char* key, T val) {
		_instance->setValue(key, val);
	}

private:
	static QSettings* _instance;
		
};
