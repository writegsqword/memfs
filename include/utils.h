#pragma once

#include <string>
#include <vector>

inline std::vector<std::string> string_split(const std::string& s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}


inline path_chunks_t get_path_chunks(const std::string& path) {
    auto path_chunks_v = string_split(path, "/");
    path_chunks_t path_chunks;
    for(std::string& s : path_chunks_v){
        if(s.length() > 0)
            path_chunks.push_front(s);
    }
    return path_chunks;

}