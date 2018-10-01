#include "Configuration.hpp"
#include <fstream>
#include <iostream>

std::string Configuration::RequiredString(const std::string& key)
{
    auto valueIt = m_underlyingConfiguration.find(key);

    if (valueIt == m_underlyingConfiguration.end()) {
        Error error;
        error.m_line = 0;
        error.m_code = MissingKey;
        m_errors.push_back(error);
        return "";
    }

    return valueIt->second;
}

int Configuration::RequiredInt(const std::string& key, int minValue, int maxValue)
{
    int value;
    Error error;

    std::tie(value, error) = FindInt(key, minValue, maxValue);

    if (error.m_code == NoError)
        return value;

    m_errors.push_back(error);
    return 0;
}

std::string Configuration::OptionalString(const std::string& key, const std::string& defaultValue)
{
    auto valueIt = m_underlyingConfiguration.find(key);

    if (valueIt == m_underlyingConfiguration.end())
        return defaultValue;

    return valueIt->second;
}

int Configuration::OptionalInt(const std::string& key, int defaultValue, int minValue, int maxValue)
{
    int value;
    Error error;

    std::tie(value, error) = FindInt(key, minValue, maxValue);

    if (error.m_code == NoError)
        return value;

    if (error.m_code == MissingKey)
        return defaultValue;

    m_errors.push_back(error);
    return 0;
}

std::tuple<int, Configuration::Error> Configuration::FindInt(
    const std::string& key, int minValue, int maxValue
)
{
    Error error;
    error.m_line = 0;
    error.m_code = NoError;

    auto valueIt = m_underlyingConfiguration.find(key);

    if (valueIt == m_underlyingConfiguration.end()) {
        error.m_code = MissingKey;
        return std::make_tuple(0, error);
    }

    std::string stringValue = valueIt->second;

    size_t endIndex;
    int value = std::stoi(stringValue, &endIndex);

    if (endIndex != stringValue.length()) {
        error.m_code = InvalidValue;
        return std::make_tuple(0, error);
    }

    if (value < minValue || value > maxValue) {
        error.m_code = ValueOutOfRange;
        return std::make_tuple(0, error);
    }

    return std::make_tuple(value, error);
}

int Configuration::Load(const std::string& path)
{
    std::ifstream file(path);
    
    if (!file.is_open())
        return -1;

    size_t lineNumber = 1;

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        size_t startIndex = 0;

        while (line[startIndex] && isspace(line[startIndex]))
            ++startIndex;

        if (!line[startIndex] || line[startIndex] == '#') {
            continue;
        }
        else if (!isalpha(line[startIndex]) && line[startIndex] != '_') {
            printf("Error at line %lu.\n", lineNumber);
            continue;
        }

        size_t endIndex = startIndex;
        bool isIdentifier = true;

        while (line[endIndex] && !isspace(line[endIndex])) {
            if (!isalnum(line[endIndex]) && line[endIndex] != '_') {
                isIdentifier = false;
                break;
            }

            ++endIndex;
        }

        if (!isIdentifier) {
            printf("Error at line %lu.\n", lineNumber);
            continue;
        }

        std::string key = line.substr(startIndex, endIndex - startIndex);

        startIndex = endIndex;

        while (line[startIndex] && isspace(line[startIndex]))
            ++startIndex;

        if (!line[startIndex]) {
            printf("Error at line %lu.\n", lineNumber);
            continue;
        }

        endIndex = line.length() - 1;

        while (isspace(line[endIndex]))
            --endIndex;

        std::string value = line.substr(startIndex, endIndex - startIndex + 1);

        m_underlyingConfiguration[key] = value;

        ++lineNumber;
    }

    return 0;
}
