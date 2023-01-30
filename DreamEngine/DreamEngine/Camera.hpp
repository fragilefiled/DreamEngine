#pragma once
#include"glm/glm.hpp"
#include"glm/ext/matrix_transform.hpp"
#include"glm/ext/matrix_clip_space.hpp"
#include "InputManager.hpp"
#include"GlobalFunction.h"
namespace DreamCamera {
	class Camera {
	public:
		glm::vec3 cameraPos;
		glm::vec3 cameraFront;
		glm::vec3 cameraUp;
		float cameraSpeed;
		float fovy;
		float aspect;
		float nearPlane;
		float farPlane;
		glm::mat4 view;
		glm::mat4 proj;
		Camera() {};
		Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp, float fovy , float aspect, float nearPlane, float farPlane,
			float cameraSpeed = 1.0f
			) {
			this->cameraPos = cameraPos;
			this->cameraFront = cameraFront;
			this->cameraUp = cameraUp;
			this->fovy = fovy;
			this->aspect = aspect;
			this->nearPlane = nearPlane;
			this->farPlane = farPlane;
			this->cameraSpeed = cameraSpeed;
		}

		glm::mat4 getViewMat() {
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			return view;
		}

		glm::mat4 getProjMat() {
			proj = glm::perspective(glm::radians(fovy), aspect, nearPlane, farPlane);
			proj[1][1] *= -1; //glm for opengl ,vk's y is inversed
			return proj;
		}

		glm::mat4 getViewProjMat() {
			return view * proj;
		}

		void processKeyBoard(GLFWwindow* inputWindow) 
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			float realCameraSpeed = cameraSpeed * deltaTime;
			if (glfwGetKey(inputWindow, GLFW_KEY_W) == GLFW_PRESS)
				cameraPos += realCameraSpeed * cameraFront;
			if (glfwGetKey(inputWindow, GLFW_KEY_S) == GLFW_PRESS)
				cameraPos -= realCameraSpeed * cameraFront;
			if (glfwGetKey(inputWindow, GLFW_KEY_A) == GLFW_PRESS)
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * realCameraSpeed;
			if (glfwGetKey(inputWindow, GLFW_KEY_D) == GLFW_PRESS)
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * realCameraSpeed;

		}
		void processInput(GLFWwindow* inputWindow) {
			processKeyBoard(inputWindow);
			processMouse(inputWindow);
		}


		void processMouse(GLFWwindow* inputWindow) {
			glfwSetCursorPosCallback(inputWindow, mouse_callback);
			processMouseRotate(mouseXpos, mouseYpos);
			glfwSetScrollCallback(inputWindow, scroll_callback);
			processMouseScoll(yScollRoll);
		}
	private:

		void processMouseRotate(double xposIn, double yposIn)
		{
			float xpos = static_cast<float>(xposIn);
			float ypos = static_cast<float>(yposIn);
			if (leftMouseState == GLFW_PRESS) 
			{
				if (init)
				{
					lastX = xpos;
					lastY = ypos;
					init = false;
				}

				float xoffset = xpos - lastX;
				float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
				lastX = xpos;
				lastY = ypos;

				float sensitivity = 0.2f; // change this value to your liking
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				yaw += xoffset;
				pitch += yoffset;

				// make sure that when pitch is out of bounds, screen doesn't get flipped
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				glm::vec3 front;
				if (mouseDown)
				{
					front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
					front.y = sin(glm::radians(pitch));
					front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
					cameraFront = glm::normalize(front);
				}
			}

			if (leftMouseState == GLFW_RELEASE)
				init = true;

		}
		void processMouseScoll(double yScollRoll)
		{
			if (fovy >= 1.0f && fovy <= 45.0f && yScollRoll != 0.0f)
				fovy -= yScollRoll;
			if (fovy <= 1.0f)
				fovy = 1.0f;
			if (fovy >= 45.0f)
				fovy = 45.0f;
		}
		bool init = false;
		float deltaTime = 0.0f; // 当前帧与上一帧的时间差
		float lastFrame = 0.0f; // 上一帧的时间
		float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
		float pitch = 0.0f;
		float lastX = 800.0f / 2.0;
		float lastY = 600.0 / 2.0;
	};
}