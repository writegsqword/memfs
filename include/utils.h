#pragma once

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& so, const std::string& delimiter) {
    std::string s = so;
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}