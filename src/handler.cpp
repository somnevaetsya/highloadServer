#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <vector>

#include "handler.h"

std::map<std::string, std::string> mimeType = {
        {".html", "text/html"},
        {".js", "application/javascript"},
        {".css", "text/css"},
        {".swf","application/x-shockwave-flash"},
        {".jpg", "image/jpeg"},
        {".jpeg","image/jpeg"},
        {".png","image/png"},
        {".gif", "image/gif"},
};

std::string Handler::handle(const std::string &request, const std::string& rootDir) {
    std::stringstream reqStream(request);
    std::string line;
    std::getline(reqStream, line);

    std::stringstream requestLine(line);
    std::string method;
    std::getline(requestLine, method, ' ');

    std::string url;

    std::getline(requestLine, url, ' ');

    if (url.find("../") != std::string::npos) {
        return "HTTP/1.1 404 Not Found\r\nServer: somnevaetsya\r\n";
    }
    bool is_head = false;
    if (method != "HEAD" && method != "GET") {
        return "HTTP/1.1 405 Method Not Implemented\r\nServer: somnevaetsya\r\n";
    }
    if (method == "HEAD") {
        is_head = true;
    }

    std::string version;
    std::getline(requestLine, version, ' ');

    std::string decoded_url;
    for (int i = 0; i < url.size(); i++) {
        if (url[i] == '%') {
            decoded_url += static_cast<char>(strtoll(url.substr(i + 1, 2).c_str(), nullptr, 16));
            i += 2;
        } else {
            decoded_url += url[i];
        }
    }
    url = decoded_url;

    std::vector<std::string> tokenize;
    std::string buf = url;
    std::string del = "/";
    uint64_t start = 0;
    uint64_t end = buf.find(del);
    while (end != -1) {
        tokenize.push_back(url.substr(start, end - start));
        start = end + del.size();
        end = buf.find(del, start);
    }
    tokenize.push_back(url.substr(start, end - start));

    auto tokens = tokenize;
    auto last = tokens[tokens.size() - 1];
    auto checkParams = last.rfind('?');
    if (checkParams != -1) {
        last = last.substr(0, checkParams);
    }
    std::string newPath;
    for (auto i = 0; i < tokens.size() - 1; i++) {
        newPath += tokens[i] + "/";
    }
    newPath += last;
    url = newPath;
    url = rootDir + url;

    url.insert(0, "..");
    bool is_dir = false;
    if (std::filesystem::is_directory(url)) {
        url += "index.html";
        is_dir = true;
    }

    std::fstream targetFile;
    targetFile.open(url, std::ios::in);
    if (!targetFile.is_open()) {
        if (is_dir) {
            return "HTTP/1.1 403 Forbidden\r\nServer: somnevaetsya\r\n\r\n";
        }
        return "HTTP/1.1 404 Not Found\r\nServer: somnevaetsya\r\n";
    }

    std::ifstream file(url, std::ios::in | std::ios::binary);
    std::stringstream fl;
    if (file.is_open()) {
        fl << file.rdbuf();
    }
    file.close();

    auto res = fl.str();

    std::time_t rawTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string date = ctime(&rawTime);
    std::string formatedDate = date.substr(0, 3) + ", " + date.substr(8, 3) + date.substr(4, 4) + date.substr(20, 4)
                               + " " + date.substr(11, 8);

    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Server: somnevaetsya\r\n"
                           "Date: " + formatedDate + "\r\n";
    auto length = res.size();

    response = response + "Content-Length:" + " " + std::to_string(length);;
    if (!is_head) {
        if (std::filesystem::is_regular_file(url)) {
            response += "\r\nContent-Type: ";
            std::string getType;
            getType = url.substr(url.rfind('.'), url.size());
            if (mimeType.find(getType) == mimeType.end()) {
                response += "none";
            } else {
                response += mimeType[getType];
            }
        }
        response += "\r\n\r\n" + res;
    } else {
        response += "\r\n\r\n";
    }

    return response;
}
