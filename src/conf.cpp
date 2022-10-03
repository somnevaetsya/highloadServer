#include "conf.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

void Conf::Parse(const std::string &filePath) {
    std::ifstream configFile;
    configFile.open(filePath, std::ios::in);
    json data = json::parse(configFile);
    cpuLimit = data["cpu_limit"];
    threadLimit = data["thread_limit"];
    documentRoot = data["document_root"];
    configFile.close();
}

int Conf::GetCpuLimit() const {
    return cpuLimit;
}

int Conf::GetThreadLimit() const {
    return threadLimit;
}

std::string Conf::GetDocumentRoot() const {
    return documentRoot;
}
