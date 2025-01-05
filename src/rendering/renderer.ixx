export module renderer;

import std;

import <glad/glad.h>;
import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import <glm/gtx/quaternion.hpp>;

import box;
import camera;
import colors;
import config;
import cube;
import drawable;
import shader;
import simulation;
import quad;
import grid;
import object;
import lightedobject;
import math;
import handle;
import externalsprings;

/// <summary>
/// Responsible for rendering the scene using OpenGL
/// </summary>
export class Renderer
{
public:
	Renderer(int windowWidth, int windowHeight, Camera& camera, SimulationEngine& simulation) :
		windowWidth(windowWidth), windowHeight(windowHeight), camera(camera), simulation(simulation)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_PROGRAM_POINT_SIZE);

		// Create objects
		cube = std::make_unique<Cube>();
		externalSprings = std::make_unique<ExternalSprings>();
		handle = std::make_unique<Handle>();
		box = std::make_unique<Box>(simulation.getBoxSize());
		grid = std::make_unique<Grid>();
	}

	/// <summary>
	/// Render the scene
	/// </summary>
	void draw(const SimulationData& data)
	{
		// Clear
		glViewport(0, 0, windowWidth, windowHeight);
		glClearColor(0.15f, 0.0f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawScene(data);

		// Grid	
		if (drawGrid)
		{
			gridShader->use();
			gridShader->setMatrix("view", camera.getView());
			gridShader->setMatrix("projection", camera.getProjection());
			grid->draw(gridShader.get());
		}
	}

	void setWindowSize(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		glViewport(0, 0, windowWidth, windowHeight);
	}

	void rotateHandle(float xDiff, float yDiff)
	{
		float angleRadians = std::sqrtf(xDiff * xDiff + yDiff * yDiff);
		auto axis = glm::normalize(camera.getUp() * xDiff + camera.getRight() * yDiff);
		handle->rotate(angleRadians, axis);
		simulation.setConfigHandleModel(handle->getModel());
	}

	void moveHandle(float xDiff, float yDiff)
	{
		handle->translate(getTranslationFromMouse(xDiff, yDiff));
		simulation.setConfigHandleModel(handle->getModel());
	}

private:
	friend class GuiController;

	Camera& camera;
	SimulationEngine& simulation;

	std::unique_ptr<Shader> uniformColorShader = std::make_unique<UniformColorShader>();
	std::unique_ptr<Shader> phongShader = std::make_unique<PhongShader>();
	std::unique_ptr<Shader> jellyShader = std::make_unique<JellyShader>();
	std::unique_ptr<Shader> gridShader = std::make_unique<InfiniteGridShader>();

	std::unique_ptr<Cube> cube;
	std::unique_ptr<LightedObject> externalSprings;
	std::unique_ptr<Object> handle;
	std::unique_ptr<Drawable> box;
	std::unique_ptr<Drawable> grid;

	int windowWidth;
	int windowHeight;
	bool drawGrid = true;
	bool drawCube = true;
	bool drawWireframe = false;
	bool drawBox = true;

	void drawScene(const SimulationData& data)
	{
		jellyShader->use();
		jellyShader->setMatrix("view", camera.getView());
		jellyShader->setMatrix("projection", camera.getProjection());

		// Use the data from the simulation
		auto renderData = Cube::createVertices(data.positions);
		cube->setData(renderData);
		setExternalSpringData(data.positions);

		if (drawCube)
		{
			cube->draw(jellyShader.get());
		}
		
		uniformColorShader->use();
		uniformColorShader->setMatrix("view", camera.getView());
		uniformColorShader->setMatrix("projection", camera.getProjection());

		// Handle
		handle->draw(uniformColorShader.get());

		// Wireframe
		if (drawWireframe)
		{
			cube->drawWireframe(uniformColorShader.get());
			externalSprings->draw(uniformColorShader.get());
		}

		// Box
		if (drawBox)
		{
			phongShader->use();
			phongShader->setMatrix("view", camera.getView());
			phongShader->setMatrix("projection", camera.getProjection());
			box->draw(phongShader.get());
		}		
	}

	inline glm::vec3 getTranslationFromMouse(float xDiff, float yDiff) const
	{
		return 0.25f / camera.getZoomScale().y * (xDiff * camera.getRight() - yDiff * camera.getUp());
	}

	void setExternalSpringData(const std::array<glm::vec3, 64>& cubePositions)
	{
		static std::vector<glm::vec3> handleVertices = Handle::createVertices();

		std::vector<glm::vec3> externalSpringData;
		for (int i = 0; i < 8; i++)
		{
			externalSpringData.push_back(cubePositions[SimulationEngine::indicesConnectedToHandle[i]]);
			externalSpringData.push_back(handle->getModel() * glm::vec4(handleVertices[i], 1.0f));	
		}
		externalSprings->setData(externalSpringData);
	}
};