#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;
namespace fs = std::filesystem;
using json = nlohmann::json;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::ofstream *out = static_cast<std::ofstream *>(stream);
    size_t written = 0;
    if (out->is_open())
    {
        out->write(static_cast<char *>(ptr), size * nmemb);
        written = size * nmemb;
    }
    return written;
}

bool askUser(const std::string &question)
{
    std::string answer;
    while (true)
    {
        std::cout << question << " (y/n): ";
        std::getline(std::cin, answer);
        if (answer == "y" || answer == "Y")
            return true;
        if (answer == "n" || answer == "N")
            return false;
        std::cout << "Please type 'y' or 'n'.\n";
    }
}

bool downloadFile(const std::string &url, const std::string &filename)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return false;

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open())
        return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    outfile.close();

    return res == CURLE_OK;
}

int init(const vector<string> &args)
{
    string dir = "./.gito";

    if (!fs::exists(dir))
    {
        try
        {
            fs::create_directory(dir);
        }
        catch (const fs::filesystem_error &)
        {
            std::cout << "Error creating directory.\n";
            return 2;
        }
    }

    string url = "https://raw.githubusercontent.com/Jamie-Poeffel/gito/refs/heads/main/template/template.gito";
    string filename = "./.gito/template.gito";

    if (downloadFile(url, filename))
    {
        std::cout << "File downloaded successfully.\n";
    }
    else
    {
        std::cout << "Download failed.\n";
        return 1;
    }

    json preCommitConfig;

    if (askUser("Do you want to add 'git add .' reminder before commit?"))
    {
        preCommitConfig["add_all"] = true;
    }

    if (askUser("Do you want to add a linting command before commit?"))
    {
        std::string lintCmd;
        std::cout << "Enter lint command (example: eslint . --fix): ";
        std::getline(std::cin, lintCmd);
        preCommitConfig["lint_command"] = lintCmd;
    }

    if (askUser("Do you want to add a test command before commit?"))
    {
        std::string testCmd;
        std::cout << "Enter test command (example: npm test): ";
        std::getline(std::cin, testCmd);
        preCommitConfig["test_command"] = testCmd;
    }

    json configJson;

    configJson["pre_commit"]["add_all"] = preCommitConfig.contains("add_all") ? preCommitConfig["add_all"] : nullptr;
    configJson["pre_commit"]["lint_command"] = preCommitConfig.contains("lint_command") ? preCommitConfig["lint_command"] : nullptr;
    configJson["pre_commit"]["test_command"] = preCommitConfig.contains("test_command") ? preCommitConfig["test_command"] : nullptr;

    std::ofstream configFile("./.gito/config.json");
    configFile << configJson.dump(4);
    configFile.close();

    std::cout << "Created config.json with all pre-commit keys (null for unselected steps).\n";
    return 0;
}
