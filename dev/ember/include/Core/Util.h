#pragma once

#include <cassert>
#include <stack>
#include <vector>

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

	// Id = 0 is considered to be invalid
	template <typename IdType>
	class SeqIdGenerator {
	public:
		SeqIdGenerator() = default;
		~SeqIdGenerator() = default;
		CLASS_NO_COPY(SeqIdGenerator);
		CLASS_DEFAULT_MOVE(SeqIdGenerator);

		IdType GenerateUniqueId() {
			if (!freeIds.empty()) {
				IdType id = freeIds.top();
				freeIds.pop();
				return id;
			}
			return idCounter++;
		}
		void FreeUniqueId(IdType id) {
			assert(IsIdValid(id) && "Invalid ID provided!");
			freeIds.push(id);
		}
		bool IsIdValid(IdType id) const {
			return (id > 0) && (id < idCounter)
		}

	private:
		IdType idCounter{1}; // always stores the next 'id' to be returned
		std::stack<IdType> freeIds;
	};

	template<typename T>
	void IdleDeleter(T* ptr) {
		// DO NOTHING
	}

	template <typename CallbackId, typename Callable>
	class CallbackStorage {
	public:
		struct Callback {
			Callable callable{};
			CallbackId id{0};
		};

		CallbackId AddCallback(Callable callable) {
			CallbackId uniqueCallbackId = idGen.GenerateUniqueId();

			Callback callback{};
			callback.id = uniqueCallbackId;
			callback.callable = callable;

			callbacks.push_back(std::move(callback));
			return uniqueCallbackId;
		}
		void RemoveCallback(CallbackId id) {
			auto pred = [id](const Callback& callback) {
				return callback.id == id;
			};
			auto begin_del_iter = std::remove_if(callbacks.begin(), callbacks.end(), pred);
			callbacks.erase(begin_del_iter, callbacks.end());
			idGen.FreeUniqueId(id);
		}

		template <class... Args>
		void Invoke(Args&&... args) const {
			for (const Callback& callback : callbacks) {
				callback.callable(std::forward<Args>(args)...);
			}
		}

	private:
		SeqIdGenerator<CallbackId> idGen;
		std::vector<Callback> callbacks;
	};

}