#pragma once

#include <string>
#include <vector>
#include <toolkit/AttributeDocument.hh>

class Worktime {
public:
	Worktime(const std::string& data_folder_path, const std::string& data_file_name);

	bool IsOpen() const
	{
		return _is_open;
	}

	enum class Command { NONE, HELP, LOGIN, LOGOUT, STATUS, REPORT, EDIT };

	struct Options {
		Options(int argc, char* argv[]);
		
		void PrintHelp() const;
		
		Command command;
		std::vector<std::string> arguments;
	};

	void ProcessOptions(const Options& opt);

private:
	bool _is_open;
	std::string _data_path;
	tk::AttributeDocument _doc;
	std::vector<void(Worktime::*)(const Options&)> _commands;
	
	void CommandHelp(const Options& opt);
	
	void CommandLogin(const Options& opt);
	
	void CommandLogout(const Options& opt);
	
	void CommandStatus(const Options& opt);
	
	void CommandReport(const Options& opt);
	
	void CommandEdit(const Options& opt);
};
