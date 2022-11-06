#pragma once

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "src/main/rect_wh.hpp"
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

inline D2D1_RECT_F moveRect(D2D1_RECT_F rect, float x, float y) {
    float w = rect.right - rect.left;
    float h = rect.bottom - rect.top;
    return D2D1::RectF(x, y, x + w, y + h);
}

inline D2D1_RECT_F makeOffsetRect(D2D1_RECT_F rect, int xOffset, int yOffset) {
    return D2D1::RectF(
        rect.left   + xOffset, 
        rect.top    + yOffset, 
        rect.right  + xOffset,
        rect.bottom + yOffset
    );
}

inline D2D1_POINT_2F makeOffsetPoint(D2D1_POINT_2F point, int xOffset, int yOffset) {
    return D2D1::Point2F(point.x + xOffset, point.y + yOffset);
}

inline RectWH makeRectWH(D2D1_RECT_F rectF) {
    RectWH rectWH;
    rectWH.x = (int)(rectF.left);
    rectWH.y = (int)(rectF.top);
    rectWH.w = (int)(rectF.right - rectF.left);
    rectWH.h = (int)(rectF.bottom - rectF.top);
    return rectWH;
}

inline D2D1_RECT_F makeRectF(RectWH rectWH) {
    return D2D1::RectF(
        (float)(rectWH.x),
        (float)(rectWH.y),
        (float)(rectWH.x + rectWH.w),
        (float)(rectWH.y + rectWH.h)
    );
}

inline D2D1_RECT_F makeRectF(float x, float y, float w, float h) {
    return D2D1::RectF(x, y, x + w, y + h);
}

inline bool isInsideRect(int x, int y, D2D1_RECT_F rect) {
    return (x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom);
}

inline D2D1_COLOR_F makeInvertedColor(D2D1_COLOR_F& color) {
    return D2D1::ColorF(1.0f - color.r, 1.0f - color.g, 1.0f - color.b, color.a);
}

inline std::string colorToString(D2D1_COLOR_F& color) {
    std::stringstream ss;
    ss << "r: " << color.r << " g: " << color.g << " b: " << color.b << " a: " << color.a;
    return ss.str();
}

inline std::string rectToString(D2D1_RECT_F& rect) {
    std::stringstream ss;
    ss << "left: " << rect.left << " top: " << rect.top << " right: " << rect.right << " bottom: " << rect.bottom;
    return ss.str();
}

inline int clamp(int value, int low, int high) {
    if (value >= low && value < high) {
        return value;
    } else if (value < low) {
        return low;
    } else if (value >= high) {
        return high - 1;
    } else {
        return 0;
    }
}

inline float clamp(float value, float low, float high) {
    if (value >= low && value < high) {
        return value;
    } else if (value < low) {
        return low;
    } else if (value >= high) {
        return high;
    } else {
        return 0;
    }
}

inline int getNumDigits(int x) {
    float f_x = (float)x;
    int count = 0;

    while (f_x >= 1) {
        f_x = f_x / 10.0f;
        ++count;
    }

    return count;
}

// example: alignRight(2, 3) -> "  2"
template<typename T>
inline std::wstring alignRight(T x, int size) {
    std::wstringstream ss;
    ss << std::setw(size) << x;
    return ss.str();
}

// example: zeroExtendRight(2, 3) -> "200"
inline std::wstring zeroExtendRight(int x, int size) {
    std::wstringstream ss;
    ss << std::setw(size) << std::setfill(L'0') << std::left;
    ss << x;
    return ss.str();
}

// inline float round(float f, int precision) {
//     float c = pow(10, precision);
//     return (std::round(f * c)) / c;
// }

inline float _round(float f, int precision) {
    float c = pow(10, precision);
    float x1 = f * c;
    float x2 = std::round(x1);
    float x3 = x2 / c;
    return x3;
}

inline int modDec(int val, int modulus) {
    --val;
    if (val < 0) {
        return modulus - 1;
    } else {
        return val;
    }
}
