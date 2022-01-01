// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/tool_bar.hpp>

namespace wingstall { namespace package_editor {

Color DefaultToolBarBorderColor()
{
    return Color(204, 206, 219);
}

Color DefaultToolBarTextButtonColor()
{
    return Color(0, 78, 170);
}

Color DefaultToolBarToolTipColor()
{
    return Color(241, 242, 247);
}

int DefaultToolBarToolButtonHeight()
{
    return 30;
}

Padding DefaultToolBarPadding()
{
    return Padding(4, 4, 4, 4);
}

Padding DefaultToolBarToolButtonPadding()
{
    return Padding(2, 0, 2, 0);
}

ToolBar* MakeToolBar()
{
    return new ToolBar(ToolBarCreateParams().SetDock(Dock::none).ToolButtonHeight(DefaultToolBarToolButtonHeight()).
        ToolBarPadding(DefaultToolBarPadding()).ToolBarToolButtonPadding(DefaultToolBarToolButtonPadding()).ToolTipWindowColor(DefaultToolBarToolTipColor()));
}

} } // namespace wingstall::package_editor
