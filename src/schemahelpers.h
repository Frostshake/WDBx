#pragma once

#include <WDBReader/Database.hpp>
#include <string>

inline constexpr auto schemaPlainName(auto name) {
    auto last_sep = std::find_if(name.rbegin(), name.rend(), [](char c) {
        return c == '/' || c == '\\';
        });

    if (last_sep != name.rend()) {
        name.erase(name.begin(), last_sep.base());
    }

    auto first_ext = std::find(name.begin(), name.end(), '.');
    if (first_ext != name.end()) {
        name.erase(first_ext, name.end());
    }

    return name;
}

inline constexpr auto schemaFieldTypeToString(WDBReader::Database::Field::Type type) {

    switch (type) {
    case WDBReader::Database::Field::Type::FLOAT:
        return "FLOAT";
    case WDBReader::Database::Field::Type::INT:
        return "INT";
    case WDBReader::Database::Field::Type::LANG_STRING:
        return "LANG_STRING";
    case WDBReader::Database::Field::Type::STRING:
        return "STRING";
    }

    return  "UNKNOWN";
}

template<typename T>
inline constexpr T schemaFieldAnnotationsToString(const WDBReader::Database::Annotation& annotation) {
    T result;

    if (annotation.isId) {
        result.push_back("ID");
    }
    if (annotation.isInline) {
        result.push_back("INLINE");
    }
    else {
        result.push_back("NON-INLINE");
    }
    if (annotation.isRelation) {
        result.push_back("RELATION");
    }

    return result;
}