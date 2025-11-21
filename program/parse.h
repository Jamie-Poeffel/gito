#pragma once

#include <string>
#include <vector>

struct output
{
    std::string question;
    std::string type;
    std::string id;
    std::string options;
    std::string defau;
    std::string includes;
    std::string before;
    bool optional;
};

std::vector<output> parseFile(const std::string &path);
