#pragma once

#include <string>

class Handler {
 public:
    [[nodiscard]] static std::string handle(const std::string &request, const std::string &rootDir) ;
};
