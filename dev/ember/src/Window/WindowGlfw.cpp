#include "Window/WindowGlfw.h"
#include "Core/Error.h"

#include <cassert>
#include <sstream>

namespace ember {

    static bool glfwLibraryInitialized{false};

    void WindowGlfw::GlfwErrorCallback(int errorCode, const char* description) {
        std::stringstream errorStream;
        errorStream << "[GLFW error code]: " << errorCode << "\n"
            << "[Error description]: " << description;
        GLFW_ERROR(errorStream.str().c_str());
    }

    void WindowGlfw::InitializeGlfwLibrary() {
        assert(!glfwLibraryInitialized && "The GLFW Library has already been initialized!");
        if (glfwInit() == GLFW_FALSE) {
            GLFW_ERROR("Failed to initialize GLFW.\n");
        }
#if defined(DEBUG) || defined(_DEBUG)
        glfwSetErrorCallback(GlfwErrorCallback);
#endif
        glfwLibraryInitialized = true;
    }
    void WindowGlfw::TerminateGlfwLibrary() {
        assert(glfwLibraryInitialized && "The GLFW Library must be initialized!");
        glfwTerminate();
    }
    bool WindowGlfw::GlfwLibraryInitialized() {
        return glfwLibraryInitialized;
    }
    void WindowGlfw::LoadOpenGlFunctions() {
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            throw std::runtime_error{ "Failed to load OpenGL functions!" };
        }
    }

    WindowGlfw::WindowGlfw(const WindowSettings& windowSettings)
        : Window(windowSettings) {
        SetClientApi(GLFW_NO_API);
    }

    void WindowGlfw::CreateWindow(void* dataPtr) {
        CreateGlfwWindow(nullptr);
    }
    void WindowGlfw::DestroyWindow() {
        glfwDestroyWindow(windowHandle);
        SetClientApi(GLFW_NO_API);
        windowHandle = nullptr;
    }

    void WindowGlfw::Update() {
        glfwPollEvents();
    }

    void WindowGlfw::CreateGlfwWindow(WindowGlfw* sharedCtxWindow) {
        InitializeWindowGlfwParams();
        GLFWwindow* sharedWindowHandle{ nullptr };
        if (sharedCtxWindow)
            sharedWindowHandle = sharedCtxWindow->GetApiSpecificHandle();
        windowHandle = glfwCreateWindow(
            windowSettings.windowDimensions.Width(), windowSettings.windowDimensions.Height(),
            windowSettings.windowTitle.data(), NULL, sharedWindowHandle);
        if (windowHandle == nullptr) {
            glfwTerminate();
            GLFW_ERROR("Failed to create an application window!\n");
        }
        RegisterGlfwCallbacks();
    }

    void WindowGlfw::InitializeOpenGLSpecific(const SettingsOgl& oglSettings) {
        SetClientApi(GLFW_OPENGL_API);
#if defined(DEBUG) || defined(_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, oglSettings.openglVersionMajor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, oglSettings.openglVersionMinor);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
    void WindowGlfw::MakeContextCurrent() {
        glfwMakeContextCurrent(windowHandle);
    }
    void WindowGlfw::MakeContextNonCurrent() {
        glfwMakeContextCurrent(nullptr);
    }

    void WindowGlfw::PresentFrame() {
        glfwSwapBuffers(windowHandle);
    }

    void WindowGlfw::InitializeVulkanSpecific(/* const VulkanSettings& vulkanSettings */) {
        SetClientApi(GLFW_NO_API);
        assert(false && "Vulkan is not supported yet!");
    }

    GLFWwindow* WindowGlfw::GetApiSpecificHandle() const {
        return windowHandle;
    }

    void WindowGlfw::InitializeWindowGlfwParams() {
        SetVisibility(windowSettings.isVisible ? GLFW_TRUE : GLFW_FALSE);
    }
    void WindowGlfw::SetVisibility(int visibility) {
        glfwWindowHint(GLFW_VISIBLE, visibility);
    }
    void WindowGlfw::SetClientApi(int api) {
        glfwWindowHint(GLFW_CLIENT_API, api);
    }
    void WindowGlfw::RegisterGlfwCallbacks() {
        if (!windowSettings.setCallbacks)
            return;

        // TODO: add callbacks.
    }

    void WindowGlfw::ProcessEvents() {
        glfwPollEvents();
    }

}