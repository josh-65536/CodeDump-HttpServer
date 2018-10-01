#include "Configuration.hpp"
#include "HttpHeader.hpp"

#include "picohttpparser.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

static const char g_configPath[] = "httpd.conf";

static std::string g_rootDirectory;

static int g_connectionTimeout;

static const std::string g_error500 =
    "<html><head></head><body><h1>500 Internal Server Error</h1></body></html>";

static const char g_sendBuffer[] =
"HTTP/1.1 200 OK\n"
"Date: Mon, 23 May 2004 22:38:34 GMT\n"
"Content-Type: text/html; charset=UTF-8\n"
"Content-Encoding: UTF-8\n"
"Content-Length: 79\n"
"Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT\n"
"Server: HTTP Server/0.0.1 (Windows) (Windows NT)\n"
"ETag: \"3f80f-1b6-3e1cb03b\"\n"
"Accept-Ranges: bytes\n"
"Connection: close\n"
"\n"
"<html><head><title>Foo</title></head><body><h1>Hello World!</h1></body></html>";

static int SetupConfiguration()
{
    Configuration configuration;

    if (configuration.Load("httpd.conf")) {
        printf("Unable to load configuration.\n");
        return -1;
    }

    g_rootDirectory = configuration.RequiredString("RootDirectory");
    g_connectionTimeout = configuration.RequiredInt("ConnectionTimeout");

    for (const Configuration::Error& error : configuration.Errors())
    {
        if (error.m_code != Configuration::NoError)
            printf("Error at line %lu: %d\n", error.m_line, error.m_code);
    }

    return 0;
}

static std::string LoadWebPage(const std::string& path)
{
    std::ifstream file(path);
    
    if (!file.is_open()) {
        printf("Unable to load file.\n");
        return "<html><head></head><body><h1>404 Not Found</h1></body></html>";
    }

    std::ostringstream content;
    content << file.rdbuf();

    return content.str();
}

static void EndConnection(int client)
{
    if (shutdown(client, SHUT_RDWR) == -1)
        perror("shutdown()");

    close(client);
}

static std::string ServePageData(const std::string& actualPath)
{
    return LoadWebPage(actualPath);
}

static std::string ServePagePHP(const std::string& actualPath)
{
    constexpr size_t ByteRate = 80;

    int pipeFD[2];

    if (pipe(pipeFD) == -1) {
        perror("pipe()");
        return g_error500;
    }

    int readEnd = pipeFD[0];
    int writeEnd = pipeFD[1];

    pid_t processID = fork();

    if (processID == -1) {
        perror("fork()");
        return g_error500;
    }

    if (processID) {
        close(writeEnd);

        std::string content;
        content.resize(65536);

        size_t length = 0;

        for (;;) {
            if (length >= content.length())
                content.resize(content.length() * 3 / 2);

            ssize_t bytesRead = read(readEnd, &content[length], ByteRate);

            if (!bytesRead)
                break;

            if (bytesRead == -1) {
                perror("read()");
                return g_error500;
            }

            length += bytesRead;
        }

        content.resize(length);

        return content;
    }
    else {
        close(1); /* 0 = stdin, 1 = stdout, 2 = stderr */

        if (dup2(writeEnd, 1) == -1) {
            perror("dup2()");
            exit(-1);
        }

        close(readEnd);
        execlp("php", "php", "-f", actualPath.c_str(), reinterpret_cast<char*>(NULL));
    }
}

static int ServePage(int client)
{
    HttpHeader header;

    if (header.ParseHttpRequest(client) == -1) {
        printf("Unable to parse request\n");
        EndConnection(client);
        return -1;
    }

    std::string path = header.Path();

    if (path == "/")
        path = "/index.php";

    std::string actualPath = g_rootDirectory + path;
    std::string extension;

    size_t dotIndex = actualPath.rfind('.');

    if (dotIndex != std::string::npos)
        extension = actualPath.substr(dotIndex + 1);

    std::string page;

    if (extension == "php")
        page = ServePagePHP(actualPath);
    else
        page = ServePageData(actualPath);

    ssize_t bytesSent = send(client, page.c_str(), page.length() + 1, 0);

    if (bytesSent == -1) {
        perror("send()");
        EndConnection(client);
        return -1;
    }

    EndConnection(client);
    return 0;
}

int main(int argc, char** argv)
{
    SetupConfiguration();

    unsigned listener = socket(AF_INET, SOCK_STREAM, 0);

    if (listener == -1) {
        perror("Error: ");
        return -1;
    }

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(80);
    address.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(listener, (const sockaddr*) &address, sizeof(address)) == -1) {
        perror("bind");
        return -1;
    }

    if (listen(listener, SOMAXCONN) == -1) {
        perror("listen()");
        return -1;
    }

    for (;;) {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen;
        int client = accept(listener, (sockaddr*) &clientAddr, &clientAddrLen);

        if (client == -1) {
            perror("accept()");
            continue;
        }

        printf("Connected to %s\n", inet_ntoa(clientAddr.sin_addr));

        pid_t processID = fork();

        if (processID == -1) {
            perror("fork()");
            continue;
        }

        if (!processID)
            return ServePage(client);
    }

    close(listener);
    return 0;
}
