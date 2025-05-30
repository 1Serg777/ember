#pragma once

#include "Core/CmdLineArgs.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

namespace ember {

	enum class TokenType {
		UNDEFINED,
		OPTION,
		EQUAL,
		DASH,
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

		void ScanCurrentArgv();
		void ScanToken();
		void String();
		void Number();
		void Identifier();

		Token CreateToken() const;
		Token CreateToken(TokenType tokenType) const;
		void AddToken(TokenType tokenType);
		void AddToken(const Token& token);

		bool IsAtEndCurrentArgc() const;
		bool IsAtEndCurrentArgv() const;

		char Peek() const;
		char PeekNext() const;
		char Advance();
		void PutBack();
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
		uint32_t argvStartCharIdx{0};
		uint32_t argvCurrentCharIdx{0};
	};

	enum class ValKind {
		OPTION_VALUE,
		OPTION_ARGUMENT,
	};

	// Option requirements for the Parser to check each option against.
	// Option values can have a list of allowed values, whereas arguments cannot.
	// An option can take variable number of arguments, but each must be of the same type.
	struct OptReqs {
		bool optValUsed{false};
		ArgType optValType{}; // not used if 'optValUsed' is false.
		const void* optValAllowedList{nullptr}; // not used if 'optValAllowedListCount' is 0.
		int optValAllowedListCount{0};
		ArgType argTypeReq{}; // not used if 'argCount' is 0.
		int argCount{0};
	};

	class CmdLineArgsParser {
	public:
		void Parse(int argc, char** argv);
		void PrintHelp();

		const CmdLineArgs& GetCmdLineArgs() const;

	private:
		void CreateArg0Path(char* path);

		void ParseTokens();
		void ParseOption();
		void ParseOptionValue(Opt& opt, const OptReqs& optReq);
		void ParseArguments(Opt& opt, const OptReqs& optReq);

		void ParseValue(Opt& opt, ArgType argType, ValKind valKind);
		void SetValue(Opt& opt, std::string_view strArg, ValKind valKind);
		void SetValue(Opt& opt, int64_t intArg, ValKind valKind);
		void SetValue(Opt& opt, double floatArg, ValKind valKind);

		bool IsOptionValueAllowed(const Opt& opt, const OptReqs& optReq);
		bool IsStringOptionValueAllowed(const Opt& opt, const OptReqs& optReq);
		bool IsIntOptionValueAllowed(const Opt& opt, const OptReqs& optReq);
		bool IsFloatOptionValueAllowed(const Opt& opt, const OptReqs& optReq);

		void Synchronize();

		const Token* Peek() const;
		const Token* Advance();
		const Token* Previous() const;
		const Token* Consume(TokenType type, std::string_view errMsg);

		bool Match(TokenType type);
		bool IsAtEnd() const;

		void PrintArgcArgv(int argc, char** argv);
		void PrintTokens();

		void PrintOptionQualifier(std::ostream& os, OptType optType);
		void PrintFloatToIntCastWarningMsg(const Opt& opt, int64_t intVal, double floatVal);
		void PrintIntToFloatCastWarningMsg(const Opt& opt, double floatVal, int64_t intVal);

		CmdLineArgs cmdLineArgs;
		CmdLineArgsLexer lexer;
		std::filesystem::path arg0;
		int current{0};
	};

}