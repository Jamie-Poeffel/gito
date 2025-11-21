#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <curl/curl.h>
#include <vector>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;
using std::string;
using std::vector;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string *str = static_cast<std::string *>(stream);
    str->append(static_cast<char *>(ptr), size * nmemb);
    return size * nmemb;
}

std::string downloadJSON(const string &url)
{
    CURL *curl = curl_easy_init();
    std::string result;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
            throw std::runtime_error("Failed to download JSON");
    }
    return result;
}

json readJSONFromFile(const fs::path &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open JSON file: " + path.string());

    json j;
    file >> j;
    return j;
}

fs::path getGlobalConfigPath()
{
#ifdef _WIN32
    const char *appData = std::getenv("APPDATA");
    if (!appData)
        throw std::runtime_error("APPDATA environment variable not set");
    return fs::path(appData) / "gito" / "config.json";
#else
    const char *home = std::getenv("HOME");
    if (!home)
        throw std::runtime_error("HOME environment variable not set");
    return fs::path(home) / ".config" / "gito" / "config.json";
#endif
}

fs::path getLocalConfigPath()
{
    return fs::current_path() / ".gito" / "config.json";
}

void setConfiguration(const vector<string> &args)
{
    if (args.size() < 2)
    {
        std::cerr << "Usage: gito --config [-g] key value\n";
        return;
    }

    bool global = false;
    size_t argIndex = 0;

    if (args[0] == "-g")
    {
        global = true;
        argIndex = 1;
    }

    if (argIndex + 1 >= args.size())
    {
        std::cerr << "Usage: gito --config [-g] key value\n";
        return;
    }

    string key = args[argIndex];
    string value = args[argIndex + 1];

    fs::path configPath = global ? getGlobalConfigPath() : getLocalConfigPath();

    fs::create_directories(configPath.parent_path());

    json config;
    if (fs::exists(configPath))
    {
        try
        {
            config = readJSONFromFile(configPath);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Warning: failed to read existing config, starting fresh: "
                      << e.what() << "\n";
            config = json::object();
        }
    }
    else
    {
        config = json::object();
    }

    json *current = &config;
    size_t start = 0, pos;
    while ((pos = key.find('.', start)) != string::npos)
    {
        string part = key.substr(start, pos - start);
        start = pos + 1;

        if (!current->contains(part) || !(*current)[part].is_object())
            (*current)[part] = json::object();

        current = &(*current)[part];
    }

    string finalKey = key.substr(start);
    (*current)[finalKey] = value;

    std::ofstream out(configPath);
    if (!out.is_open())
    {
        std::cerr << "Error: could not open config file for writing: "
                  << configPath << "\n";
        return;
    }
    out << config.dump(4);

    std::cout << (global ? "Global" : "Local")
              << " config updated: " << key << " = " << value << "\n";
}
