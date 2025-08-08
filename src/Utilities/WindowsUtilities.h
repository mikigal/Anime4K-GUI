#ifndef WINDOWUTILITIES_H
#define WINDOWUTILITIES_H

#ifdef _WIN32
#include <dwmapi.h>
#include <windows.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define GLFW_EXPOSE_NATIVE_WIN32

#pragma comment(lib, "dwmapi.lib")
#include <winternl.h>
typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);


namespace Upscaler {
    class Windows {
    public:
        static void EnableWindowDarkMode(GLFWwindow* window) {
            HWND hwnd = glfwGetWin32Window(window);
            SetWindowAttribute(hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, true);
            SetWindowAttribute(hwnd, 19 /* DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 */, true);
        }

        static void SetWindowAttribute(HWND hwnd, DWORD attribute, BOOL value) {
            (void)DwmSetWindowAttribute(hwnd, attribute, &value, sizeof(value));
        }

        static void SetIcon(GLFWwindow* window) {
            HWND hwnd = glfwGetWin32Window(window);
            HINSTANCE hInstance = GetModuleHandle(NULL);
            HICON hIconBig = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(101), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
            HICON hIconSmall = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(101), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) hIconBig);
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) hIconSmall);

            DestroyIcon(hIconBig);
            DestroyIcon(hIconSmall);
        }

        static void PreventSleep() {
            if (!SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED)) {
                m_Logger.Error("Failed to block sleep/hibernation. Error code: {}", GetLastError());
            }
        }
    };
}
#endif

#endif //WINDOWUTILITIES_H
