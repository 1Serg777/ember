#include "Core/Error.h"

#include <sstream>

namespace ember
{
	// GLFWError

	GLFWError::GLFWError()
		: EmberError("Unknown GLFW error")
	{
	}

	GLFWError::GLFWError(const char* message)
		: EmberError(message)
	{
	}

	GLFWError::GLFWError(const char* file, const char* function, uint32_t line, const char* message)
		: EmberError(CreateGLFWErrorMessage(file, function, line, message))
	{
	}

	std::string GLFWError::CreateGLFWErrorMessage(
		const char* file, const char* function, uint32_t line, const char* message) const
	{
		std::stringstream errorStream;

		errorStream << "********** GLFW ERROR **********\n"
			<< "[FILE]: " << file << "\n"
			<< "[FUNCTION]: " << function << "\n"
			<< "[LINE]: " << line << "\n"
			<< "[MESSAGE]: " << message;

		return errorStream.str();
	}
}