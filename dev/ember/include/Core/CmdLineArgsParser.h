#pragma once

#include "Core/CmdLineArgs.h"

#include <string_view>
#include <vector>

namespace ember {

	enum class TokenType {
		OPTION,
		IDENTIFIER,
		STRING,
		INTCONST,
		FLOATCONST
	};

	struct Token {
		std::string_view lexeme;
		TokenType type{};
		int symbolTableId{};
	};

	class CmdLineArgsLexer {
	public:
		void Scan(int argc, char** argv);

		const Token* GetTokenData() const;
		size_t GetTokenCount() const;

	private:
		void ClearState();

		void ScanToken();
		void Option();
		void String();
		void Number();
		void Identifier();

		bool IsAtEndArgc() const;
		bool IsAtEndArgv() const;

		int AdvanceArgc();

		char Advance();
		char Peek();
		bool Match(char c);
		char Previous();
		char Consume(char c, std::string_view errMsg);

		bool IsAlpha(char c);
		bool IsNumeric(char c);
		bool IsAlphaNumeric(char c);

		std::vector<Token> tokens;

		char** argv{nullptr};
		char* argvCurrent{nullptr};
		int argc{0};
		int argcCurrent{0};
		uint32_t argvCurrentCharIdx{0};
	};

	// Option value requirements.
	enum class OptValueReq {
		EMPTY,
		REQUIRED
	};
	// Option requirements for the Parser to check each option against.
	// Option values can
	struct OptReqs {
		OptValueReq optValReq{OptValueReq::EMPTY};
		ArgType optValTypeReq{}; // not used if 'optValReq' is EMPTY.
		char** allowedOptValList{nullptr}; // not used if 'allowedOptValListCount' is 0.
		int allowedOptValListCount{0};
		int argCount{0};
		ArgType argTypeReq{}; // not used if the 'argCount' is 0.
	};

	class CmdLineArgsParser {
	public:
		void Parse(int argc, char** argv);
		void PrintHelp();

		const CmdLineArgs& GetCmdLineArgs() const;

	private:
		void Scan(int argc, char** argv);
		void ParseTokens();

		void PrintArgcArgv(int argc, char** argv);
		void PrintTokens();

		CmdLineArgsLexer lexer;
		CmdLineArgs cmdLineArgs;

		int current{0};
	};

}