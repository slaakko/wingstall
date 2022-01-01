// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_WINDOW_MANAGER_INCLUDED
#define WING_WINDOW_MANAGER_INCLUDED
#include <wing/WingApi.hpp>
#include <Windows.h>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <stdint.h>

namespace wing {

class Control;

class WING_API WindowManager
{
public:
    WindowManager();
    bool IsSystemClassName(const std::string& windowClassName) const;
    uint16_t Register(const std::string& windowClassName, uint32_t windowClassStyle, int windowClassBackgroundColor);
    void AddWindow(Control* window);
    void RemoveWindow(Control* window);
    Control* GetWindow(HWND handle) const;
private:
    std::set<std::string> systemClassNames;
    std::map<std::string, uint16_t> registeredWindowClasses;
    std::unordered_map<HWND, Control*> windowMap;
};

} // wing

#endif // WING_WINDOW_MANAGER_INCLUDED
