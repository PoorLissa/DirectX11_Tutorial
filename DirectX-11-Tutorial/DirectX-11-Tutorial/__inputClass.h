#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_


class InputClass {
 public:
	InputClass()				   {}
	InputClass(const InputClass &) {}
   ~InputClass()				   {}

   // Initialize all the keys to being released and not pressed.
	void Initialize() {
		for (int i = 0; i < 256; i++)
			m_keys[i] = false;
	}

	// If a key is pressed then save that state in the key array.
	void KeyDown(unsigned int Input) {
		m_keys[Input] = true;
	}

	// If a key is released then clear that state in the key array.
	void KeyUp(unsigned int Input) {
		m_keys[Input] = false;
	}

	// Return what state the key is in (pressed/not pressed).
	bool IsKeyDown(unsigned int Key) {
		return m_keys[Key];
	}

private:
	bool m_keys[256];
};

#endif
