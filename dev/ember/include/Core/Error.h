#pragma once

#include "Core/Util.h"

#include <stdexcept>
#include <string>

namespace ember
{
#define GLFW_ERROR(message)\
	throw GLFWError(EMBER_FILE, EMBER_FUNCTION, EMBER_LINE, message)

	class EmberError : public std::exception
	{
	public:

		EmberError(const char* message)
			: std::exception(), errorMessage(message) {}
		EmberError(const std::string& message)
			: std::exception(), errorMessage(message) {}

		EMBER_NODISCARD char const* what() const override
		{
			return errorMessage.c_str();
		}

	private:

		std::string errorMessage;
	};

	class GLFWError : public EmberError
	{
	public:

		GLFWError();
		GLFWError(const char* message);
		GLFWError(const char* file, const char* function, uint32_t line, const char* message);

		virtual ~GLFWError() EMBER_NOEXCEPT = default;

	private:

		std::string CreateGLFWErrorMessage(
			const char* file, const char* function, uint32_t line, const char* message) const;
	};
}