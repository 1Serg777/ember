#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>

namespace ember {

	enum class OptType {
		UNDEFINED,
		SINGLE_LETTER, // -w, -h, etc.
		LONG_NAME, // --width, --height, --long_name, --even-longer-name, etc.
	};

	enum class ArgType {
		UNDEFINED,
		STRING, // value or "value"
		INTCONST, // 18, 5 (but not 05), etc.
		FLOATCONST, // 3.14, 0.53 (but not .53), 6.0 (but not 6.), etc.
	};

	// We reuse the Arg structure for option values and option arguments.
	// As an option value: [=value], [="value"], [=2], [=5.38]
	// As an argument: [value], ["value"], [2], [5.38]
	struct Arg {
	public:
		bool IsString() const;
		bool IsInt() const;
		bool IsFloat() const;

		std::string_view GetString() const;
		int64_t GetInt() const;
		double GetFloat() const;

		std::variant<std::string_view, int64_t, double> arg;
	};

	// -o[=optval] [arg1] [arg2] ...
	// --optname[=optval] [arg1] [arg2] ...
	// --opt-name[=optval] [arg1] [arg2] ...
	// --opt_name[=optval] [arg1] [arg2] ...
	class Opt {
	public:
		Opt() = default;
		Opt(std::string_view optName, OptType optType);
		
		void SetValue(std::string_view strVal);
		void SetValue(int64_t intVal);
		void SetValue(double floatVal);

		void AddArg(const Arg& arg);
		void AddArg(std::string_view strVal);
		void AddArg(int64_t intVal);
		void AddArg(double floatVal);

		const Arg& GetArg(int idx) const;
		Arg& GetArg(int idx);

		size_t GetArgsCount() const;
		const std::vector<Arg>& GetArgs() const;
		std::vector<Arg>& GetArgs();

		bool HasValue() const;
		const Arg& GetValue() const;
		Arg& GetValue();

		std::string_view GetName() const;
		OptType GetType() const;

	private:
		std::vector<Arg> optArgs;
		std::optional<Arg> optVal;
		std::string_view optName;
		OptType optType{};
	};


	class CmdLineArgs {
	public:
		void AddOpt(const Opt& opt);
		bool HasOption(std::string_view optName) const;
		Opt& GetOpt(std::string_view optName);
		const Opt& GetOpt(std::string_view optName) const;

		bool HasOptions() const;

	private:
		std::unordered_map<std::string_view, Opt> opts;
	};

}