// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_PANEL_INCLUDED
#define WING_PANEL_INCLUDED
#include <wing/ContainerControl.hpp>

namespace wing {

class WING_API Panel : public ContainerControl
{
public:
    Panel(ControlCreateParams& createParams);
    bool IsPanel() const override { return true; }
};

} // wing

#endif // WING_PANEL_INCLUDED
