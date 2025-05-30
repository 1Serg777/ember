#include "Core/CmdLineArgsParser.h"

#include <array>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

namespace ember {

	static constexpr std::array<std::string_view, 2> gpuApis{
		cmdopt::gpuApiOpenglVal,
		cmdopt::gpuApiVulkanVal,
	};
#ifdef EMBER_PLATFORM_WIN32
	static constexpr std::array<std::string_view, 2> windowApis{
		cmdopt::windowApiGlfwVal,
		cmdopt::windowApiWin32Val,
	};
#elif EMBER_PLATFORM_LINUX
	static constexpr std::array<std::string_view, 4> windowApis{
		cmdopt::windowApiGlfwVal,
		cmdopt::windowApiXlibVal,
		cmdopt::windowApiXcbVal,
		cmdopt::windowApiWaylandVal,
	};
#endif
	static constexpr std::array<std::string_view, 2> onOffOpts{
		cmdopt::optOnVal,
		cmdopt::optOffVal,
	};

	static constexpr std::array<int64_t, 2> intOpts{
		cmdopt::intZeroVal,
		cmdopt::intOneVal,
	};
	static constexpr std::array<double, 3> floatOpts{
		cmdopt::floatZeroVal,
		cmdopt::floatHalfVal,
		cmdopt::floatOneVal,
	};

	const static std::unordered_map<std::string_view, OptReqs> optReqs{
		{cmdopt::gpuApiOpt, OptReqs{true, ArgType::STRING, gpuApis.data(), 2, ArgType::UNDEFINED, 0}},
#ifdef EMBER_PLATFORM_WIN32
		{cmdopt::windowApiOpt, OptReqs{true, ArgType::STRING, windowApis.data(), 2, ArgType::UNDEFINED, 0}},
#elif EMBER_PLATFORM_LINUX
		{cmdopt::windowApiOpt, OptReqs{true, ArgType::STRING, windowApis.data(), 4, ArgType::UNDEFINED, 0}},
#endif
		{cmdopt::windowWidthOpt, OptReqs{true, ArgType::INTCONST, nullptr, 0, ArgType::UNDEFINED, 0}},
		{cmdopt::windowHeightOpt, OptReqs{true, ArgType::INTCONST, nullptr, 0, ArgType::UNDEFINED, 0}},
		{cmdopt::windowMinWidthOpt, OptReqs{true, ArgType::INTCONST, nullptr, 0, ArgType::UNDEFINED, 0}},
		{cmdopt::windowMinHeightOpt, OptReqs{true, ArgType::INTCONST, nullptr, 0, ArgType::UNDEFINED, 0}},

		{cmdopt::fullscreenOpt, OptReqs{true, ArgType::STRING, onOffOpts.data(), 2, ArgType::UNDEFINED, 0}},
		{cmdopt::visibleOpt, OptReqs{true, ArgType::STRING, onOffOpts.data(), 2, ArgType::UNDEFINED, 0}},
		{cmdopt::resizableOpt, OptReqs{true, ArgType::STRING, onOffOpts.data(), 2, ArgType::UNDEFINED, 0}},

		{cmdopt::numIntTestOpt, OptReqs{true, ArgType::INTCONST, intOpts.data(), 2, ArgType::UNDEFINED, 0}},
		{cmdopt::numFloatTestOpt, OptReqs{true, ArgType::FLOATCONST, floatOpts.data(), 3, ArgType::UNDEFINED, 0}},
	};

