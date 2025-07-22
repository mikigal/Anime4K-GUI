#ifndef WINDOWUTILITIES_H
#define WINDOWUTILITIES_H

#ifdef _WIN32
#include "pch.h"
#include <dwmapi.h>
#include <windows.h>
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
    };
}
#endif

#endif //WINDOWUTILITIES_H
