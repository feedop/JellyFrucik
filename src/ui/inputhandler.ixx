export module inputhandler;

import std;

import <glm/gtc/constants.hpp>;
import <GLFW/glfw3.h>;
import <imgui/imgui/imgui.h>;

import camera;
import renderer;
import math;

export class InputHandler
{
public:
	InputHandler(GLFWwindow* window, Camera& camera, Renderer& renderer) :
		camera(camera), renderer(renderer)
	{
		glfwSetWindowUserPointer(window, this);

		glfwSetFramebufferSizeCallback(window, viewportCallback);
		glfwSetMouseButtonCallback(window, mouseCallback);
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyboardCallback);
	}

	static void viewportCallback(GLFWwindow* window, int width, int height)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.setAspect(static_cast<float>(width) / static_cast<float>(height));
		handler->renderer.setWindowSize(width, height);
	}

	static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return;

		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->handleMouseInput(window, button, action);
	}

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		handler->camera.zoom(static_cast<float>(yoffset));
	}
	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_CONTROL:
				handler->pressedKeys.CTRL = true;
				break;
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = true;
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_LEFT_CONTROL:
				handler->pressedKeys.CTRL = false;
				break;
			case GLFW_KEY_LEFT_SHIFT:
				handler->pressedKeys.SHIFT = false;
				break;
			}
		}
	}

	void handleMouseInput(GLFWwindow* window, int button, int action)
	{
		if (action == GLFW_PRESS)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_RIGHT:
				mouseMovingMode = true;
				glfwGetCursorPos(window, &lastX, &lastY);
				initialX = lastX;
				initialY = lastY;
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (button)
			{
			case GLFW_MOUSE_BUTTON_RIGHT:
				mouseMovingMode = false;
				break;
			}
		}
	}

	void handleMouseMovement(GLFWwindow* window)
	{
		if (!mouseMovingMode)
			return;

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		double dx = x - lastX;
		double dy = y - lastY;

		if ((dx < math::eps && dx > -math::eps) && (dy < math::eps && dy > -math::eps))
			return;

		int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

		float xDiff = dx / windowWidth * math::pi;
		float yDiff = dy / windowHeight * math::pi;

		// Camera movement and rotation

		if (pressedKeys.CTRL)
		{
			if (pressedKeys.SHIFT)
			{
				renderer.moveHandle(xDiff, yDiff);
			}
			else
			{
				renderer.rotateHandle(xDiff, yDiff);
			}
		}
		else if (pressedKeys.SHIFT)
		{
			camera.move(xDiff, yDiff);
		}
		else
		{
			camera.rotate(xDiff, yDiff);
		}

		lastX = x;
		lastY = y;
	}

private:
	Camera& camera;
	Renderer& renderer;

	double lastX = 0;
	double lastY = 0;
	double initialX = 0;
	double initialY = 0;

	bool mouseMovingMode = false;

	struct PressedKeys
	{
		bool CTRL = false;
		bool SHIFT = false;	
	} pressedKeys;
};

