#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;
using std::string;
using std::vector;

void setConfiguration(const vector<string> &args);
json readJSONFromFile(const fs::path &path);
string downloadJSON(const string &url);
