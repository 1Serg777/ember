#include "Window/WindowGlfw.h"
#include "Core/Error.h"
#include "Input/Keyboard.h"

#include <cassert>
#include <sstream>
#include <unordered_map>

namespace ember {

    static bool glfwLibraryInitialized{false};

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
        SetWindowHints();
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
        SetMinSizeLimit(windowSettings.minSizeLimit.Width(), windowSettings.minSizeLimit.Height());
        RetrieveFramebufferDimensions();
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

    void WindowGlfw::SetWindowHints() {
        SetResizability(windowSettings.isResizable ? GLFW_TRUE : GLFW_FALSE);
        SetVisibility(windowSettings.isVisible ? GLFW_TRUE : GLFW_FALSE);
    }
    void WindowGlfw::SetResizability(int resize) {
        glfwWindowHint(GLFW_RESIZABLE, resize);
    }
    void WindowGlfw::SetMinSizeLimit(int width, int height) {
        glfwSetWindowSizeLimits(windowHandle, width, height, GLFW_DONT_CARE, GLFW_DONT_CARE);
    }
    void WindowGlfw::SetVisibility(int visibility) {
        glfwWindowHint(GLFW_VISIBLE, visibility);
    }
    void WindowGlfw::SetClientApi(int api) {
        glfwWindowHint(GLFW_CLIENT_API, api);
    }
    
    void WindowGlfw::RetrieveFramebufferDimensions() {
        int width{0};
        int height{0};
        glfwGetFramebufferSize(windowHandle, &width, &height);
        windowSettings.framebufferDimensions = Dimensions2D{
            static_cast<uint32_t>(width), static_cast<uint32_t>(height)
        };
    }
    void WindowGlfw::RegisterGlfwCallbacks() {
        if (!windowSettings.setCallbacks)
            return;
        glfwSetKeyCallback(windowHandle, KeyboardKeyEventCallback);
        glfwSetWindowCloseCallback(windowHandle, WindowCloseEventCallback);
        glfwSetFramebufferSizeCallback(windowHandle, FramebufferResizeEventCallback);
        glfwSetWindowUserPointer(windowHandle, this);
    }

    static const std::unordered_map<int, Keyboard::KeyCode> glfwKeyboardMap {
        {GLFW_KEY_SPACE, Keyboard::KeyCode::EM_KEY_SPACE},

        {GLFW_KEY_0, Keyboard::KeyCode::EM_KEY_0},
        {GLFW_KEY_1, Keyboard::KeyCode::EM_KEY_1},
        {GLFW_KEY_2, Keyboard::KeyCode::EM_KEY_2},
        {GLFW_KEY_3, Keyboard::KeyCode::EM_KEY_3},
        {GLFW_KEY_4, Keyboard::KeyCode::EM_KEY_4},
        {GLFW_KEY_5, Keyboard::KeyCode::EM_KEY_5},
        {GLFW_KEY_6, Keyboard::KeyCode::EM_KEY_6},
        {GLFW_KEY_7, Keyboard::KeyCode::EM_KEY_7},
        {GLFW_KEY_8, Keyboard::KeyCode::EM_KEY_8},
        {GLFW_KEY_9, Keyboard::KeyCode::EM_KEY_9},

        {GLFW_KEY_A, Keyboard::KeyCode::EM_KEY_A},
        {GLFW_KEY_B, Keyboard::KeyCode::EM_KEY_B},
        {GLFW_KEY_C, Keyboard::KeyCode::EM_KEY_C},
        {GLFW_KEY_D, Keyboard::KeyCode::EM_KEY_D},
        {GLFW_KEY_E, Keyboard::KeyCode::EM_KEY_E},
        {GLFW_KEY_F, Keyboard::KeyCode::EM_KEY_F},
        {GLFW_KEY_G, Keyboard::KeyCode::EM_KEY_G},
        {GLFW_KEY_H, Keyboard::KeyCode::EM_KEY_H},
        {GLFW_KEY_I, Keyboard::KeyCode::EM_KEY_I},
        {GLFW_KEY_J, Keyboard::KeyCode::EM_KEY_J},
        {GLFW_KEY_K, Keyboard::KeyCode::EM_KEY_K},
        {GLFW_KEY_L, Keyboard::KeyCode::EM_KEY_L},
        {GLFW_KEY_M, Keyboard::KeyCode::EM_KEY_M},
        {GLFW_KEY_N, Keyboard::KeyCode::EM_KEY_N},
        {GLFW_KEY_O, Keyboard::KeyCode::EM_KEY_O},
        {GLFW_KEY_P, Keyboard::KeyCode::EM_KEY_P},
        {GLFW_KEY_Q, Keyboard::KeyCode::EM_KEY_Q},
        {GLFW_KEY_R, Keyboard::KeyCode::EM_KEY_R},
        {GLFW_KEY_S, Keyboard::KeyCode::EM_KEY_S},
        {GLFW_KEY_T, Keyboard::KeyCode::EM_KEY_T},
        {GLFW_KEY_U, Keyboard::KeyCode::EM_KEY_U},
        {GLFW_KEY_V, Keyboard::KeyCode::EM_KEY_V},
        {GLFW_KEY_W, Keyboard::KeyCode::EM_KEY_W},
        {GLFW_KEY_X, Keyboard::KeyCode::EM_KEY_X},
        {GLFW_KEY_Y, Keyboard::KeyCode::EM_KEY_Y},
        {GLFW_KEY_Z, Keyboard::KeyCode::EM_KEY_Z},

        {GLFW_KEY_F1,  Keyboard::KeyCode::EM_KEY_F1},
        {GLFW_KEY_F2,  Keyboard::KeyCode::EM_KEY_F2},
        {GLFW_KEY_F3,  Keyboard::KeyCode::EM_KEY_F3},
        {GLFW_KEY_F4,  Keyboard::KeyCode::EM_KEY_F4},
        {GLFW_KEY_F5,  Keyboard::KeyCode::EM_KEY_F5},
        {GLFW_KEY_F6,  Keyboard::KeyCode::EM_KEY_F6},
        {GLFW_KEY_F7,  Keyboard::KeyCode::EM_KEY_F7},
        {GLFW_KEY_F8,  Keyboard::KeyCode::EM_KEY_F8},
        {GLFW_KEY_F9,  Keyboard::KeyCode::EM_KEY_F9},
        {GLFW_KEY_F10, Keyboard::KeyCode::EM_KEY_F10},
        {GLFW_KEY_F11, Keyboard::KeyCode::EM_KEY_F11},
        {GLFW_KEY_F12, Keyboard::KeyCode::EM_KEY_F12},

        {GLFW_KEY_ESCAPE,  Keyboard::KeyCode::EM_KEY_ESC},
        {GLFW_KEY_DELETE,  Keyboard::KeyCode::EM_KEY_DEL},

        {GLFW_KEY_LEFT_CONTROL,  Keyboard::KeyCode::EM_KEY_LEFT_CTRL },
        {GLFW_KEY_LEFT_SHIFT,    Keyboard::KeyCode::EM_KEY_LEFT_SHIFT},
        {GLFW_KEY_LEFT_ALT,      Keyboard::KeyCode::EM_KEY_LEFT_ALT  },

        {GLFW_KEY_RIGHT_CONTROL, Keyboard::KeyCode::EM_KEY_RIGHT_CTRL },
        {GLFW_KEY_RIGHT_SHIFT,   Keyboard::KeyCode::EM_KEY_RIGHT_SHIFT},
        {GLFW_KEY_RIGHT_ALT,     Keyboard::KeyCode::EM_KEY_RIGHT_ALT  },
    };
    static Keyboard::KeyCode MapGlfwKeyboardKeycode(int keycode) {
        auto searchRes = glfwKeyboardMap.find(keycode);
        if (searchRes == glfwKeyboardMap.end())
            return Keyboard::KeyCode::EM_KEY_UNIDENTIFIED;
        return searchRes->second;
    }

