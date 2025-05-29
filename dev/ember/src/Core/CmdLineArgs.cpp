#include "Core/CmdLineArgs.h"

#include <cassert>

namespace ember {

	bool Arg::IsString() const {
		return std::holds_alternative<std::string_view>(arg);
	}
	bool Arg::IsInt() const {
		return std::holds_alternative<int64_t>(arg);
	}
	bool Arg::IsFloat() const {
		return std::holds_alternative<double>(arg);
	}

	std::string_view Arg::GetString() const {
		return std::get<std::string_view>(arg);
	}
	int64_t Arg::GetInt() const {
		return std::get<int64_t>(arg);
	}
	double Arg::GetFloat() const {
		return std::get<double>(arg);
	}

	Opt::Opt(std::string_view optName, OptType optType)
		: optName(optName), optType(optType) {
	}

	void Opt::SetValue(std::string_view strVal) {
		optVal.value().arg = strVal;
	}
	void Opt::SetValue(int64_t intVal) {
		optVal.value().arg = intVal;
	}
	void Opt::SetValue(double floatVal) {
		optVal.value().arg = floatVal;
	}

	void Opt::AddArg(const Arg& arg) {
		optArgs.push_back(arg);
	}
	void Opt::AddArg(std::string_view strVal) {
		optArgs.push_back(Arg{strVal});
	}
	void Opt::AddArg(int64_t intVal) {
		optArgs.push_back(Arg{intVal});
	}
	void Opt::AddArg(double floatVal) {
		optArgs.push_back(Arg{floatVal});
	}
	const Arg& Opt::GetArg(int idx) const {
		return optArgs[idx];
	}
	Arg& Opt::GetArg(int idx) {
		return optArgs[idx];
	}

	size_t Opt::GetArgsCount() const {
		return optArgs.size();
	}
	const std::vector<Arg>& Opt::GetArgs() const {
		return optArgs;
	}
	std::vector<Arg>& Opt::GetArgs() {
		return optArgs;
	}

	bool Opt::HasValue() const {
		return optVal.has_value();
	}
	const Arg& Opt::GetValue() const {
		return optVal.value();
	}
	Arg& Opt::GetValue() {
		return optVal.value();
	}

	std::string_view Opt::GetName() const {
		return optName;
	}
	OptType Opt::GetType() const {
		return optType;
	}

	void CmdLineArgs::AddOpt(const Opt& opt) {
		opts[opt.GetName()] = opt;
	}
	bool CmdLineArgs::HasOption(std::string_view optName) const {
		auto searchRes = opts.find(optName);
		if (searchRes == opts.end()) {
			return false;
		}
		return true;
	}
	Opt& CmdLineArgs::GetOpt(std::string_view optName) {
		auto searchRes = opts.find(optName);
		assert(searchRes != opts.end() && "You must check the existence of the option first!");
		return searchRes->second;
	}
	const Opt& CmdLineArgs::GetOpt(std::string_view optName) const {
		auto searchRes = opts.find(optName);
		assert(searchRes != opts.end() && "You must check the existence of the option first!");
		return searchRes->second;
	}

	bool CmdLineArgs::HasOptions() const {
		return !opts.empty();
	}

}