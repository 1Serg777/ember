#pragma once

namespace ember {

	enum class InputDeviceType {
		UNIDENTIFIED = -1,
		MOUSE,
		KEYBOARD,
		GAMEPAD // not supported yet!
	};

	class InputDevice {
	public:
		virtual InputDeviceType GetInputDeviceType() = 0;
	};

	enum class MouseMovementType {
		SCREEN_COORDS,
		RAW_DELTA
	};

	enum class KeyActionType {
		UNIDENTIFIED = -1,
		PRESSED,
		REPEATED,
		RELEASED,
	};

	class KeyCombo {
	public:
		virtual InputDeviceType GetKeyComboInputDeviceType() = 0;
	};

}