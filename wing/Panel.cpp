// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Panel.hpp>

namespace wing {

ControlCreateParams& MakePanelCreateParams(ControlCreateParams& createParams)
{
    if (createParams.windowClassName.empty())
    { 
        createParams.WindowClassName("wing.Panel");
    }
    createParams.WindowStyle(DefaultChildWindowStyle());
    return createParams;
}

Panel::Panel(ControlCreateParams& createParams) : ContainerControl(MakePanelCreateParams(createParams))
{
}

} // wing
