// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_ICON_INCLUDED
#define WING_ICON_INCLUDED
#include <wing/ResourceManager.hpp>
#include <Windows.h>

namespace wing {

class WING_API Icon : public Resource
{
public:
    Icon();
    Icon(HICON handle_);
    Icon(HICON handle_, bool standard_);
    Icon(const Icon&) = delete;
    Icon(Icon&& that) noexcept;
    Icon& operator=(const Icon&) = delete;
    Icon& operator=(Icon&& that) noexcept;
    ~Icon();
    bool IsIcon() const override { return true; }
    HICON Handle() const { return handle; }
    bool Standard() const { return standard; }
private:
    HICON handle;
    bool standard;
};

WING_API Icon LoadStandardIcon(int standardIconId);
WING_API Icon LoadIconByName(const std::string& iconName);

} // wing

#endif // WING_ICON_INCLUDED
