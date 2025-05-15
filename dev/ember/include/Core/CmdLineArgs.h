#pragma once

namespace ember {

	class CmdLineArgs {
	public:
		CmdLineArgs(int argc, char* argv[]);

	private:
		void Parse(int argc, char* argv[]);
	};

}