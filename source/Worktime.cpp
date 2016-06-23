#include "Worktime.hpp"
#include <toolkit/FileSystem.hh>
#include <toolkit/AttributeDocument.hh>

#include "date.hpp"
#include "Time.hpp"

tk::Time PrintTaskInfo(const tk::Attribute& task)
{
	tk::Time begin(0, 0, 0);
	tk::Time end(0, 0, 0);
	bool is_active = false;

	if (task.GetName() == "Task") {
		if (task.Has("login")) {
			begin = tk::Time(task["login"].GetValue<std::string>());

			if (task.Has("logout")) {
				end = tk::Time(task["logout"].GetValue<std::string>());
			}
			// End time is current if task is still active.
			else {
				end = tk::Time();
				is_active = true;
			}
		}
	}

	const tk::Time diff = (end - begin);

	const int t_id = task["id"].GetValue<int>();

	if (t_id < 10) {
		tk::console::Write("           ", tk::console::Color::Yellow, t_id, " ");
	}
	else {
		tk::console::Write("          ", tk::console::Color::Yellow, t_id, " ");
	}

	// Time interval.
	tk::console::Write(tk::console::Color::Green, diff.ToString(), tk::console::Color::Magenta, " [",
		begin.ToStringHM(), " - ", end.ToStringHM(), "] ", tk::console::Color::Reset);

	if (task.Has("description")) {
		tk::console::Write(task["description"].GetValue<std::string>());
	}

	if (is_active) {
		tk::console::Print("[", tk::console::Color::BoldGreen, "ACTIVE", tk::console::Color::Reset, "]");
	}
	else {
		tk::console::Write("\n");
	}

	return diff;
}

Worktime::Options::Options(int argc, char* argv[])
{
	if (argc < 2) {
		tk::console::Error("Not enough argument.");
		command = Command::NONE;
		return;
	}

	const std::string args(argv[1]);

	if (args == "login") {
		command = Command::LOGIN;
	}
	else if (args == "logout") {
		command = Command::LOGOUT;
	}
	else if (args == "status") {
		command = Command::STATUS;
	}
	else if (args == "report") {
		command = Command::REPORT;
	}
	else if (args == "edit") {
		command = Command::EDIT;
	}
	else {
		tk::console::Error("Invalid option.");
		command = Command::NONE;
		return;
	}

	if (argc > 2) {
		//				description = std::string(argv[2]);

		for (int i = 2; i < argc; i++) {
			arguments.push_back(argv[i]);
		}
	}
}

void Worktime::Options::PrintHelp() const
{
	std::vector<std::string> wt_img = {
	
	"                    _    _   _",
	"                   | |  | | (_)",
	"__      _____  _ __| | _| |_ _ _ __ ___   ___",
	"\\ \\ /\\ / / _ \\| '__| |/ / __| | '_ ` _ \\ / _ \\",
	" \\ V  V / (_) | |  |   <| |_| | | | | | |  __/",
	"  \\_/\\_/ \\___/|_|  |_|\\_\\\\__|_|_| |_| |_|\\___|\n"};
	
	
	for(auto& n : wt_img) {
		tk::console::CPrint(tk::console::Color::BoldMagenta, n);
	}
	
	tk::console::Print("Terminal utility to keep track of working hours.\n");
	
	tk::console::Print("Usage    : worktime [command] [args..]");
	tk::console::Print("Commands :");
	
	tk::console::Print("         * login  : Start a new task (starting now).");
	tk::console::Print("                    worktime login \"task description\"");
	tk::console::Print("         * logout : Logout from current task.");
	tk::console::Print("         * status : Print today's work time.");
	tk::console::Print("         * report : List all created tasks.");
	tk::console::Print("         * edit   : Edit task informations.");
	tk::console::Print("                  - label   : Change task label.");
	tk::console::Print("                              worktime edit label date task_id new_task_label");
	tk::console::Print("                         ex : worktime edit label 2016-02-10 2 \"My task label\"");
}

