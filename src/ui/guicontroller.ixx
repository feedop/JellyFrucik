export module gui:controller;

import std;

import <imgui.h>;
import <imgui/backend/imgui_impl_glfw.h>;
import <imgui/backend/imgui_impl_opengl3.h>;
import <implot/implot.h>;
import <GLFW/glfw3.h>;

import <glm/vec2.hpp>;
import <glm/vec3.hpp>;

import camera;
import renderer;
import simulation;
import theme;

ImGuiIO& createImguiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    return ImGui::GetIO();
}

/// <summary>
/// Responsible for managing the GUI
/// </summary>
export class GuiController
{
public:
    GuiController(GLFWwindow* window, Camera& camera, SimulationEngine& simulation, Renderer& renderer, const SimulationConfig& config) :
        io(createImguiContext()), camera(camera), simulation(simulation), renderer(renderer), config(config)
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowMinSize = ImVec2(500, 500);

        cinder::setTheme();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 420");
    }

    void draw(const SimulationData& data)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui debug
        bool showDemo = false;
        if (showDemo)
        {
            //ImGui::ShowDemoWindow(&showDemo);
            ImPlot::ShowDemoWindow(&showDemo);
            render();
            return;
        }

        // Render the config window
        ImGui::Begin("Configuration window", nullptr, ImGuiWindowFlags_None);
        renderConfig(data);
        ImGui::End();

        render();
    }
private:
    ImGuiIO& io;
    Camera& camera;
    SimulationEngine& simulation;
    Renderer& renderer;

    SimulationConfig config;

    static constexpr int size = 144 * 20;

    void render()
    {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    // Components
    void renderConfig(const SimulationData& data);
};