#pragma once

#include <string>
#include <vector>
#include "parse.h"

using std::string;
using std::vector;

string askQuestion(const output &o);

string buildString(const string &templateStr, const vector<std::pair<string, string>> &answers);

int parse(const vector<string> &args);
