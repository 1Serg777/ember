#include "Input/Keyboard.h"

namespace ember {

	Keyboard::Keyboard() {
		InitializeKeyNameMap();
	}

	InputDeviceType Keyboard::GetInputDeviceType() {
		return InputDeviceType::KEYBOARD;
	}

	void Keyboard::SetButtonState(Keyboard::KeyCode keycode, KeyActionType action) {
		if (action == KeyActionType::PRESSED || action == KeyActionType::REPEATED)
			keysState.set(static_cast<size_t>(keycode), true);
		else if (action == KeyActionType::RELEASED)
			keysState.set(static_cast<size_t>(keycode), false);
	}
	bool Keyboard::ButtonPressed(Keyboard::KeyCode keycode) const {
		return keysState.test(static_cast<size_t>(keycode));
	}

	bool Keyboard::IsModifierKey(Keyboard::KeyCode keycode) const {
		return IsLeftModifierKey(keycode) || IsRightModifierKey(keycode);
	}

	bool Keyboard::IsLeftModifierKey(Keyboard::KeyCode keycode) const {
		return (keycode == Keyboard::KeyCode::EM_KEY_LEFT_CTRL) ||
			(keycode == Keyboard::KeyCode::EM_KEY_LEFT_SHIFT) ||
			(keycode == Keyboard::KeyCode::EM_KEY_LEFT_ALT);
	}
	bool Keyboard::IsRightModifierKey(Keyboard::KeyCode keycode) const {
		return (keycode == Keyboard::KeyCode::EM_KEY_RIGHT_CTRL) ||
			(keycode == Keyboard::KeyCode::EM_KEY_RIGHT_SHIFT) ||
			(keycode == Keyboard::KeyCode::EM_KEY_RIGHT_ALT);
	}

	bool Keyboard::CtrlModifierPressed() const {
		return ButtonPressed(Keyboard::KeyCode::EM_KEY_LEFT_CTRL) ||
			ButtonPressed(Keyboard::KeyCode::EM_KEY_RIGHT_CTRL);
	}
	bool Keyboard::ShiftModifierPressed() const {
		return ButtonPressed(Keyboard::KeyCode::EM_KEY_LEFT_SHIFT) ||
			ButtonPressed(Keyboard::KeyCode::EM_KEY_RIGHT_SHIFT);
	}
	bool Keyboard::AltModifierPressed() const {
		return ButtonPressed(Keyboard::KeyCode::EM_KEY_LEFT_ALT) ||
			ButtonPressed(Keyboard::KeyCode::EM_KEY_RIGHT_ALT);
	}

	void Keyboard::ClearButtonsState() {
		keysState.reset();
	}