Worktime::Worktime(const std::string& data_folder_path, const std::string& data_file_name)
	: _is_open(false)
{
	// Connect command callbacks.
	_commands.resize(static_cast<int>(Command::EDIT) + 1);
	_commands[static_cast<int>(Command::NONE)] = &Worktime::CommandHelp;
	_commands[static_cast<int>(Command::HELP)] = &Worktime::CommandHelp;
	_commands[static_cast<int>(Command::LOGIN)] = &Worktime::CommandLogin;
	_commands[static_cast<int>(Command::LOGOUT)] = &Worktime::CommandLogout;
	_commands[static_cast<int>(Command::STATUS)] = &Worktime::CommandStatus;
	_commands[static_cast<int>(Command::REPORT)] = &Worktime::CommandReport;
	_commands[static_cast<int>(Command::EDIT)] = &Worktime::CommandEdit;

	tk::os::Path folder_path(data_folder_path, tk::os::Path::Type::DIRECTORY, false);

	// If folder path doesn't exist.
	if (!folder_path.IsValid()) {
		tk::console::Error("Can't create folder", folder_path.GetPath());
		return;
	}

	_data_path = folder_path.GetPath() + "/" + data_file_name;

	tk::os::Path path(_data_path);
	_doc = tk::AttributeDocument(_data_path, tk::AttributeDocument::Type::JSON);

	// Create new document is file doesn't exist.
	if (!path.IsValid()) {

		if (!_doc.IsOpen()) {
			tk::console::Error("Can't create data_file", _data_path);
			return;
		}

		_doc.GetMainAttribute().Add(tk::Attribute("TimeSheet", tk::Attribute::Type::OBJECT));
	}

	_is_open = true;
}

void Worktime::ProcessOptions(const Options& opt)
{
	(this->*(_commands[static_cast<int>(opt.command)]))(opt);
}

void Worktime::CommandHelp(const Options& opt)
{
	opt.PrintHelp();
}

void Worktime::CommandLogin(const Options& opt)
{
	auto& tm_sheet = _doc.GetMainAttribute()["TimeSheet"];

	std::pair<tk::Date, tk::Time> date_time;
	const std::string date(date_time.first.ToString());
	const std::string time(date_time.second.ToString());

	// Check if date already exist.
	if (tm_sheet.Has(date)) {
		tk::Attribute& today = tm_sheet[date];

		tk::Attribute& last_task = today.Back();

		// If last task is close.
		if (last_task.Has("logout")) {

			// Create new task.
			tk::Attribute task("Task");

			// Last task id.
			const int tid = last_task["id"].GetValue<int>();

			task.Add(tk::Attribute::Create<int>("id", tid + 1));

			if (!opt.arguments.empty()) {
				task.Add(tk::Attribute::Create<std::string>("description", opt.arguments[0]));
			}

			task.Add(tk::Attribute::Create<std::string>("login", time));
			today.Add(std::move(task));
		}
		// Task is already login.
		else {
			// Ask to close task and start a new one.
			tk::console::CPrint("Last task is still active.");
			tk::console::Write("Would you like to logout and create a new one ? (yes | ",
				tk::console::Color::BoldBlack, "no", tk::console::Color::Reset, ") : ");

			std::string answer;
			std::getline(std::cin, answer);

			if (!(answer == "yes" || answer == "YES" || answer == "y" || answer == "Y")) {
				tk::console::Print("No changes were made.");
			}

			// Logout and create a new task.
			last_task.Add(tk::Attribute::Create<std::string>("logout", time));

			// Create new task.
			tk::Attribute task("Task");

			// Last task id.
			const int tid = last_task["id"].GetValue<int>();

			task.Add(tk::Attribute::Create<int>("id", tid + 1));

			if (!opt.arguments.empty()) {
				task.Add(tk::Attribute::Create<std::string>("description", opt.arguments[0]));
			}

			task.Add(tk::Attribute::Create<std::string>("login", time));
			today.Add(std::move(task));
		}
	}

	// Create new date and new task.
	else {
		tk::Attribute today(date);
		{
			tk::Attribute task("Task");
			task.Add(tk::Attribute::Create<int>("id", 0));
			task.Add(tk::Attribute::Create<std::string>("login", time));

			if (!opt.arguments.empty()) {
				task.Add(tk::Attribute::Create<std::string>("description", opt.arguments[0]));
			}

			today.Add(std::move(task));
		}
		tm_sheet.Add(std::move(today));
	}

	_doc.Save(_data_path, tk::AttributeDocument::Type::JSON);
}

