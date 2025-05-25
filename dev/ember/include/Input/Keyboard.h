#pragma once 

#include "Input/Input.h"

#include <bitset>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ember {

	class Keyboard : public InputDevice {
	public:
		enum class KeyCode {
			EM_KEY_UNIDENTIFIED = -1,

			// Key codes 0-255

			EM_KEY_SPACE = 0x20, // SPACE key

			EM_KEY_0 = 0x30, // 0 key
			EM_KEY_1 = 0x31, // 1 key
			EM_KEY_2 = 0x32, // 2 key
			EM_KEY_3 = 0x33, // 3 key
			EM_KEY_4 = 0x34, // 4 key
			EM_KEY_5 = 0x35, // 5 key
			EM_KEY_6 = 0x36, // 6 key
			EM_KEY_7 = 0x37, // 7 key
			EM_KEY_8 = 0x38, // 8 key
			EM_KEY_9 = 0x39, // 9 key

			EM_KEY_A = 0x41, // A key
			EM_KEY_B = 0x42, // B key
			EM_KEY_C = 0x43, // C key
			EM_KEY_D = 0x44, // D key
			EM_KEY_E = 0x45, // E key
			EM_KEY_F = 0x46, // F key
			EM_KEY_G = 0x47, // G key
			EM_KEY_H = 0x48, // H key
			EM_KEY_I = 0x49, // I key
			EM_KEY_J = 0x4A, // J key
			EM_KEY_K = 0x4B, // K key
			EM_KEY_L = 0x4C, // L key
			EM_KEY_M = 0x4D, // M key
			EM_KEY_N = 0x4E, // N key
			EM_KEY_O = 0x4F, // O key
			EM_KEY_P = 0x50, // P key
			EM_KEY_Q = 0x51, // Q key
			EM_KEY_R = 0x52, // R key
			EM_KEY_S = 0x53, // S key
			EM_KEY_T = 0x54, // T key
			EM_KEY_U = 0x55, // U key
			EM_KEY_V = 0x56, // V key
			EM_KEY_W = 0x57, // W key
			EM_KEY_X = 0x58, // X key
			EM_KEY_Y = 0x59, // Y key
			EM_KEY_Z = 0x5A, // Z key

			// Key codes beyond 255

			EM_KEY_F1  = 0x100, // F1 key
			EM_KEY_F2  = 0x101, // F2 key
			EM_KEY_F3  = 0x102, // F3 key
			EM_KEY_F4  = 0x103, // F4 key
			EM_KEY_F5  = 0x104, // F5 key
			EM_KEY_F6  = 0x105, // F6 key
			EM_KEY_F7  = 0x106, // F7 key
			EM_KEY_F8  = 0x107, // F8 key
			EM_KEY_F9  = 0x108, // F9 key
			EM_KEY_F10 = 0x109, // F10 key
			EM_KEY_F11 = 0x10A, // F11 key
			EM_KEY_F12 = 0x10B, // F12 key

			EM_KEY_ESC = 0x10C, // ESC key
			EM_KEY_DEL = 0x10D, // DEL key

			EM_KEY_LEFT_CTRL  = 0x10E, // LCTRL  key
			EM_KEY_LEFT_SHIFT = 0x10F, // LSHIFT key
			EM_KEY_LEFT_ALT   = 0x110, // LALT   key

			EM_KEY_RIGHT_CTRL  = 0x111, // RCTRL  key
			EM_KEY_RIGHT_SHIFT = 0x112, // RSHIFT key
			EM_KEY_RIGHT_ALT   = 0x113, // RALT   key

			COUNT = 57,
			MAX_CODE = EM_KEY_RIGHT_ALT,
		};

		Keyboard();

		InputDeviceType GetInputDeviceType() override;

		void SetButtonState(Keyboard::KeyCode keycode, KeyActionType action);
		bool ButtonPressed(Keyboard::KeyCode keycode) const;

		bool IsModifierKey(Keyboard::KeyCode keycode) const;
		bool IsLeftModifierKey(Keyboard::KeyCode keycode) const;
		bool IsRightModifierKey(Keyboard::KeyCode keycode) const;

		bool CtrlModifierPressed() const;
		bool ShiftModifierPressed() const;
		bool AltModifierPressed() const;

		void ClearButtonsState();

		std::string GetKeyName(Keyboard::KeyCode keycode) const;

	private:

		void InitializeKeyNameMap();

		std::bitset<static_cast<size_t>(Keyboard::KeyCode::MAX_CODE) + 1> keysState;
		std::unordered_map<Keyboard::KeyCode, std::string> keyNames;
	};

	// KeyboardCombo

	class KeyboardKeyCombo : public KeyCombo
	{
	public:

		KeyboardKeyCombo(Keyboard::KeyCode keycode,
			bool ctrlActive, bool shiftActive, bool altActive);

		InputDeviceType GetKeyComboInputDeviceType() override;

		uint16_t GetComboId() const;

		Keyboard::KeyCode GetKeyboardKeyCode() const;

		bool CtrlModifierActive() const;
		bool ShiftModifierActive() const;
		bool AltModifierActive() const;

	private:

		void SetComboId();

		Keyboard::KeyCode keycode{ Keyboard::KeyCode::EM_KEY_UNIDENTIFIED };
		uint32_t comboId{ 0 };

		bool ctrlActive{ false };
		bool shiftActive{ false };
		bool altActive{ false };
	};

}