// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Custom header
#include "StringConv.h"
#include "WMI.h"
#include "Widgets.h"
#include "ImguiHelper.h"
#include "Path.h"
#include "Registry.h"

#include <vector>
#include <iostream>

#include <chrono>
#include <thread> // std::this_thread::sleep_until

struct UninstInfoObj
{
    std::wstring wsKeyName;
    std::wstring wsDisplayName;
    std::wstring wsDisplayVersion;
};

void GetUninstList(std::vector<UninstInfoObj>& uninstInfoObj)
{
    Registry reg;
    HKEY hKey;

    // X64: HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall     
    LSTATUS lResult = ::RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        0,
        KEY_ALL_ACCESS,
        &hKey);

    if (lResult != ERROR_SUCCESS)
    {
        std::cout << "open fail" << std::endl;
        return;
    }
    {
        std::wstring wsKeyname;
        DWORD dwPathsize = MAX_PATH;
        wsKeyname.resize(dwPathsize);
        DWORD index = 0;
        UninstInfoObj UninstObj;
        while (::RegEnumKeyExW(hKey, index, &wsKeyname[0], &dwPathsize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            wsKeyname.resize(dwPathsize);

            std::wcout << L"-------------------------- Key: " << wsKeyname << std::endl;
            UninstObj.wsKeyName = wsKeyname;

            reg.SetRootAndPath(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + wsKeyname);
            RegResult RegRes = reg.GetString(L"DisplayName");

            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayName = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayName not found" << std::endl;
                UninstObj.wsDisplayName = L"(" + wsKeyname + L")";
            }

            RegRes = reg.GetString(L"DisplayVersion");
            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayVersion = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayVersion not found" << std::endl;
                UninstObj.wsDisplayVersion = L"N/A";
            }

            UninstObj.wsDisplayVersion += L" (X64)";

            uninstInfoObj.push_back(UninstObj);

            // Reset for next iteration
            dwPathsize = MAX_PATH;
            wsKeyname.clear();
            wsKeyname.resize(dwPathsize);
            ++index;
        }
    }
    ::RegCloseKey(hKey);

    // X86: HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall
    lResult = ::RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        0,
        KEY_ALL_ACCESS,
        &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        std::cout << "open fail" << std::endl;
        return;
    }
    {
        std::wstring wsKeyname;
        DWORD dwPathsize = MAX_PATH;
        wsKeyname.resize(dwPathsize);
        DWORD index = 0;
        UninstInfoObj UninstObj;
        while (::RegEnumKeyExW(hKey, index, &wsKeyname[0], &dwPathsize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            wsKeyname.resize(dwPathsize);

            std::wcout << L"-------------------------- Key: " << wsKeyname << std::endl;
            UninstObj.wsKeyName = wsKeyname;

            reg.SetRootAndPath(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + wsKeyname);
            RegResult RegRes = reg.GetString(L"DisplayName");

            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayName = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayName not found" << std::endl;
                UninstObj.wsDisplayName = L"(" + wsKeyname + L")";
            }

            RegRes = reg.GetString(L"DisplayVersion");
            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayVersion = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayVersion not found" << std::endl;
                UninstObj.wsDisplayVersion = L"N/A";
            }

            UninstObj.wsDisplayVersion += L" (X86)";

            uninstInfoObj.push_back(UninstObj);

            // Reset for next iteration
            dwPathsize = MAX_PATH;
            wsKeyname.clear();
            wsKeyname.resize(dwPathsize);
            ++index;
        }
    }
    ::RegCloseKey(hKey);

    // HKCU X64: HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall
    lResult = ::RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        0,
        KEY_ALL_ACCESS,
        &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        std::cout << "open fail" << std::endl;
        return;
    }
    {
        std::wstring wsKeyname;
        DWORD dwPathsize = MAX_PATH;
        wsKeyname.resize(dwPathsize);
        DWORD index = 0;
        UninstInfoObj UninstObj;
        while (::RegEnumKeyExW(hKey, index, &wsKeyname[0], &dwPathsize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            wsKeyname.resize(dwPathsize);

            std::wcout << L"-------------------------- Key: " << wsKeyname << std::endl;
            UninstObj.wsKeyName = wsKeyname;

            reg.SetRootAndPath(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + wsKeyname);
            RegResult RegRes = reg.GetString(L"DisplayName");

            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayName = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayName not found" << std::endl;
                UninstObj.wsDisplayName = L"(" + wsKeyname + L")";
            }

            RegRes = reg.GetString(L"DisplayVersion");
            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayVersion = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayVersion not found" << std::endl;
                UninstObj.wsDisplayVersion = L"N/A";
            }

            UninstObj.wsDisplayVersion += L" (HKCU_X64)";

            uninstInfoObj.push_back(UninstObj);

            // Reset for next iteration
            dwPathsize = MAX_PATH;
            wsKeyname.clear();
            wsKeyname.resize(dwPathsize);
            ++index;
        }
    }
    ::RegCloseKey(hKey);

    /*
    // HKCU X86: HKEY_CURRENT_USER\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall
    lResult = ::RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        0,
        KEY_ALL_ACCESS,
        &hKey);
    if (lResult != ERROR_SUCCESS)
    {
        std::cout << "open fail" << std::endl;
        return;
    }
    {
        std::wstring wsKeyname;
        DWORD dwPathsize = MAX_PATH;
        wsKeyname.resize(dwPathsize);
        DWORD index = 0;
        UninstInfoObj UninstObj;
        while (::RegEnumKeyExW(hKey, index, &wsKeyname[0], &dwPathsize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            wsKeyname.resize(dwPathsize);

            std::wcout << L"-------------------------- Key: " << wsKeyname << std::endl;

            reg.SetRootAndPath(HKEY_CURRENT_USER, L"Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + wsKeyname);
            RegResult RegRes = reg.GetString(L"DisplayName");

            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayName = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayName not found" << std::endl;
                UninstObj.wsDisplayName = L"(" + wsKeyname + L")";
            }

            RegRes = reg.GetString(L"DisplayVersion");
            if (RegRes.bSuc)
            {
                UninstObj.wsDisplayVersion = RegRes.wsValue;
            }
            else
            {
                std::cout << "DisplayVersion not found" << std::endl;
                UninstObj.wsDisplayVersion = L"N/A";
            }

            UninstObj.wsDisplayVersion += L" (HKCU_X86)";

            uninstInfoObj.push_back(UninstObj);

            // Reset for next iteration
            dwPathsize = MAX_PATH;
            wsKeyname.clear();
            wsKeyname.resize(dwPathsize);
            ++index;
        }
    }
    ::RegCloseKey(hKey);
    */

}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(700, 720, "Driver Viewer", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\calibri.ttf", 19.0f);
    IM_ASSERT(font != NULL);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    WmiInfo wmi;
    std::vector<SystemDriverObj> vSysDrvObj = wmi.GetSystemDriverInfo(L"SELECT DisplayName,PathName,Name from Win32_SystemDriver");

    // Convert list to ANSI, Its stupid to do this, but no choice
    std::vector<std::vector<std::string>> vSysDrvObjANSI;
    for (int i = 0; i < vSysDrvObj.size(); i++)
    {
        vSysDrvObjANSI.push_back({
            STDWStringToSTDString(vSysDrvObj[i].wsDisplayName),
            STDWStringToSTDString(vSysDrvObj[i].wsFileVer),
            STDWStringToSTDString(vSysDrvObj[i].wsName) });
    }

    // Get Uninstall app list obj
    std::vector<UninstInfoObj> vUninstList;
    GetUninstList(vUninstList);
    // Convert list to ANSI, Its stupid to do this, but no choice
    std::vector<std::vector<std::string>> vUninstObjANSI;
    for (int i = 0; i < vUninstList.size(); i++)
    {
        vUninstObjANSI.push_back({
            STDWStringToSTDString(vUninstList[i].wsDisplayName),
            STDWStringToSTDString(vUninstList[i].wsDisplayVersion),
            STDWStringToSTDString(vUninstList[i].wsKeyName) });
    }

    using clock = std::chrono::steady_clock;
    auto next_frame = clock::now();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Limit FPS to 30Hz
        next_frame += std::chrono::milliseconds(1000 / 20);

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            SetFitParentWindow(window);

            // ImGui Main Begin
            ImGui::Begin("Driver Seeker", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);                          // Create a window called "Hello, world!" and append into it.

            // Stretch items
            ImGui::PushItemWidth(-70);

            // Hwids list 
            int w = 0;
            int h = 0;
            glfwGetWindowSize(window, &w, &h);


            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("System Drivers"))
                {
                    CustomWidget::DriverTable(
                        "hwidraw",
                        "System Drivers",
                        ImVec2(0, h - 105),
                        ImVec2(0, 0),
                        L"drivers.txt",
                        vSysDrvObjANSI);

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Utilities"))
                {
                    CustomWidget::DriverTable(
                        "uninst",
                        "Uninstall List",
                        ImVec2(0, h - 105),
                        ImVec2(0, 0),
                        L"utilities.txt",
                        vUninstObjANSI);

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        // wait for end of frame
        std::this_thread::sleep_until(next_frame);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