	std::string Keyboard::GetKeyName(Keyboard::KeyCode keycode) const {
		auto search = keyNames.find(keycode);
		if (search == keyNames.end()) {
			return "EM_KEY_UNIDENTIFIED";
		}
		return search->second;
	}
	void Keyboard::InitializeKeyNameMap() {
		// Key codes 0-255

		keyNames.insert({KeyCode::EM_KEY_SPACE, "EM_KEY_SPACE"});

		keyNames.insert({KeyCode::EM_KEY_0, "EM_KEY_0"});
		keyNames.insert({KeyCode::EM_KEY_1, "EM_KEY_1"});
		keyNames.insert({KeyCode::EM_KEY_2, "EM_KEY_2"});
		keyNames.insert({KeyCode::EM_KEY_3, "EM_KEY_3"});
		keyNames.insert({KeyCode::EM_KEY_4, "EM_KEY_4"});
		keyNames.insert({KeyCode::EM_KEY_5, "EM_KEY_5"});
		keyNames.insert({KeyCode::EM_KEY_6, "EM_KEY_6"});
		keyNames.insert({KeyCode::EM_KEY_7, "EM_KEY_7"});
		keyNames.insert({KeyCode::EM_KEY_8, "EM_KEY_8"});
		keyNames.insert({KeyCode::EM_KEY_9, "EM_KEY_9"});

		keyNames.insert({KeyCode::EM_KEY_A, "EM_KEY_A"});
		keyNames.insert({KeyCode::EM_KEY_B, "EM_KEY_B"});
		keyNames.insert({KeyCode::EM_KEY_C, "EM_KEY_C"});
		keyNames.insert({KeyCode::EM_KEY_D, "EM_KEY_D"});
		keyNames.insert({KeyCode::EM_KEY_E, "EM_KEY_E"});
		keyNames.insert({KeyCode::EM_KEY_F, "EM_KEY_F"});
		keyNames.insert({KeyCode::EM_KEY_G, "EM_KEY_G"});
		keyNames.insert({KeyCode::EM_KEY_H, "EM_KEY_H"});
		keyNames.insert({KeyCode::EM_KEY_I, "EM_KEY_I"});
		keyNames.insert({KeyCode::EM_KEY_J, "EM_KEY_J"});
		keyNames.insert({KeyCode::EM_KEY_K, "EM_KEY_K"});
		keyNames.insert({KeyCode::EM_KEY_L, "EM_KEY_L"});
		keyNames.insert({KeyCode::EM_KEY_M, "EM_KEY_M"});
		keyNames.insert({KeyCode::EM_KEY_N, "EM_KEY_N"});
		keyNames.insert({KeyCode::EM_KEY_O, "EM_KEY_O"});
		keyNames.insert({KeyCode::EM_KEY_P, "EM_KEY_P"});
		keyNames.insert({KeyCode::EM_KEY_Q, "EM_KEY_Q"});
		keyNames.insert({KeyCode::EM_KEY_R, "EM_KEY_R"});
		keyNames.insert({KeyCode::EM_KEY_S, "EM_KEY_S"});
		keyNames.insert({KeyCode::EM_KEY_T, "EM_KEY_T"});
		keyNames.insert({KeyCode::EM_KEY_U, "EM_KEY_U"});
		keyNames.insert({KeyCode::EM_KEY_V, "EM_KEY_V"});
		keyNames.insert({KeyCode::EM_KEY_W, "EM_KEY_W"});
		keyNames.insert({KeyCode::EM_KEY_X, "EM_KEY_X"});
		keyNames.insert({KeyCode::EM_KEY_Y, "EM_KEY_Y"});
		keyNames.insert({KeyCode::EM_KEY_Z, "EM_KEY_Z"});

		// Key codes beyond 255

		keyNames.insert({KeyCode::EM_KEY_F1,  "EM_KEY_F1"});
		keyNames.insert({KeyCode::EM_KEY_F2,  "EM_KEY_F2"});
		keyNames.insert({KeyCode::EM_KEY_F3,  "EM_KEY_F3"});
		keyNames.insert({KeyCode::EM_KEY_F4,  "EM_KEY_F4"});
		keyNames.insert({KeyCode::EM_KEY_F5,  "EM_KEY_F5"});
		keyNames.insert({KeyCode::EM_KEY_F6,  "EM_KEY_F6"});
		keyNames.insert({KeyCode::EM_KEY_F7,  "EM_KEY_F7"});
		keyNames.insert({KeyCode::EM_KEY_F8,  "EM_KEY_F8"});
		keyNames.insert({KeyCode::EM_KEY_F9,  "EM_KEY_F9"});
		keyNames.insert({KeyCode::EM_KEY_F10, "EM_KEY_F10"});
		keyNames.insert({KeyCode::EM_KEY_F11, "EM_KEY_F11"});
		keyNames.insert({KeyCode::EM_KEY_F12, "EM_KEY_F12"});

		keyNames.insert({KeyCode::EM_KEY_ESC, "EM_KEY_ESC"});
		keyNames.insert({KeyCode::EM_KEY_DEL, "EM_KEY_DEL"});

		keyNames.insert({KeyCode::EM_KEY_LEFT_CTRL,  "EM_KEY_LEFT_CTRL"});
		keyNames.insert({KeyCode::EM_KEY_LEFT_SHIFT, "EM_KEY_LEFT_SHIFT"});
		keyNames.insert({KeyCode::EM_KEY_LEFT_ALT,   "EM_KEY_LEFT_ALT"});

		keyNames.insert({KeyCode::EM_KEY_RIGHT_CTRL,  "EM_KEY_RIGHT_CTRL"});
		keyNames.insert({KeyCode::EM_KEY_RIGHT_SHIFT, "EM_KEY_RIGHT_SHIFT"});
		keyNames.insert({KeyCode::EM_KEY_RIGHT_ALT,   "EM_KEY_RIGHT_ALT"});
	}

	KeyboardKeyCombo::KeyboardKeyCombo(Keyboard::KeyCode keycode,
		bool ctrlActive, bool shiftActive, bool altActive)
		: keycode(keycode),
		ctrlActive(ctrlActive), shiftActive(shiftActive), altActive(altActive) {
		SetComboId();
	}

	InputDeviceType KeyboardKeyCombo::GetKeyComboInputDeviceType() {
		return InputDeviceType::KEYBOARD;
	}

	uint16_t KeyboardKeyCombo::GetComboId() const {
		return comboId;
	}

	Keyboard::KeyCode KeyboardKeyCombo::GetKeyboardKeyCode() const {
		return keycode;
	}

	bool KeyboardKeyCombo::CtrlModifierActive() const {
		return ctrlActive;
	}
	bool KeyboardKeyCombo::ShiftModifierActive() const {
		return shiftActive;
	}
	bool KeyboardKeyCombo::AltModifierActive() const {
		return altActive;
	}

	void KeyboardKeyCombo::SetComboId() {
		uint32_t lsWord =
			(ctrlActive  ? 1 << 0 : 0) |
			(shiftActive ? 1 << 1 : 0) |
			(altActive   ? 1 << 2 : 0);
		uint32_t msWord =
			static_cast<uint32_t>(keycode) << 16;
		comboId = lsWord | msWord;
	}

}