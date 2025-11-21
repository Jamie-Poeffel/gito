#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "program/version.h"
#include "program/init.h"
#include "program/main.h"
#include "program/config.h"

using namespace std;

struct Command
{
	string name;
	string help;
	function<void(const vector<string> &)> func;
};

void showHelp(const vector<string> &args, const vector<Command> &commands);
void getVersion(const vector<string> &args);
void doCommit(const vector<string> &args);
void gitoCommand(const vector<string> &args);
int init(const vector<string> &args);
void setConfiguration(const vector<string> &args);

int main(int argc, char *argv[])
{
	vector<Command> commands;

	commands = {
		{"", "Gito commit template", gitoCommand},
		{"help", "Show this help message", [&](const vector<string> &args)
		 {
			 showHelp(args, commands);
		 }},
		{"version", "Show program version", getVersion},
		{"commit", "Commit changes", doCommit},
		{"init", "Initialize template", init},
		{"--config", "<-g> for global config <key> <value>", setConfiguration}};

	string commandName;
	vector<string> arguments;

	if (argc < 2)
	{
		commandName = "";
	}
	else
	{
		commandName = argv[1];
		for (int i = 2; i < argc; ++i)
		{
			arguments.push_back(argv[i]);
		}
	}

	for (const auto &cmd : commands)
	{
		if (cmd.name == commandName)
		{
			cmd.func(arguments);
			return 0;
		}
	}

	cout << "Unknown command: " << commandName << "\n";
	cout << "Use 'help' to see available commands.\n";
	return 1;
}

void gitoCommand(const vector<string> &args)
{
	parse(args);
}

void showHelp(const vector<string> &args, const vector<Command> &commands)
{
	cout << "Available commands:\n";
	for (const auto &cmd : commands)
	{
		if (cmd.name.empty())
			continue;
		cout << "  " << cmd.name << " - " << cmd.help << "\n";
	}
}

void doCommit(const vector<string> &args)
{
	cout << "Committing changes...\n";
	for (size_t i = 0; i < args.size(); ++i)
	{
		cout << "Arg[" << i << "]: " << args[i] << "\n";
	}
}
