#pragma once

#include <string>
#include <vector>

#include "Exceptions.h"

// ---------------------------------------------------------------
// StringUtils.h
//
// Small helpers for reading the text records of the data files.
// Header-only (inline), so there is no StringUtils.cpp.
// ---------------------------------------------------------------

// Split text at every separator, keeping empty parts:
//   splitBy("a:b::c", ':')  ->  {"a", "b", "", "c"}
inline std::vector<std::string> splitBy(const std::string& text, char separator) {
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        size_t pos = text.find(separator, start);
        if (pos == std::string::npos) {
            parts.push_back(text.substr(start));
            break;
        }
        parts.push_back(text.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

// Whole-text number conversion: "12" works, "12x" and "" do not.
// `what` names the field in the error message, e.g. "quantity".
inline int parseInt(const std::string& text, const std::string& what) {
    try {
        size_t used = 0;
        int value = std::stoi(text, &used);
        if (used != text.size())
            throw std::invalid_argument("extra characters");
        return value;
    }
    catch (const std::exception&) {
        throw ShopException("Invalid " + what + ": '" + text + "'");
    }
}

inline double parseDouble(const std::string& text, const std::string& what) {
    try {
        size_t used = 0;
        double value = std::stod(text, &used);
        if (used != text.size())
            throw std::invalid_argument("extra characters");
        return value;
    }
    catch (const std::exception&) {
        throw ShopException("Invalid " + what + ": '" + text + "'");
    }
}