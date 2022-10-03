#pragma once
#include <iostream>

class Conf {
public:
    void Parse(const std::string& filePath);
    int GetCpuLimit() const;
    int GetThreadLimit() const;
    std::string GetDocumentRoot() const;
private:
    int cpuLimit;
    int threadLimit;
    std::string documentRoot;
};

