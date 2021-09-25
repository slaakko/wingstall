// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_TOOL_BAR_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_TOOL_BAR_INCLUDED
#include <wing/ToolBar.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

Color DefaultToolBarBorderColor();
Color DefaultToolBarTextButtonColor();
Color DefaultToolBarToolTipColor();
int DefaultToolBarToolButtonHeight();
Padding DefaultToolBarPadding();
Padding DefaultToolBarToolButtonPadding();

ToolBar* MakeToolBar();

} } // namespace wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_TOOL_BAR_INCLUDED
