#include <GLFW\glfw3.h>
#pragma once
float mouseXpos;
float mouseYpos;
float yScollRoll;
bool mouseDown;
int leftMouseState;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	mouseXpos = xpos;
	mouseYpos = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	yScollRoll = yoffset;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouseDown = true;
	else
		mouseDown = false;
}

void getMouseState(GLFWwindow* window) 
{
	leftMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
}