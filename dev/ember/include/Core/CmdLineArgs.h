#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>

namespace ember {

	// When defining a new command line option, don't forget to
	// define its requirements in the 'CmdLineArgs.cpp' file!
	namespace cmdopt {
		// Options

		constexpr std::string_view gpuApiOpt{"gpu-api"};
		constexpr std::string_view windowApiOpt{"window-api"};

		constexpr std::string_view windowWidthOpt{"window-width"};
		constexpr std::string_view windowHeightOpt{"window-height"};
		constexpr std::string_view windowMinWidthOpt{"window-min-width"};
		constexpr std::string_view windowMinHeightOpt{"window-min-height"};

		constexpr std::string_view fullscreenOpt{"fullscreen"};
		constexpr std::string_view visibleOpt{"visible"};
		constexpr std::string_view resizableOpt{"resizable"};

		constexpr std::string_view numIntTestOpt{"num-int-test"};
		constexpr std::string_view numFloatTestOpt{"num-float-test"};

		// Option values

		constexpr std::string_view gpuApiOpenglVal{"opengl"};
		constexpr std::string_view gpuApiVulkanVal{"vulkan"};

		constexpr std::string_view windowApiGlfwVal{"glfw"};
#ifdef EMBER_PLATFORM_WIN32
		constexpr std::string_view windowApiWin32Val{"win32"};
#elif EMBER_PLATFORM_LINUX
		constexpr std::string_view windowApiXlibVal{"xlib"};
		constexpr std::string_view windowApiXcbVal{"xcb"};
		constexpr std::string_view windowApiWaylandVal{"wayland"};
#endif
		constexpr std::string_view optOnVal{"on"};
		constexpr std::string_view optOffVal{"off"};

		constexpr int64_t intZeroVal{0};
		constexpr int64_t intOneVal{1};

		constexpr double floatZeroVal{0.0};
		constexpr double floatHalfVal{0.5};
		constexpr double floatOneVal{1.0};
	}


	enum class OptType {
		UNDEFINED,
		SINGLE_LETTER, // -w, -h, etc.
		LONG_NAME, // --width, --height, --long_name, --even-longer-name, etc.
	};

	enum class ArgType {
		UNDEFINED,
		STRING, // value or "value"
		INTCONST, // 18, 5, 001, etc.
		FLOATCONST, // 3.14, 0.53, .31, 6.0, 8., etc.
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