    void WindowGlfw::KeyboardKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        KeyboardKeyEventData keyEventData{};
        keyEventData.key = MapGlfwKeyboardKeycode(key);
        keyEventData.scancode = scancode;

        if (keyEventData.key == Keyboard::KeyCode::EM_KEY_UNIDENTIFIED)
            return;
        switch (action) {
        case GLFW_PRESS:
            keyEventData.action = KeyActionType::PRESSED;
            break;
        case GLFW_REPEAT:
            keyEventData.action = KeyActionType::REPEATED;
            break;
        case GLFW_RELEASE:
            keyEventData.action = KeyActionType::RELEASED;
            break;
        }

        WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(glfwGetWindowUserPointer(window));
        assert(windowGlfw && "WindowGlfw* cannot be 'nullptr'. Forgot to set it through 'glfwSetWindowUserPointer'?");
        windowGlfw->eventRegistry->NotifyEventCallbackDelayed(keyEventData);
    }
    void WindowGlfw::WindowCloseEventCallback(GLFWwindow* window) {
        WindowCloseEventData windowCloseEventData{};
        windowCloseEventData.close = true;

        WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(glfwGetWindowUserPointer(window));
        assert(windowGlfw && "WindowGlfw* cannot be 'nullptr'. Forgot to set it through 'glfwSetWindowUserPointer'?");
        windowGlfw->eventRegistry->NotifyEventCallbackDelayed(windowCloseEventData);
    }
    void WindowGlfw::FramebufferResizeEventCallback(GLFWwindow* window, int width, int height) {
        FramebufferResizeEventData framebufferResizeEventData{};
        framebufferResizeEventData.width = width;
        framebufferResizeEventData.height = height;

        WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(glfwGetWindowUserPointer(window));
        assert(windowGlfw && "WindowGlfw* cannot be 'nullptr'. Forgot to set it through 'glfwSetWindowUserPointer'?");

        windowGlfw->windowSettings.framebufferDimensions = Dimensions2D{
            static_cast<uint32_t>(width), static_cast<uint32_t>(height)
        };
        if (width == 0 && height == 0) {
            windowGlfw->isMinimized = true;
        }
        windowGlfw->eventRegistry->NotifyEventCallbackImmediate(framebufferResizeEventData);
        // windowGlfw->eventRegistry->NotifyEventCallbackDelayed(framebufferResizeEventData);
    }
    void WindowGlfw::GlfwErrorCallback(int errorCode, const char* description) {
        std::stringstream errorStream;
        errorStream << "[GLFW error code]: " << errorCode << "\n"
            << "[Error description]: " << description;
        GLFW_ERROR(errorStream.str().c_str());
    }

    void WindowGlfw::ProcessEvents() {
        glfwPollEvents();
    }

}