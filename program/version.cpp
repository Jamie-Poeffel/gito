#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <curl/curl.h>
#include <vector>
#include <nlohmann/json.hpp>
#include "config.h"

namespace fs = std::filesystem;
using json = nlohmann::json;
using std::string;
using std::vector;

fs::path getConfigPath()
{
#ifdef _WIN32
    const char *appData = std::getenv("APPDATA");
    if (!appData)
        throw std::runtime_error("APPDATA environment variable not set");
    return fs::path(appData) / "gito";
#else
    const char *home = std::getenv("HOME");
    if (!home)
        throw std::runtime_error("HOME environment variable not set");
    return fs::path(home) / ".config" / "gito";
#endif
}

void checkVersion()
{
    try
    {
        fs::path configDir = getConfigPath();
        fs::create_directories(configDir);
        fs::path localFile = configDir / "version.json";

        json localJSON;
        json remoteJSON;
        string localVersion;
        string remoteVersion;

        if (fs::exists(localFile))
        {
            localJSON = readJSONFromFile(localFile);
            localVersion = localJSON.value("version", "");
        }

        string url = "https://raw.githubusercontent.com/Jamie-Poeffel/gito/refs/heads/main/program/version.json";
        string jsonText = downloadJSON(url);
        remoteJSON = json::parse(jsonText);
        remoteVersion = remoteJSON.value("version", "");

        if (!localVersion.empty() && localVersion == remoteVersion)
        {
            std::cout << "You are running the latest version: " << localVersion << "\n";
        }
        else
        {
            std::cout << "A new version is available: " << remoteVersion << "\n";
            std::cout << "Run 'gito update gito' to update to the latest version.\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error checking version: " << e.what() << "\n";
    }
}

void getVersion(const vector<string> &args)
{
    checkVersion();
}
