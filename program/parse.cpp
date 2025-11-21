#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "parse.h"

using std::string;
using std::vector;

vector<output> parseFile(const string &path)
{
    std::ifstream file(path);
    vector<output> results;
    if (!file.is_open())
    {
        std::cerr << "Cannot open file: " << path << "\n";
        return results;
    }

    string line;
    output current{};
    bool inOptions = false;
    std::stringstream optionsStream;

    while (std::getline(file, line))
    {
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch)
                                              { return !std::isspace(ch); }));
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch)
                                { return !std::isspace(ch); })
                       .base(),
                   line.end());

        if (line.empty())
            continue;

        if (line.rfind("QUESTION:", 0) == 0)
        {
            if (!current.question.empty())
            {
                current.options = optionsStream.str();
                results.push_back(current);
                current = output{};
                optionsStream.str("");
                optionsStream.clear();
                inOptions = false;
            }
            current.question = line.substr(9);
            current.question.erase(std::remove(current.question.begin(), current.question.end(), '\"'), current.question.end());
        }
        else if (line.rfind("TYPE:", 0) == 0)
        {
            current.type = line.substr(5);
            current.type.erase(std::remove(current.type.begin(), current.type.end(), '\"'), current.type.end());
        }
        else if (line.rfind("OPTIONAL:", 0) == 0)
        {
            string t = line.substr(10);
            if (t == "True")
            {
                current.optional = true;
            }
            else
            {
                current.optional = false;
            }
        }
        else if (line.rfind("ID:", 0) == 0)
        {
            current.id = line.substr(3);
            current.id.erase(std::remove(current.id.begin(), current.id.end(), '\"'), current.id.end());
        }
        else if (line.rfind("OPTIONS:", 0) == 0)
        {
            inOptions = true;
        }
        else if (line.rfind("BEFORE:", 0) == 0)
        {
            current.before = line.substr(8);
            current.before.erase(std::remove(current.defau.begin(), current.defau.end(), '\"'), current.defau.end());
        }
        else if (line.rfind("DEFAULT:", 0) == 0)
        {
            current.defau = line.substr(8);
            current.defau.erase(std::remove(current.defau.begin(), current.defau.end(), '\"'), current.defau.end());
            inOptions = false;
        }
        else if (line.rfind("INCLUDES:", 0) == 0)
        {
            current.includes = line.substr(9);
            current.includes.erase(std::remove(current.includes.begin(), current.includes.end(), '\"'), current.includes.end());
        }
        else if (inOptions)
        {
            line.erase(std::remove(line.begin(), line.end(), '\"'), line.end());
            if (!line.empty())
            {
                if (!optionsStream.str().empty())
                    optionsStream << ", ";
                optionsStream << line;
            }
        }
    }

    if (!current.question.empty())
    {
        current.options = optionsStream.str();
        if (!current.options.empty() && current.options.back() == ',')
            current.options.pop_back();
        results.push_back(current);
    }

    file.close();
    return results;
}