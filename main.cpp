#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

GLFWwindow *initializeOpenGL();
void initializeUI(GLFWwindow *window);

#define DEBUG

int main() {
    GLFWwindow *window = initializeOpenGL();
    initializeUI(window);

#ifdef DEBUG
    // Output some debugging information
    std::cout << "VENDOR: " << (char *) glGetString(GL_VENDOR) << std::endl;
    std::cout << "VERSION: " << (char *) glGetString(GL_VERSION) << std::endl;
    std::cout << "RENDERER: " << (char *) glGetString(GL_RENDERER) << std::endl;
#endif

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* DearImGui */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Demo window");
        ImGui::Button("Hello!");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

GLFWwindow *initializeOpenGL() {
    GLFWwindow *window;

    if (glfwInit() != GLFW_TRUE) {
        std::cout << "Couldn't initialize glfw" << std::endl;
        std::exit(-1);
    }

#if __APPLE__
    // GL 3.2 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        std::cout << "Couldn't initialize the window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err) {
        std::cout << "Couldn't initialize the window" << std::endl;
        std::exit(-1);
    }

    return window;
}

void initializeUI(GLFWwindow *window) {
    IMGUI_CHECKVERSION();

#if __APPLE__
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 410";
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
#endif

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();
}