	void CmdLineArgsLexer::Scan(int argc, char** argv) {
		ClearState();
		this->argc = argc;
		this->argv = argv;
		argcCurrent = 0;
		argvCurrent = this->argv[argcCurrent];
		argvCurrentCharIdx = argvStartCharIdx = 0;
		while (!IsAtEndCurrentArgc()) {
			ScanCurrentArgv();
			argcCurrent++;
			argvCurrent = this->argv[argcCurrent];
			argvCurrentCharIdx = argvStartCharIdx = 0;
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
		this->argvStartCharIdx = 0;
		this->argvCurrentCharIdx = 0;
	}

	void CmdLineArgsLexer::ScanCurrentArgv() {
		while (!IsAtEndCurrentArgv()) {
			ScanToken();
			argvStartCharIdx = argvCurrentCharIdx;
		}
	}
	void CmdLineArgsLexer::ScanToken() {
		char c = Advance();
		switch (c) {
			case '-':
				AddToken(TokenType::DASH);
				break;
			case '=':
				AddToken(TokenType::EQUAL);
				break;
			case '"':
				String();
				break;
			case '.':
				if (IsNumeric(Peek())) {
					PutBack();
					Number();
				} else {
					throw std::runtime_error{"Unknown sequence of characters starting a token!"};
				}
				break;
			default:
				if (IsAlpha(c)) {
					Identifier();
				} else if (IsNumeric(c)) {
					Number();
				} else {
					throw std::runtime_error{"Unknown character starting a token!"};
				}
				break;
		}
	}
	void CmdLineArgsLexer::String() {
		while (!IsAtEndCurrentArgv() && Peek() != '"') {
			Advance();
		}
		Consume('"', "Unterminated string encountered!");
		AddToken(TokenType::STRING);
	}
	void CmdLineArgsLexer::Number() {
		while (IsNumeric(Peek())) {
			Advance();
		}
		if (Match('.')) {
			while (IsNumeric(Peek())) {
				Advance();
			}
			AddToken(TokenType::FLOATCONST);
		} else {
			AddToken(TokenType::INTCONST);
		}
	}
	void CmdLineArgsLexer::Identifier() {
		while (IsAlphaNumeric(Peek())) {
			Advance();
		}
		AddToken(TokenType::IDENTIFIER);
	}

	Token CmdLineArgsLexer::CreateToken() const {
		Token token{};
		token.type = TokenType::UNDEFINED;
		token.lexeme = std::string_view{
			argvCurrent + argvStartCharIdx, argvCurrentCharIdx - argvStartCharIdx
		};
		return token;
	}
	Token CmdLineArgsLexer::CreateToken(TokenType tokenType) const {
		Token token{};
		token.type = tokenType;
		token.lexeme = std::string_view{
			argvCurrent + argvStartCharIdx, argvCurrentCharIdx - argvStartCharIdx
		};
		return token;
	}
	void CmdLineArgsLexer::AddToken(TokenType tokenType) {
		tokens.push_back(CreateToken(tokenType));
	}
	void CmdLineArgsLexer::AddToken(const Token& token) {
		tokens.push_back(token);
	}

	bool CmdLineArgsLexer::IsAtEndCurrentArgc() const {
		return argcCurrent >= argc;
	}
	bool CmdLineArgsLexer::IsAtEndCurrentArgv() const {
		return argvCurrent[argvCurrentCharIdx] == '\0';
	}

	char CmdLineArgsLexer::Peek() const {
		if (IsAtEndCurrentArgv())
			return '\0';
		return argvCurrent[argvCurrentCharIdx];
	}
	char CmdLineArgsLexer::PeekNext() const {
		if (IsAtEndCurrentArgv() || argvCurrent[argvCurrentCharIdx + 1] == '\0')
			return '\0';
		return argvCurrent[argvCurrentCharIdx + 1];
	}
	char CmdLineArgsLexer::Advance() {
		if (IsAtEndCurrentArgc())
			return '\0';
		return argvCurrent[argvCurrentCharIdx++];
	}
	void CmdLineArgsLexer::PutBack() {
		assert(argvCurrentCharIdx != 0 && "Calling 'PutBack' before advancing once is not allowed!");
		argvCurrentCharIdx--;
	}
	bool CmdLineArgsLexer::Match(char c) {
		if (Peek() == c) {
			Advance();
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

	void CmdLineArgsParser::Parse(int argc, char** argv) {
		PrintArgcArgv(argc, argv);
		CreateArg0Path(argv[0]);
		lexer.Scan(argc - 1, argv + 1);
		ParseTokens();
		PrintTokens();
	}
	void CmdLineArgsParser::PrintHelp() {
		// TODO
	}

	const CmdLineArgs& CmdLineArgsParser::GetCmdLineArgs() const {
		return cmdLineArgs;
	}

	void CmdLineArgsParser::CreateArg0Path(char* path) {
		cmdLineArgs.SetExePath(std::filesystem::path{path});
	}

	void CmdLineArgsParser::ParseTokens() {
		current = 0;
		while (!IsAtEnd()) {
			try {
				ParseOption();
			} catch (std::runtime_error& re) {
				std::cerr << re.what() << std::endl;
				Synchronize();
			}
		}
	}
	void CmdLineArgsParser::ParseOption() {
		OptType optType = OptType::UNDEFINED;
		Consume(TokenType::DASH, "The '-' option start character expected!");
		optType = OptType::SINGLE_LETTER;
		if (Match(TokenType::DASH)) {
			optType = OptType::LONG_NAME;
		}
		// 1. Option name.
		const Token* optId = Consume(TokenType::IDENTIFIER, "Option identifier expected!");
		// 2. Option requirements.
		auto optReqSearchRes = optReqs.find(optId->lexeme);
		if (optReqSearchRes == optReqs.end()) {
			throw std::runtime_error{"Unknown option encountered!"};
		}
		const OptReqs& optReq = optReqSearchRes->second;
		Opt opt{optId->lexeme, optType};
		// 3. Option value.
		if (optReq.optValUsed) {
			ParseOptionValue(opt, optReq);
		}
		// 4. Arguments
		if (optReq.argCount != 0) {
			ParseArguments(opt, optReq);
		}
		cmdLineArgs.AddOpt(opt);
	}
	void CmdLineArgsParser::ParseOptionValue(Opt& opt, const OptReqs& optReq) {
		Consume(TokenType::EQUAL, "The '=' sign is expected before the option value.");
		ParseValue(opt, optReq, ValKind::OPTION_VALUE);
	}
	void CmdLineArgsParser::ParseArguments(Opt& opt, const OptReqs& optReq) {
		for (int i = 0; i < optReq.argCount; i++) {
			ParseValue(opt, optReq, ValKind::OPTION_ARGUMENT);
		}
	}

	template<typename T>
	bool IsValueAllowed(T value, const Opt& opt, const OptReqs& optReq) {
		// std::is_integral_v<T> ||
		// std::is_floating_point_v<T>
		static_assert(
			std::is_same_v<T, std::string_view> ||
			std::is_same_v<T, int64_t> ||
			std::is_same_v<T, double>,
			"IsValueAllowed() only supports std::string_view, int64_t, or double"
			);
		if (optReq.allowedValListCount == 0)
			return true;
		const T* allowedList = static_cast<const T*>(optReq.allowedValList);
		for (int i = 0; i < optReq.allowedValListCount; i++) {
			if (value == allowedList[i]) {
				return true;
			}
		}
		return false;
	}
	void CmdLineArgsParser::ParseValue(Opt& opt, const OptReqs& optReq, ValKind valKind) {
		ArgType valType;
		if (valKind == ValKind::OPTION_VALUE)
			valType = optReq.optValType;
		else if (valKind == ValKind::OPTION_ARGUMENT)
			valType = optReq.argType;
		else
			throw std::runtime_error{"Unexpected value type provided!"};

		if (valType == ArgType::STRING) {
			std::string_view strVal;
			if (Match(TokenType::IDENTIFIER)) {
				const Token* strTok = Previous();
				strVal = strTok->lexeme;
			} else if (Match(TokenType::STRING)) {
				const Token* strTok = Previous();
				// Remove the double quotes and start after the first one.
				strVal = std::string_view{strTok->lexeme.data() + 1, strTok->lexeme.size() - 2};
			} else {
				throw std::runtime_error{"String or identifier expected!"};
			}
			if (!IsValueAllowed(strVal, opt, optReq)) {
				throw std::runtime_error{"String value is not allowed!"};
			}
			SetValue(opt, strVal, valKind);
		} else if (valType == ArgType::INTCONST) {
			int64_t intVal;
			if (Match(TokenType::INTCONST)) {
				const Token* intTok = Previous();
				intVal = std::strtol(intTok->lexeme.data(), nullptr, 10);
			} else if (Match(TokenType::FLOATCONST)) {
				const Token* floatTok = Previous();
				double floatVal = std::strtod(floatTok->lexeme.data(), nullptr);
				intVal = static_cast<int64_t>(floatVal);
				PrintFloatToIntCastWarningMsg(opt, intVal, floatVal);
			} else {
				throw std::runtime_error{"Integer or float (narrowing cast) expected!"};
			}
			if (!IsValueAllowed(intVal, opt, optReq)) {
				throw std::runtime_error{"Integer value is not allowed!"};
			}
			SetValue(opt, intVal, valKind);
		} else if (valType == ArgType::FLOATCONST) {
			double floatVal;
			if (Match(TokenType::FLOATCONST)) {
				const Token* floatTok = Previous();
				floatVal = std::strtod(floatTok->lexeme.data(), nullptr);
			} else if (Match(TokenType::INTCONST)) {
				const Token* intTok = Previous();
				int64_t intVal = std::strtol(intTok->lexeme.data(), nullptr, 10);
				floatVal = static_cast<double>(intVal);
				PrintIntToFloatCastWarningMsg(opt, floatVal, intVal);
			} else {
				throw std::runtime_error{"Float or integer (narrowing cast) expected!"};
			}
			if (!IsValueAllowed(floatVal, opt, optReq)) {
				throw std::runtime_error{"Float value is not allowed!"};
			}
			SetValue(opt, floatVal, valKind);
		}
	}
	void CmdLineArgsParser::SetValue(Opt& opt, std::string_view strVal, ValKind valKind) {
		if (valKind == ValKind::OPTION_VALUE)
			opt.SetValue(strVal);
		else if (valKind == ValKind::OPTION_ARGUMENT)
			opt.AddArg(strVal);
	}
	void CmdLineArgsParser::SetValue(Opt& opt, int64_t intVal, ValKind valKind) {
		if (valKind == ValKind::OPTION_VALUE)
			opt.SetValue(intVal);
		else if (valKind == ValKind::OPTION_ARGUMENT)
			opt.AddArg(intVal);
	}
	void CmdLineArgsParser::SetValue(Opt& opt, double floatVal, ValKind valKind) {
		if (valKind == ValKind::OPTION_VALUE)
			opt.SetValue(floatVal);
		else if (valKind == ValKind::OPTION_ARGUMENT)
			opt.AddArg(floatVal);
	}

	void CmdLineArgsParser::Synchronize() {
		while (!IsAtEnd() && (Peek()->type != TokenType::DASH)) {
			Advance();
		}
	}

	const Token* CmdLineArgsParser::Peek() const {
		if (IsAtEnd())
			return Previous();
		return lexer.GetTokenData() + current;
	}
	const Token* CmdLineArgsParser::Advance() {
		if (IsAtEnd())
			return Previous();
		return lexer.GetTokenData() + current++;
	}
	const Token* CmdLineArgsParser::Previous() const {
		return lexer.GetTokenData() + (current - 1);
	}
	const Token* CmdLineArgsParser::Consume(TokenType type, std::string_view errMsg) {
		if (!Match(type)) {
			throw std::runtime_error{errMsg.data()};
		}
		return Previous();
	}

	bool CmdLineArgsParser::Match(TokenType type) {
		if (Peek()->type == type) {
			Advance();
			return true;
		}
		return false;
	}
	bool CmdLineArgsParser::IsAtEnd() const {
		return current >= lexer.GetTokenCount();
	}

	void CmdLineArgsParser::PrintArgcArgv(int argc, char** argv) {
		std::cout << "Command Line Arguments:\n";
		for (int i = 0; i < argc; i++) {
			std::cout << "Arg #" << i << ": " << argv[i] << "\n";
		}
	}
	void CmdLineArgsParser::PrintTokens() {
		std::cout << "[Command Line Arguments] Tokens:\n";
		for (size_t i = 0; i < lexer.GetTokenCount(); i++) {
			const Token* token = lexer.GetTokenData() + i;
			std::cout << "{" << token->lexeme << "} ";
		}
		std::cout << std::endl;
	}

	void CmdLineArgsParser::PrintOptionQualifier(std::ostream& os, OptType optType) {
		if (optType == OptType::SINGLE_LETTER) {
			os << "-";
		} else if (optType == OptType::LONG_NAME) {
			os << "--";
		}
	}
	void CmdLineArgsParser::PrintFloatToIntCastWarningMsg(const Opt& opt, int64_t intVal, double floatVal) {
		std::cerr << "[Warning] Command line option '";
		PrintOptionQualifier(std::cerr, opt.GetType());
		std::cerr << opt.GetName() << "'" << " expects integer option value while float encountered!\n";
		std::cerr << "Casting '" << floatVal << "' to '" << intVal << "'!" << std::endl;
	}
	void CmdLineArgsParser::PrintIntToFloatCastWarningMsg(const Opt& opt, double floatVal, int64_t intVal) {
		std::cerr << "[Warning] Command line option '";
		PrintOptionQualifier(std::cerr, opt.GetType());
		std::cerr << opt.GetName() << "'" << " expects floating-point option value, while int encountered!\n";
		std::cerr << "Casting '" << intVal << "' to '" << floatVal << "'!" << std::endl;
	}

}