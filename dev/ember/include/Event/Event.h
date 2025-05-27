#pragma once

#include "Input/Keyboard.h"

#include <functional>

namespace ember {

	enum class EventType {
		UNIDENTIFIED = -1,

		KEYBOARD_KEY = 0,

		MOUSE_BUTTON = 1,
		MOUSE_SCROLL = 2,
		MOUSE_POS = 3,

		FRAMEBUFFER_RESIZE = 4,

		WINDOW_RESIZE = 5,
		WINDOW_CLOSE = 6,

		CRITICAL_ASSETS_LOADED = 7,

		COUNT = 8
	};

	class EventData {
	public: 
		EventData(EventType eventType)
			: eventType(eventType) {}

		EventType GetCallbackType() const {
			return eventType;
		}

	protected:
		EventType eventType{ EventType::UNIDENTIFIED };
	};

	class KeyboardKeyEventData : public EventData {
	public:
		static constexpr EventType eventType = EventType::KEYBOARD_KEY;
		using EventCallbackType = std::function<void(const KeyboardKeyEventData&)>;

		KeyboardKeyEventData()
			: EventData(EventType::KEYBOARD_KEY) {}

		Keyboard::KeyCode key{ Keyboard::KeyCode::EM_KEY_UNIDENTIFIED };
		KeyActionType action{ KeyActionType::UNIDENTIFIED };

		int scancode{ 0 };
	};

	class WindowCloseEventData : public EventData {
	public:
		static constexpr EventType eventType = EventType::WINDOW_CLOSE;
		using EventCallbackType = std::function<void(const WindowCloseEventData&)>;

		WindowCloseEventData()
			: EventData(EventType::WINDOW_CLOSE) {}

		bool close{false};
	};

	class FramebufferResizeEventData : public EventData {
	public:
		static constexpr EventType eventType = EventType::FRAMEBUFFER_RESIZE;
		using EventCallbackType = std::function<void(const FramebufferResizeEventData&)>;

		FramebufferResizeEventData()
			: EventData(EventType::FRAMEBUFFER_RESIZE) {
		}

		uint32_t width{ 0 };
		uint32_t height{ 0 };
	};

	class EventCallbackBase {
	public:
		using EventCallbackId = int;

		EventCallbackBase()
			: id(GenerateUniqueId()) {}

		EventCallbackId GetCallbackId() const {
			return id;
		}

	protected:
		static EventCallbackId GenerateUniqueId() {
			static EventCallbackId uniqueId{ 0 };
			return ++uniqueId;
		}

	private:
		// id = 0 is an invalid id
		EventCallbackId id{ 0 };
	};

	template <typename EventCallbackData>
	class EventCallback : public EventCallbackBase {
	public:
		EventCallback(typename EventCallbackData::EventCallbackType callback)
			: callback(callback) {}

		void Invoke(const EventCallbackData& eventCallbackData) {
			callback(eventCallbackData);
		}

	private:
		typename EventCallbackData::EventCallbackType callback;
	};

	template<typename EventCallbackData>
	inline bool operator==(
		const EventCallback<EventCallbackData>& c1,
		const EventCallback<EventCallbackData>& c2) {
		return c1.GetCallbackId() == c2.GetCallbackId();
	}
	template<typename EventCallbackData>
	inline bool operator!=(
		const EventCallback<EventCallbackData>& c1,
		const EventCallback<EventCallbackData>& c2) {
		return !(c1 == c2);
	}

}