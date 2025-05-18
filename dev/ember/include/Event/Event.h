#pragma once

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

	/*
	class KeyboardKeyEventData : public EventData {
	public:
		static constexpr EventType Type = EventType::KEYBOARD_KEY;
		using Callable = std::function<void(const KeyboardKeyEventData&)>;

		KeyboardKeyEventData()
			: EventData(EventType::KEYBOARD_KEY) {}

		Keyboard::KeyCode key{ Keyboard::KeyCode::VLE_KEY_UNIDENTIFIED };
		KeyActionType action{ KeyActionType::UNIDENTIFIED };

		int scanCode{ 0 };
	};
	*/

}