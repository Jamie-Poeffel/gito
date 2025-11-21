#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

int init(const vector<string> &args);

bool downloadFile(const string &url, const string &filename);
