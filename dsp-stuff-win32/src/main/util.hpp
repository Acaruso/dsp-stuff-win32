#pragma once

#include <d2d1.h>
#include <sstream>
#include <string>
#pragma comment(lib, "d2d1")

#include "src/shared/shared_util.hpp"

inline void messageBox(const wchar_t* message) {
    MessageBoxW(nullptr, message, L"", 0);
}

inline void messageBoxError(HRESULT hr) {
    wchar_t buffer[64];
    swprintf_s(buffer, 64, L"error:\n%s", toHexStringW(hr).c_str());
    messageBox(buffer);
}

template <class T>
inline void safeRelease(T **resource) {
    if (*resource != nullptr) {
        (*resource)->Release();
        *resource = nullptr;
    }
}

inline D2D1_RECT_F moveRect(D2D1_RECT_F rect, float x, float y) {
    float w = rect.right - rect.left;
    float h = rect.bottom - rect.top;
    return D2D1::RectF(x, y, x + w, y + h);
}

inline void printRect(D2D1_RECT_F rect) {
    wchar_t buffer[128];

    swprintf_s(
        buffer,
        128,
        L"rect:\nleft: %.2f, top: %.2f, right: %.2f, bottom: %.2f",
        rect.left,
        rect.top,
        rect.right,
        rect.bottom
    );

    messageBox(buffer);
}

inline void storeInstanceData(HWND window, LPARAM lParam) {
    CREATESTRUCT* createStruct = (CREATESTRUCT*)lParam;
    void* createParam = createStruct->lpCreateParams;
    SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)createParam);
}

inline void* getInstanceData(HWND window) {
    return (void*)GetWindowLongPtrW(window, GWLP_USERDATA);
}

inline bool getKeyState(UINT keyCode) {
    return (GetKeyState(keyCode) & 0x8000);
}

inline bool getKeyStateNoRepeat(UINT keyCode, LPARAM lParam) {
    return ((GetKeyState(keyCode) & 0x8000) && (lParam & 0x40000000));
}

inline std::string pixelFormatToString(D2D1_PIXEL_FORMAT pixelFormat) {
    std::string pixelFormatStr = "";
    if (pixelFormat.format == DXGI_FORMAT_B8G8R8A8_UNORM) {
        pixelFormatStr = "DXGI_FORMAT_B8G8R8A8_UNORM";
    } else if (pixelFormat.format == DXGI_FORMAT_R8G8B8A8_UNORM) {
        pixelFormatStr = "DXGI_FORMAT_R8G8B8A8_UNORM";
    } else if (pixelFormat.format == DXGI_FORMAT_A8_UNORM) {
        pixelFormatStr = "DXGI_FORMAT_A8_UNORM";
    } else {
        pixelFormatStr = "unknown pixel format";
    }

    std::string alphaModeStr = "";
    if (pixelFormat.alphaMode == D2D1_ALPHA_MODE_IGNORE) {
        alphaModeStr = "D2D1_ALPHA_MODE_IGNORE";
    } else if (pixelFormat.alphaMode == D2D1_ALPHA_MODE_STRAIGHT) {
        alphaModeStr = "D2D1_ALPHA_MODE_STRAIGHT";
    } else if (pixelFormat.alphaMode == D2D1_ALPHA_MODE_PREMULTIPLIED) {
        alphaModeStr = "D2D1_ALPHA_MODE_PREMULTIPLIED";
    } else {
        alphaModeStr = "unknown alpha mode";
    }

    return "pixel format: " + pixelFormatStr + ", alpha mode: " + alphaModeStr;
}

D2D1_RECT_F makeRectF(float x, float y, float w, float h) {
    return D2D1::RectF(x, y, x + w, y + h);
}

// bool isCoordInsideRect(unsigned x, unsigned y, D2D1_RECT_F rect) {

// }
