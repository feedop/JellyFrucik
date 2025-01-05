import std;

import <glad/glad.h>;
import <GLFW/glfw3.h>;
import <glm/vec3.hpp>;

import application;
import camera;
import config;
import gui;
import simulation;
import renderer;
import inputhandler;
import math;

#ifdef _WIN32
// Select an NVIDIA GPU if possible
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

int main()
{
    Application application(cfg::initialWidth, cfg::initialHeight);
    GLFWwindow* window = application.getWindowPtr();

    // Create initial values for config
    SimulationConfig config
    {
        0.007f,
        1.0f,
        40.0f,
        40.0f,
        1.0f,
        0.0,
        1.0f,
        true,
        glm::mat4{1.0f}
    };

    // Create the simulation engine
    SimulationEngine simulationEngine(config);

    // Create a graphics controller
    Camera camera(15.0f);
    Renderer renderer(cfg::initialWidth, cfg::initialHeight, camera, simulationEngine);

    // Setup input handling
    InputHandler inputHandler(window, camera, renderer);

    // Create a GUI controller
    GuiController guiController(window, camera, simulationEngine, renderer, config);

    simulationEngine.start();

    // MAIN LOOP HERE - dictated by glfw
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {    
        // Receive a snapshot of the data from the simulation
        SimulationData data = simulationEngine.getData();
        // OpenGL render
        renderer.draw(data);

        // GUI rendering
        guiController.draw(data);

        glfwSwapBuffers(window);

        // Show FPS in the title bar
        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        if (delta >= 1.0)
        {
            double fps = double(frameCount) / delta;
            std::stringstream ss;
            ss << "CAD Bane" << " " << " [" << fps << " FPS]";

            glfwSetWindowTitle(window, ss.str().c_str());
            lastTime = currentTime;
            frameCount = 0;
        }
        else
        {
            frameCount++;
        }

        // Handle user input
        glfwPollEvents();
        inputHandler.handleMouseMovement(window);
    }
    simulationEngine.stop();

    return 0;
}