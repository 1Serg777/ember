#pragma once

#include "Event/Event.h"

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace ember {

	class EventCallbackStorageBase {
	public:
		virtual void NotifyAll() = 0;
		virtual void ClearAll();
		virtual void ClearEventCallbacks() = 0;
		virtual void ClearEventCallbackData() = 0;
	};

	template <typename EventCallbackData>
	class EventCallbackStorage : public EventCallbackStorageBase {
	public:
		void NotifyAll() override {
			for (auto& eventData : eventCallbackData) {
				for (auto& eventCallback : eventCallbacks) {
					eventCallback.Invoke(eventData);
				}
			}
		}

		void ClearEventCallbacks() override {
			eventCallbacks.clear();
		}
		void ClearEventCallbackData() override {
			eventCallbackData.clear();
		}

		std::vector<EventCallback<EventCallbackData>> eventCallbacks;
		std::vector<EventCallbackData> eventCallbackData;
	};

	class EventRegistry {
	public:
		void Update();

		template<typename EventCallbackData>
		int RegisterEventCallback(typename EventCallbackData::EventCallbackType eventCallback) {
			EventType type = EventCallbackData::eventType;
			int typeIdx = static_cast<int>(type);
			if (!eventCallbackStorage[typeIdx])
				CreateEventCallbackStorage<EventCallbackData>();

			EventCallbackStorage<EventCallbackData>* typedEventCallbackStorage =
				GetTypedEventCallbackStorage<EventCallbackData>();
			EventCallback<EventCallbackData> callback{ eventCallback };
			typedEventCallbackStorage->eventCallbacks.push_back(callback);
			return callback.GetCallbackId();
		}

		template<typename EventCallbackData>
		void NotifyEventCallbackImmediate(const EventCallbackData& eventCallbackData) {
			EventType type = EventCallbackData::eventType;
			int typeIdx = static_cast<int>(type);
			if (!eventCallbackStorage[typeIdx])
				return;

			EventCallbackStorage<EventCallbackData>* typedEventCallbackStorage =
				GetTypedEventCallbackStorage<EventCallbackData>();
			for (auto& eventCallback : typedEventCallbackStorage->eventCallbacks) {
				eventCallback.Invoke(eventCallbackData);
			}
		}

		template<typename EventCallbackData>
		void NotifyEventCallbackDelayed(const EventCallbackData& eventCallbackData) {
			EventType type = EventCallbackData::eventType;
			int typeIdx = static_cast<int>(type);
			if (!eventCallbackStorage[typeIdx])
				return;

			EventCallbackStorage<EventCallbackData>* typedEventCallbackStorage =
				GetTypedEventCallbackStorage<EventCallbackData>();
			typedEventCallbackStorage->eventCallbackData.push_back(eventCallbackData);
		}

	private:

		void NotifyAllCallbacks();
		void TidyCallbackData();

		template <typename EventCallbackData>
		void CreateEventCallbackStorage() {
			EventType type = EventCallbackData::eventType;
			int typeIdx = static_cast<int>(type);
			eventCallbackStorage[typeIdx] = std::make_unique<EventCallbackStorage<EventCallbackData>>();
		}

		template <typename EventCallbackData>
		EventCallbackStorage<EventCallbackData>* GetTypedEventCallbackStorage()
		{
			EventType type = EventCallbackData::eventType;
			int typeIdx = static_cast<int>(type);
			EventCallbackStorage<EventCallbackData>* typedEventCallbackStorage =
				static_cast<EventCallbackStorage<EventCallbackData>*>(eventCallbackStorage[typeIdx].get());
			return typedEventCallbackStorage;
		}

		// All callbacks are indexed by the their type id.
		static constexpr uint32_t CALLBACK_TYPE_COUNT = static_cast<uint32_t>(EventType::COUNT);
		std::array<std::unique_ptr<EventCallbackStorageBase>, CALLBACK_TYPE_COUNT> eventCallbackStorage;
	};

}