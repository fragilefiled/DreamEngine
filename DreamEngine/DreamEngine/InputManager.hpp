#pragma once
#include <GLFW/glfw3.h>
namespace Input {
	class InputManager
	{

	public:
		static GLFWwindow* window;
		static enum KeyWordOperation
		{
			Press
		};
		static enum Key
		{
			W,
			A,
			S,
			D
		};
		InputManager();
		~InputManager();

		static bool GetKeyOperation(GLFWwindow* inputWindow, KeyWordOperation op, Key key)
		{
			int GLFW_KEYOP = GLFW_KEY_W;
			int GLFW_KEY = GLFW_PRESS;
			switch (key)
			{
			case W:
				GLFW_KEY = GLFW_KEY_W;
				break;
			case A:
				GLFW_KEY = GLFW_KEY_A;
				break;
			case S:
				GLFW_KEY = GLFW_KEY_S;
				break;
			case D:
				GLFW_KEY = GLFW_KEY_D;
				break;
			default:
				break;
			};

			switch (op)
			{
			case Press:
				GLFW_KEYOP = GLFW_PRESS;
				break;
			}
			if (glfwGetKey(inputWindow , GLFW_KEYOP) == GLFW_KEYOP)
				return true;
			else
				return false;

		}

		static void initWindow(GLFWwindow* inputWindow) {
			
			window = inputWindow;
		}


	private:

	};
}
