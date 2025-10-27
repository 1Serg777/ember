#pragma once

namespace ember {

#ifdef EMBER_PLATFORM_WIN32

	#define EMBER_FILE __FILE__
	#define EMBER_FUNCTION __FUNCTION__
	#define EMBER_LINE __LINE__

	#define EMBER_NODISCARD [[nodiscard]]
	#define EMBER_NOEXCEPT noexcept

#elif EMBER_PLATFORM_LINUX

	#define EMBER_FILE __FILE__
	#define EMBER_FUNCTION __FUNCTION__
	#define EMBER_LINE __LINE__

	#define EMBER_NODISCARD [[nodiscard]]
	#define EMBER_NOEXCEPT noexcept

#else

	#define EMBER_FILE __FILE__
	#define EMBER_FUNCTION ""
	#define EMBER_LINE __LINE__

	#define EMBER_NODISCARD 
	#define EMBER_NOEXCEPT noexcept

#endif

#define CLASS_DEFAULT_COPY(class_name)\
	class_name(const class_name& copy) = default;\
	class_name& operator=(const class_name& copy) = default

#define CLASS_DEFAULT_MOVE(class_name)\
	class_name(class_name&& move) = default;\
	class_name& operator=(class_name&& move) = default

#define CLASS_NO_COPY(class_name)\
	class_name(const class_name& copy) = delete;\
	class_name& operator=(const class_name& copy) = delete

#define CLASS_NO_MOVE(class_name)\
	class_name(class_name&& move) = delete;\
	class_name& operator=(class_name&& move) = delete

#define STRUCT_DEFAULT_COPY(struct_name) CLASS_DEFAULT_COPY(struct_name)

#define STRUCT_DEFAULT_MOVE(struct_name) CLASS_DEFAULT_MOVE(struct_name)

#define STRUCT_NO_COPY(struct_name) CLASS_NO_COPY(struct_name)

#define STRUCT_NO_MOVE(struct_name) CLASS_NO_MOVE(struct_name)

}