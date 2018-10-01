#ifndef HttpServer_HttpHeader_Hpp
#define HttpServer_HttpHeader_Hpp
#include <string>

enum HttpMethod {
    HttpMethod_Unknown,
    HttpMethod_Options,
    HttpMethod_Get,
    HttpMethod_Head,
    HttpMethod_Post,
    HttpMethod_Put,
    HttpMethod_Delete,
    HttpMethod_Trace,
    HttpMethod_Connect
};

class HttpHeader {
public:
    HttpHeader();
    HttpHeader(const HttpHeader& hh);
    ~HttpHeader();

    int ParseHttpRequest(int fd);

    HttpMethod Method() const;
    std::string Path() const;

private:
    HttpMethod m_method;
    std::string m_path;
};

inline HttpHeader::HttpHeader() :
    m_method(HttpMethod_Unknown)
{}

inline HttpHeader::HttpHeader(const HttpHeader& hh) :
    m_method(hh.m_method),
    m_path(hh.m_path)
{}

inline HttpHeader::~HttpHeader() {}

inline HttpMethod HttpHeader::Method() const
{ return m_method; }

inline std::string HttpHeader::Path() const
{ return m_path; }

#endif
