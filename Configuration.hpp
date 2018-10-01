#ifndef HttpServer_Configuration_Hpp
#define HttpServer_Configuration_Hpp
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <limits.h>

class Configuration {
public:
    enum ErrorCode {
        NoError,
        MissingKey,
        InvalidValue,
        ValueOutOfRange
    };

    struct Error {
        /*
         * If zero, then the error isn't located in the file.
         * Instead, the error is either a MissingKey or an
         * InvalidValue.
         */
        size_t m_line;
        ErrorCode m_code;
    };

public:
    Configuration();
    Configuration(const Configuration& c) = delete;
    ~Configuration();

    std::string RequiredString(const std::string& key);
    int RequiredInt(const std::string& key, int minValue = INT_MIN, int maxValue = INT_MAX);

    std::string OptionalString(const std::string& key, const std::string& defaultValue);

    int OptionalInt(
        const std::string& key, int defaultValue, int minValue = INT_MIN, int maxValue = INT_MAX
    );

    const std::vector<Error>& Errors() const;

    int Load(const std::string& path);

private:
    typedef std::tuple<std::string, std::string> KeyValuePair;

    std::tuple<int, Error> FindInt(const std::string& key, int minValue, int maxValue);

private:
    std::unordered_map<std::string, std::string> m_underlyingConfiguration;
    std::vector<Error> m_errors;
};

inline Configuration::Configuration() {}

inline Configuration::~Configuration() {}

inline const std::vector<Configuration::Error>& Configuration::Errors() const
{ return m_errors; }

#endif
