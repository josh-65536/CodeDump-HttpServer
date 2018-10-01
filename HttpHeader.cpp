#include "HttpHeader.hpp"
#include "picohttpparser.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Placeholder code from GitHub */
int HttpHeader::ParseHttpRequest(int fd)
{
    char buffer[4096];
    char* method;
    char* path;
    int parseResult;
    int minorVersion;
    phr_header headers[100];
    size_t bufferLength = 0;
    size_t prevBufferLength = 0;
    size_t methodLength;
    size_t pathLength;
    size_t headerCount;

    for (;;) {
        ssize_t result;
        do result = read(fd, buffer + bufferLength, sizeof(buffer) - bufferLength);
        while (result == -1 && errno == EINTR);

        if (result <= 0) {
            printf("Unable to read\n");
            return -1;
        }

        prevBufferLength = bufferLength;
        bufferLength += result;

        headerCount = sizeof(headers) / sizeof(headers[0]);
        parseResult = phr_parse_request(
            buffer,
            bufferLength,
            (const char**) &method,
            &methodLength,
            (const char**) &path,
            &pathLength,
            &minorVersion,
            headers,
            &headerCount,
            prevBufferLength
        );

        if (parseResult > 0) {
            break;
        }
        else if (parseResult = -1) {
            printf("HTTP Parse Error\n");
            return -1;
        }

        assert(parseResult == -2);

        if (bufferLength == sizeof(buffer)) {
            printf("Request is too long\n");
            return -1;
        }
    }

    if (!strcmp(method, "OPTIONS"))
        m_method = HttpMethod_Options;
    else if (!strcmp(method, "GET"))
        m_method = HttpMethod_Get;
    else if (!strcmp(method, "HEAD"))
        m_method = HttpMethod_Head;
    else if (!strcmp(method, "POST"))
        m_method = HttpMethod_Post;
    else if (!strcmp(method, "PUT"))
        m_method = HttpMethod_Put;
    else if (!strcmp(method, "DELETE"))
        m_method = HttpMethod_Delete;
    else if (!strcmp(method, "TRACE"))
        m_method = HttpMethod_Trace;
    else if (!strcmp(method, "CONNECT"))
        m_method = HttpMethod_Connect;
    else
        m_method = HttpMethod_Unknown;

    m_path = std::string(path, pathLength);

    printf("\n");
    printf("    Request is %d bytes long\n", parseResult);
    printf("    Method is %.*s\n", static_cast<int>(methodLength), method);
    printf("    Path is %.*s\n", static_cast<int>(pathLength), path);
    printf("    HTTP version is 1.%d\n", minorVersion);
    printf("    Headers:\n");

    for (int i = 0; i < headerCount; ++i) {
        printf(
            "        %.*s: %.*s\n",
            static_cast<int>(headers[i].name_len),
            headers[i].name,
            static_cast<int>(headers[i].value_len),
            headers[i].value
        );
    }

    printf("\n\n");

    return 0;
}
