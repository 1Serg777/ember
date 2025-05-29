#include "Core/CmdLineArgsParser.h"

#include <array>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace ember {

	void CmdLineArgsLexer::Scan(int argc, char** argv) {
		ClearState();
		this->argc = argc;
		this->argv = argv;
		argcCurrent = 0;
		argvCurrent = this->argv[argcCurrent];
		argvCurrentCharIdx = 0;
		while (!IsAtEndArgc()) {
			ScanToken();
			AdvanceArgc();
		}
	}

	const Token* CmdLineArgsLexer::GetTokenData() const {
		return tokens.data();
	}
	size_t CmdLineArgsLexer::GetTokenCount() const {
		return tokens.size();
	}

	void CmdLineArgsLexer::ClearState() {
		this->argv = nullptr;
		this->argvCurrent = nullptr;
		this->argc = 0;
		this->argcCurrent = 0;
		this->argvCurrentCharIdx = 0;
	}

	void CmdLineArgsLexer::ScanToken() {
		char c = Advance();
		switch (c) {
			case '-':
				Option();
			break;
			case '"':
				String();
			break;
			default:
				if (IsNumeric(c)) {
					Number();
				} else if (IsAlpha(c)) {
					Identifier();
				} else {
					throw std::runtime_error{"Unknown character starting a token!"};
				}
			break;
		}
	}
	void CmdLineArgsLexer::Option() {
		OptType optType{OptType::SINGLE_LETTER};
		if (Match('-')) {
			optType = OptType::LONG_NAME;
		}
		if (!IsAlpha(Peek())) {
			throw std::runtime_error{ "An option must start with a letter!" };
		}
		while (IsAlphaNumeric(Peek())) {
			Advance();
		}
		std::string_view optName{argvCurrent, argvCurrentCharIdx};
		if (Match('=')) {
			int start = argvCurrentCharIdx;
			if (IsAlpha(Peek())) {
				while (IsAlphaNumeric(Peek())) {
					Advance();
				}
				std::string_view strOptValue{argvCurrent + start, argvCurrentCharIdx - start};
			} else if (IsNumeric(Peek())) {
				// TODO: numeric option value.
			} else if (Peek() == '"') {
				// TODO: string option value.
			}
		}
		Token optToken{};
		optToken.lexeme = std::string_view{argvCurrent, argvCurrentCharIdx};
		optToken.type = TokenType::OPTION;
		optToken.symbolTableId = 0; // TODO
		tokens.push_back(optToken);
	}
	void CmdLineArgsLexer::String() {
		// TODO
	}
	void CmdLineArgsLexer::Number() {
		// TODO
	}
	void CmdLineArgsLexer::Identifier() {
		// TODO
	}

	bool CmdLineArgsLexer::IsAtEndArgc() const {
		return argcCurrent >= argc;
	}
	bool CmdLineArgsLexer::IsAtEndArgv() const {
		if (argvCurrent[argvCurrentCharIdx] == '\0')
			return true;
		return false;
	}

	int CmdLineArgsLexer::AdvanceArgc() {
		return argcCurrent++;
	}

	char CmdLineArgsLexer::Advance() {
		if (IsAtEndArgv())
			return '\0';
		return argvCurrent[argvCurrentCharIdx++];
	}
	char CmdLineArgsLexer::Peek() {
		if (IsAtEndArgv())
			return '\0';
		return argvCurrent[argvCurrentCharIdx];
	}
	bool CmdLineArgsLexer::Match(char c) {
		if (Peek() == c) {
			return true;
		}
		return false;
	}
	char CmdLineArgsLexer::Previous() {
		return argvCurrent[argvCurrentCharIdx - 1];
	}
	char CmdLineArgsLexer::Consume(char c, std::string_view errMsg) {
		if (Peek() != c) {
			throw std::runtime_error{errMsg.data()};
		}
		return Advance();
	}

	bool CmdLineArgsLexer::IsAlpha(char c) {
		if (c >= 'A' && c <= 'Z' ||
			c >= 'a' && c <= 'z') {
			return true;
		}
		return false;
	}
	bool CmdLineArgsLexer::IsNumeric(char c) {
		if (c >= '0' && c <= '9') {
			return true;
		}
		return false;
	}
	bool CmdLineArgsLexer::IsAlphaNumeric(char c) {
		if (IsAlpha(c) || IsNumeric(c) || c == '_' || c == '-') {
			return true;
		}
		return false;
	}

	std::array<char*, 2> gpuApis{
		"opengl",
		"vulkan",
	};
#ifdef EMBER_PLATFORM_WIN32
	std::array<char*, 2> windowApis{
		"glfw",
		"win32",
	};
#elif EMBER_PLATFORM_LINUX
	std::array<char*, 4> windowApis{
		"glfw",
		"xlib",
		"xcb",
		"wayland",
	};
#endif
	std::array<char*, 2> onOffOpts{
		"on",
		"off",
	};

	const static std::unordered_map<std::string_view, OptReqs> optReqs{
		{"gpuapi", OptReqs{OptValueReq::REQUIRED, ArgType::INTCONST, nullptr, 0, 0, ArgType::UNDEFINED}},
#ifdef EMBER_PLATFORM_WIN32
		{"windowapi", OptReqs{OptValueReq::REQUIRED, ArgType::STRING, windowApis.data(), 2, 0, ArgType::UNDEFINED}},
#elif EMBER_PLATFORM_LINUX
		{"windowapi", OptReqs{OptValueReq::REQUIRED, ArgType::STRING, windowApis.data(), 4, 0, ArgType::UNDEFINED}},
#endif
		{"window-width", OptReqs{OptValueReq::REQUIRED, ArgType::INTCONST, nullptr, 0, 0, ArgType::UNDEFINED}},
		{"window-height", OptReqs{OptValueReq::REQUIRED, ArgType::INTCONST, nullptr, 0, 0, ArgType::UNDEFINED}},
		{"window-min-width", OptReqs{OptValueReq::REQUIRED, ArgType::INTCONST, nullptr, 0, 0, ArgType::UNDEFINED}},
		{"window-min-height", OptReqs{OptValueReq::REQUIRED, ArgType::INTCONST, nullptr, 0, 0, ArgType::UNDEFINED}},

		{"fullscreen", OptReqs{OptValueReq::REQUIRED, ArgType::STRING, onOffOpts.data(), 2, 0, ArgType::UNDEFINED}},
		{"visible", OptReqs{OptValueReq::REQUIRED, ArgType::STRING, onOffOpts.data(), 2, 0, ArgType::UNDEFINED}},
		{"resizable", OptReqs{OptValueReq::REQUIRED, ArgType::STRING, onOffOpts.data(), 2, 0, ArgType::UNDEFINED}},
	};

	void CmdLineArgsParser::Parse(int argc, char** argv) {
		PrintArgcArgv(argc, argv);
		Scan(argc, argv);
		ParseTokens();
		PrintTokens();
	}
	void CmdLineArgsParser::PrintHelp() {
		// TODO
	}

	const CmdLineArgs& CmdLineArgsParser::GetCmdLineArgs() const {
		return cmdLineArgs;
	}

	void CmdLineArgsParser::Scan(int argc, char** argv) {
		// TODO	
	}
	void CmdLineArgsParser::ParseTokens() {
		// TODO
	}

	void CmdLineArgsParser::PrintArgcArgv(int argc, char** argv) {
		std::cout << "Command Line Arguments:\n";
		for (int i = 0; i < argc; i++) {
			std::cout << "Arg #" << i << ": " << argv[i] << "\n";
		}
	}
	void CmdLineArgsParser::PrintTokens() {
		// TODO
	}

}