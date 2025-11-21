#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "parse.h"
#include <string>
#include <algorithm>
#include <cctype>
#include "run.h"
#include <cstdlib>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

using std::string;
using std::vector;

struct Answer
{
    string id;
    string value;
    bool has_brackets = false;
    string before = "";
};

std::string trim(const std::string &s)
{
    std::string result = s;

    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char c)
                                              { return !std::isspace(c); }));

    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char c)
                              { return !std::isspace(c); })
                     .base(),
                 result.end());

    return result;
}

string toUpper(const string &s)
{
    string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

Answer askQuestion(const output &o)
{
    string answer;
    while (true)
    {
        std::cout << trim(o.question);

        if (!o.options.empty())
        {
            string def;
            string op;
            string defult = trim(o.defau);

            if (o.options.substr(0, defult.length()) == defult)
            {
                def = toUpper(defult);

                op = op = o.options.substr(defult.length());
                std::cout << " (" << def << op << ")";
            }
            else
            {
                std::cout << " (" << o.options << ")";
            }
        }
        std::cout << ": ";

        std::getline(std::cin, answer);

        if (!answer.empty())
        {
            return {o.id, answer, trim(toUpper(o.includes)) == "BRACKETS", o.before};
        }
        else if (answer.empty() && o.optional)
        {
            return {o.id, "", false, o.before};
        }
        else if (answer.empty() && !o.defau.empty())
        {
            return {o.id, trim(o.defau), trim(toUpper(o.includes)) == "BRACKETS", o.before};
        }
    }
}

string buildString(const string &templateStr, const vector<Answer> &answers)
{
    std::string tmp = templateStr;
    std::string result = "";
    bool is_placeholder = false;
    std::string cur_plac = "";

    for (int i = 0; i < (int)tmp.length(); i++)
    {
        if (tmp[i] == '{' || is_placeholder)
        {
            is_placeholder = true;

            if (tmp[i] != '{' && tmp[i] != '}')
            {
                cur_plac = cur_plac + tmp[i];
            }

            if (tmp[i] == '}')
            {
                for (const auto &a : answers)
                {
                    if (trim(toUpper(a.id)) == trim(toUpper(cur_plac)))
                    {
                        if (a.has_brackets)
                        {
                            result = result + "(" + a.before + a.value + ")";
                        }
                        else
                        {
                            result = result + a.before + a.value;
                        }
                    }
                }

                is_placeholder = false;
                cur_plac = "";
            }

            continue;
        }

        result = result + tmp[i];
    }

    return result;
}

int parse(const vector<string> &args)
{

    std::string configPath = "./.gito/config.json";

    if (!fs::exists(configPath))
    {
        std::cout << "No config.json found. Please run init first.\n";
        return 1;
    }

    std::ifstream configFile(configPath);
    json configJson;
    configFile >> configJson;
    configFile.close();

    if (!configJson.contains("pre_commit"))
    {
        std::cout << "No pre_commit section found in config.json.\n";
        return 1;
    }

    json preCommit = configJson["pre_commit"];

    if (!preCommit["add_all"].is_null() && preCommit["add_all"].get<bool>())
    {
        std::cout << "[Running] git add .\n";
        int result = system("git add .");
        if (result != 0)
        {
            std::cout << "git add failed. Please check your repository.\n";
        }
    }

    if (!preCommit["lint_command"].is_null())
    {
        std::string lintCmd = preCommit["lint_command"].get<std::string>();
        std::cout << "[Running lint] " << lintCmd << "\n";
        int lintResult = system(lintCmd.c_str());
        if (lintResult != 0)
        {
            std::cout << "Linting failed. Aborting commit.\n";
            return 2;
        }
    }

    if (!preCommit["test_command"].is_null())
    {
        std::string testCmd = preCommit["test_command"].get<std::string>();
        std::cout << "[Running tests] " << testCmd << "\n";
        int testResult = system(testCmd.c_str());
        if (testResult != 0)
        {
            std::cout << "Tests failed. Aborting commit.\n";
            return 3;
        }
    }

    std::cout << "All selected pre-commit steps completed successfully.\n";

    vector<output> parsed = parseFile("./.gito/template.gito");
    vector<Answer> answers;

    string buildTemplate;

    for (const auto &o : parsed)
    {
        answers.push_back(askQuestion(o));
    }

    std::ifstream file("./.gito/template.gito");
    string line;
    while (std::getline(file, line))
    {
        if (line.rfind("BUILD:", 0) == 0)
        {
            if (std::getline(file, buildTemplate))
            {
                buildTemplate.erase(buildTemplate.begin(), std::find_if(buildTemplate.begin(), buildTemplate.end(), [](unsigned char ch)
                                                                        { return !std::isspace(ch); }));
                buildTemplate.erase(std::find_if(buildTemplate.rbegin(), buildTemplate.rend(), [](unsigned char ch)
                                                 { return !std::isspace(ch); })
                                        .base(),
                                    buildTemplate.end());
            }

            break;
        }
    }

    string finalOutput = buildString(buildTemplate, answers);
    std::cout << "\n"
              << finalOutput << "\n";

    string confirm;
    std::cout << "\nDo you want to commit with this message? [c]: ";
    std::getline(std::cin, confirm);
    if (confirm.empty())
        confirm = "c";

    if (confirm == "c" || confirm == "C")
    {

        std::cout << "Commit confirmed!\n";

        runCommand({"git", "commit", "-m", finalOutput});
    }
    else
        std::cout << "Commit cancelled.\n";

    return 0;
}
