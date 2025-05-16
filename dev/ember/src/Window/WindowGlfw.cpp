#include "Window/WindowGlfw.h"
#include "Core/Error.h"

#include <sstream>

namespace ember {

    void WindowGlfw::GlfwErrorCallback(int errorCode, const char* description) {
        std::stringstream errorStream;

        errorStream << "[GLFW error code]: " << errorCode << "\n"
            << "[Error description]: " << description;
        GLFW_ERROR(errorStream.str().c_str());
    }

    void WindowGlfw::InitializeGlfwLibrary() {
        if (glfwInit() == GLFW_FALSE) {
            GLFW_ERROR("Failed to initialize GLFW.\n");
        }
#if defined(DEBUG) || defined(_DEBUG)
        glfwSetErrorCallback(GlfwErrorCallback);
#endif
    }
    void WindowGlfw::TerminateGlfwLibrary() {
        glfwTerminate();
    }

    void WindowGlfw::LoadOpenGlFunctions() {
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            throw std::runtime_error{ "Failed to initialize OpenGL context!" };
        }
    }

    WindowGlfw::WindowGlfw(const WindowSettings& windowSettings)
        : Window(windowSettings) {
    }
    void WindowGlfw::Update() {
        /* Render here */
        glClearColor(215.0f/255.0f, 153/255.0f , 33.0f/255.0f , 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(windowHandle);

        /* Poll for and process events */
        glfwPollEvents();
    }

    void WindowGlfw::InitializeOpenGLSpecific(const SettingsGlfw& glfwOglSettings) {
#if defined(DEBUG) || defined(_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwOglSettings.openglVersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwOglSettings.openglVersionMinor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    void WindowGlfw::MakeContextCurrent() {
        glfwMakeContextCurrent(windowHandle);
    }
    void WindowGlfw::SetSwapInterval(int swapInterval) {
        glfwSwapInterval(swapInterval);
    }

    void WindowGlfw::InitializeVulkanSpecific(/* const VulkanSettings& vulkanSettings */) {
        assert(false && "Vulkan is not supported yet!");
    }

    void WindowGlfw::CreateWindow(WindowGlfw* sharedCtxWindow) {
        InitializeWindowGlfwParams();
        CreateGlfwWindow(sharedCtxWindow);
        RegisterGlfwCallbacks();
    }
    void WindowGlfw::DestroyWindow() {
        glfwDestroyWindow(windowHandle);
    }

    GLFWwindow* WindowGlfw::GetApiSpecificHandle() const {
        return windowHandle;
    }

    void WindowGlfw::InitializeWindowGlfwParams() {
        int visible = windowSettings.isVisible ? GLFW_TRUE : GLFW_FALSE;
        glfwWindowHint(GLFW_VISIBLE, visible);
    }
    void WindowGlfw::CreateGlfwWindow(WindowGlfw* sharedCtxWindow) {
        // TODO: fullscreen state handling.
        /*
        GLFWwindow* sharedWindow{ nullptr };
        if (coreGlobal.mainWindow)
        {
            sharedWindow = coreGlobal.mainWindow->GetApiSpecificHandle();
        }
        bool sharedContextCondition = (sharedWindow == nullptr && windowSettings.isContextShared);
        assert(!sharedContextCondition &&
            "Cannot share context with the main editor's window! 'coreGlobal.window' is nullptr!");
        */

        GLFWwindow* sharedWindowHandle{ nullptr };
        if (sharedCtxWindow)
            sharedWindowHandle = sharedCtxWindow->GetApiSpecificHandle();

        windowHandle = glfwCreateWindow(
            windowSettings.windowDimensions.Width(), windowSettings.windowDimensions.Height(),
            windowSettings.windowTitle.data(), NULL, sharedWindowHandle);

        if (windowHandle == nullptr) {
            glfwTerminate();
            GLFW_ERROR("Failed to create the application window.\n");
        }
    }
    void WindowGlfw::RegisterGlfwCallbacks() {
        // TODO: add callbacks.
    }

}