void Worktime::CommandLogout(const Options& opt)
{
	auto& tm_sheet = _doc.GetMainAttribute()["TimeSheet"];

	std::pair<tk::Date, tk::Time> date_time;
	const std::string date(date_time.first.ToString());
	const std::string time(date_time.second.ToString());

	if (!tm_sheet.Has(date)) {
		tk::console::Print("User is in not login yet.");
		return;
	}

	tk::Attribute& today = tm_sheet[date];
	tk::Attribute& last_task = today.Back();

	if (last_task.Has("logout")) {
		tk::console::Print("There is no open task to logout from.");
		return;
	}

	last_task.Add(tk::Attribute::Create<std::string>("logout", time));

	_doc.Save(_data_path, tk::AttributeDocument::Type::JSON);
}

void Worktime::CommandStatus(const Options& opt)
{
	auto& tm_sheet = _doc.GetMainAttribute()["TimeSheet"];

	std::pair<tk::Date, tk::Time> date_time;
	const std::string date(date_time.first.ToString());
	const std::string time(date_time.second.ToString());

	if (!tm_sheet.Has(date)) {
		tk::console::Print("No task was log today.");
		return;
	}

	// Sum all task time.
	tk::Attribute& today = tm_sheet[date];

	// Print date and full format date.
	tk::console::CPrint(tk::console::Color::BoldBlack, today.GetName(), ":", tk::console::Color::Cyan, "(",
		tk::Date(today.GetName()).ToStringFull(), ")");

	tk::Time total_time(0, 0, 0);

	for (auto& task : today.GetAttributes()) {
		if (task.GetName() == "Task") {
			total_time += PrintTaskInfo(task);
		}
	}

	tk::console::CPrint(tk::console::Color::BoldBlack, "     Total :", total_time.ToString());
}

void Worktime::CommandReport(const Options& opt)
{
	auto& tm_sheet = _doc.GetMainAttribute()["TimeSheet"];

	std::pair<tk::Date, tk::Time> date_time;
	const std::string date(date_time.first.ToString());
	const std::string time(date_time.second.ToString());

	tk::Time total_time(0, 0, 0);

	// For each day in time sheet.
	for (auto& day : tm_sheet.GetAttributes()) {
		tk::Time total_day_time(0, 0, 0);

		// Print date and full format date.
		tk::console::CPrint(tk::console::Color::BoldBlack, day.GetName(), ":", tk::console::Color::Cyan, "(",
			tk::Date(day.GetName()).ToStringFull(), ")");

		// Print all tasks info and sum total day time.
		for (auto& task : day.GetAttributes()) {
			if (task.GetName() == "Task") {
				total_day_time += PrintTaskInfo(task);
			}
		}

		tk::console::CPrint(tk::console::Color::BoldBlack, "     Total :", total_day_time.ToString());
		total_time += total_day_time;
	}
}

void Worktime::CommandEdit(const Options& opt)
{
	tk::console::Print("EDIT");

	if (opt.arguments.size() != 4) {
		tk::console::Error("Invalid edit arguments.");
		opt.PrintHelp();
		return;
	}

	// Label -> [label, date, id, new_description].

	if (opt.arguments[0] == "label") {
		/// @todo Tru catch.

		const tk::Date date(opt.arguments[1]);
		const int id = std::stoi(opt.arguments[2]);
		const std::string& description = opt.arguments[3];

		auto& tm_sheet = _doc.GetMainAttribute()["TimeSheet"];

		if (!tm_sheet.Has(date.ToString())) {
			tk::console::Error("Date doesn't exist.");
			return;
		}

		auto& day = tm_sheet[date.ToString()];

		if (!day.IsValid()) {
			tk::console::Error("Date doesn't exist.");
			return;
		}

		if (!day.Has("Task")) {
			tk::console::Error("Date doesn't have any task registered.");
			return;
		}

		for (auto& task : day.GetAttributes()) {
			if (task.GetName() == "Task") {
				if (task["id"].GetValue<int>() == id) {

					if (task.Has("description")) {
						task["description"].SetValue<std::string>(description);
					}
					else {
						task.Add(tk::Attribute::Create<std::string>("description", description));
					}

					_doc.Save(_data_path, tk::AttributeDocument::Type::JSON);
					return;
				}
			}
		}

		tk::console::Error("Unvalid task id.");
		opt.PrintHelp();
	}
	else {
		tk::console::Error("Invalid edit arguments.");
		opt.PrintHelp();
		return;
	}
}