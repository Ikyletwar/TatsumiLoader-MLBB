#include "Android_draw/draw.h"
#include "../font/SabFont.h"
#include "../font/verdana.h"
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;
ANativeWindow *native_window;
ImFont* verdana;
int native_window_screen_x = 0;
int native_window_screen_y = 0;
android::ANativeWindowCreator::DisplayInfo displayInfo{0};
uint32_t orientation = 0;
bool g_Initialized = false;
ImGuiWindow *g_window = nullptr;
bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log) {
    orientation = displayInfo.orientation;
    #if defined(USE_OPENGL)
        if (!init_egl(_screen_x, _screen_y, log)) {
            return false;
        }
    #else
        InitVulkan();
        SetupVulkan();
        ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y, true);
        SetupVulkanWindow(::native_window, (int) _screen_x, (int) _screen_y);
    #endif
    if (!ImGui_init()) {
        return false;
    }   
    #ifndef USE_OPENGL
        UploadFonts();
    #endif
    return true;
}
bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log) {
    FILE *fp;
    char buffer[1024];
    fp = popen("settings put system block_untrusted_touches 0", "r");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    }
    pclose(fp);
    system ("settings put global block_untrusted_touches 0 > /dev/null 2>&1");
    system ("settings put secure block_untrusted_touches 0 > /dev/null 2>&1");
    ::native_window = android::ANativeWindowCreator::Create("AImGui", _screen_x, _screen_y, false);
    ANativeWindow_acquire(native_window);
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        return false;
    }
    if (log) {
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        return false;
    }
    if (log) {
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,   
            EGL_GREEN_SIZE, 6,  
            EGL_RED_SIZE, 5,    
            EGL_BUFFER_SIZE, 32,  
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    const EGLint attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION,
            3,
            EGL_NONE
    };
    if (log) {
    }
    if (eglChooseConfig(display, attribList, &config, 1, &num_config) != EGL_TRUE) {
        return false;
    }
    if (log) {
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        return false;
    }
    if (log) {
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        return false;
    }
    if (log) {
    }
    if (!eglMakeCurrent(display, surface, surface, context)) {
        return false;
    }
    if (log) {
    }
    return true;
}
void screen_config() {
    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
}

bool ImGui_init() {
    if (g_Initialized) {
        return true;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.00f, 0.00f, 0.67f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.80f, 0.00f, 0.00f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.80f, 0.00f, 0.00f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.00f, 0.00f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.80f, 0.00f, 0.00f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.80f, 0.00f, 0.00f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.80f, 0.00f, 0.00f, 0.86f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.00f, 0.00f, 0.97f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.60f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.80f, 0.00f, 0.00f, 0.35f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);

    ImGui_ImplAndroid_Init(native_window);
    #if defined(USE_OPENGL)
        ImGui_ImplOpenGL3_Init("#version 300 es");
    #endif
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.Fonts->AddFontFromMemoryTTF((void*)SabFont, SabFont_size, 26.0f);
    io.FontGlobalScale = 1.0f;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    style.ScaleAllSizes(3.0f);
    style.ScrollbarSize = 18.0f;
    style.ScrollbarRounding = 12.0f;

    ::g_Initialized = true;
    return true;
}
void drawBegin() {
    screen_config();
    if (::orientation != displayInfo.orientation) {
        ::orientation = displayInfo.orientation;
        UpdateScreenData(displayInfo.width, displayInfo.height, displayInfo.orientation);
        if (g_window) {
        g_window->Pos.x = 100;
        g_window->Pos.y = 125;
        }
    }
    #ifdef USE_OPENGL
        ImGui_ImplOpenGL3_NewFrame();
    #else
        ImGui_ImplVulkan_NewFrame();
    #endif        
    ImGui_ImplAndroid_NewFrame(native_window_screen_x, native_window_screen_y);
    ImGui::NewFrame();
}
void drawEnd() {
    ImGui::Render();
    #ifdef USE_OPENGL
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        eglSwapBuffers(display, surface);
    #else
        FrameRender(ImGui::GetDrawData());
        FramePresent();
    #endif
}
void shutdown() {
    if (!g_Initialized) {
        return;
    }
    #ifdef USE_OPENGL
        ImGui_ImplOpenGL3_Shutdown();
    #else
        DeviceWait();
        ImGui_ImplVulkan_Shutdown();
    #endif
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    #ifdef USE_OPENGL
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
            }
            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
            }
            eglTerminate(display);
        }
        display = EGL_NO_DISPLAY;
        context = EGL_NO_CONTEXT;
        surface = EGL_NO_SURFACE;
    #else    
        CleanupVulkanWindow();
        CleanupVulkan();
    #endif
    ANativeWindow_release(native_window);
    android::ANativeWindowCreator::Destroy(native_window);
    ::g_Initialized = false;
}
