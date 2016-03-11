#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

inline bool forwardKeyboard(unsigned int key) {
	return (key == GLFW_KEY_UP)
		|| (key == GLFW_KEY_W);
}

inline bool backwardKeyboard(unsigned int key) {
	return (key == GLFW_KEY_DOWN)
		|| (key == GLFW_KEY_S);
}

inline bool leftKeyboard(unsigned int key) {
	return (key == GLFW_KEY_LEFT)
		|| (key == GLFW_KEY_A);
}

inline bool rightKeyboard(unsigned int key) {
	return (key == GLFW_KEY_RIGHT)
		|| (key == GLFW_KEY_D);
}

inline bool hornKeyboard(unsigned int key) {
	return (key == GLFW_KEY_E);
}





#endif