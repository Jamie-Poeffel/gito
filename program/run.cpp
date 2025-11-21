#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <sstream>

std::string runCommand(const std::vector<std::string> &args)
{
    // Build a properly quoted shell command
    std::ostringstream cmd;

    for (const auto &a : args)
    {
        cmd << "\"";

        // escape internal quotes
        for (char c : a)
        {
            if (c == '"')
                cmd << "\\\"";
            else
                cmd << c;
        }

        cmd << "\" ";
    }

    // capture stderr too
    std::string fullCmd = cmd.str() + " 2>&1";

    std::array<char, 256> buffer{};
    std::string output;

    std::unique_ptr<FILE, decltype(&pclose)>
        pipe(popen(fullCmd.c_str(), "r"), pclose);

    if (!pipe)
        return "ERROR: popen failed.";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        output += buffer.data();

    return output;
}
