// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PATH_BAR_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PATH_BAR_INCLUDED
#include <package_editor/node.hpp>
#include <wing/ContainerControl.hpp>
#include <wing/ListView.hpp>
#include <wing/PathView.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class DirectoryPathView;
class ParentPathSelector;
class PathDivider;

Color DefaultPathBarBackgroundColor();
Color DefaultPathBarFrameColor();
Color DefaultDirectoryPathViewBackgroundColor();
Color DefaultParentPathSelectorBackgroundColor();
Color DefaultParentPathSelectorMouseOverColor();
Color DefaultParentPathSelectorMouseClickColor();
Color DefaultPathDividerBackgroundColor();
Color DefaultPathDividerLineColor();
std::string DefaultDirectoryPathViewFontFamilyName();
float DefaultDirectoryPathViewFontSize();

struct DirectoryPathViewCreateParams
{
    DirectoryPathViewCreateParams();
    DirectoryPathViewCreateParams& Defaults();
    DirectoryPathViewCreateParams& WindowClassName(const std::string& windowClassName_);
    DirectoryPathViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    DirectoryPathViewCreateParams& WindowStyle(int windowStyle_);
    DirectoryPathViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    DirectoryPathViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    DirectoryPathViewCreateParams& Text(const std::string& text_);
    DirectoryPathViewCreateParams& Location(Point location_);
    DirectoryPathViewCreateParams& SetSize(Size size_);
    DirectoryPathViewCreateParams& SetAnchors(Anchors anchors_);
    DirectoryPathViewCreateParams& SetDock(Dock dock_);
    DirectoryPathViewCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    DirectoryPathViewCreateParams& FontSize(float fontSize_);
    wing::ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
};

struct ParentPathSelectorCreateParams
{
    ParentPathSelectorCreateParams(ImageList* imageList_);
    ParentPathSelectorCreateParams& Defaults();
    ParentPathSelectorCreateParams& WindowClassName(const std::string& windowClassName_);
    ParentPathSelectorCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ParentPathSelectorCreateParams& WindowStyle(int windowStyle_);
    ParentPathSelectorCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ParentPathSelectorCreateParams& BackgroundColor(const Color& backgroundColor_);
    ParentPathSelectorCreateParams& Text(const std::string& text_);
    ParentPathSelectorCreateParams& Location(Point location_);
    ParentPathSelectorCreateParams& SetSize(Size size_);
    ParentPathSelectorCreateParams& SetAnchors(Anchors anchors_);
    ParentPathSelectorCreateParams& SetDock(Dock dock_);
    ParentPathSelectorCreateParams& MouseOverColor(const Color& mouseOverColor_);
    ParentPathSelectorCreateParams& MouseClickColor(const Color& mouseClickColor_);
    wing::ControlCreateParams controlCreateParams;
    ImageList* imageList;
    Color mouseOverColor;
    Color mouseClickColor;
};

struct PathDividerCreateParams
{
    PathDividerCreateParams();
    PathDividerCreateParams& Defaults();
    PathDividerCreateParams& WindowClassName(const std::string& windowClassName_);
    PathDividerCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    PathDividerCreateParams& WindowStyle(int windowStyle_);
    PathDividerCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    PathDividerCreateParams& BackgroundColor(const Color& backgroundColor_);
    PathDividerCreateParams& Text(const std::string& text_);
    PathDividerCreateParams& Location(Point location_);
    PathDividerCreateParams& SetSize(Size size_);
    PathDividerCreateParams& SetAnchors(Anchors anchors_);
    PathDividerCreateParams& SetDock(Dock dock_);
    PathDividerCreateParams& PenColor(const Color& penColor_);
    wing::ControlCreateParams controlCreateParams;
    Color penColor;
};

struct PathBarCreateParams
{
    PathBarCreateParams(ImageList* imageList_);
    PathBarCreateParams& Defaults();
    PathBarCreateParams& WindowClassName(const std::string& windowClassName_);
    PathBarCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    PathBarCreateParams& WindowStyle(int windowStyle_);
    PathBarCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    PathBarCreateParams& BackgroundColor(const Color& backgroundColor_);
    PathBarCreateParams& Text(const std::string& text_);
    PathBarCreateParams& Location(Point location_);
    PathBarCreateParams& SetSize(Size size_);
    PathBarCreateParams& SetAnchors(Anchors anchors_);
    PathBarCreateParams& SetDock(Dock dock_);
    PathBarCreateParams& FrameColor(const Color& frameColor_);
    wing::ControlCreateParams controlCreateParams;
    ImageList* imageList;
    DirectoryPathViewCreateParams directoryPathViewCreateParams;
    ParentPathSelectorCreateParams parentPathSelectorCreateParams;
    PathDividerCreateParams pathDividerCreateParams;
    Color frameColor;
};

class PathBar : public ContainerControl
{
public:
    PathBar(PathBarCreateParams& createParams);
    void SetDirectoryPath(const std::string& directoryPath);
    void SetDirectoryPathViewWidth(int width);
    void SetPathViewMaxWidth(int pathViewMaxWidth);
    ParentPathSelector* GetParentPathSelector() const { return parentPathSelector; }
    PathView* GetPathView() const { return pathView; }
    void SetCurrentNode(Node* currentNode);
    void Show();
    void Hide();
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnSizeChanged() override;
private:
    DirectoryPathView* directoryPathView;
    PathDivider* pathDivider1;
    ParentPathSelector* parentPathSelector;
    PathDivider* pathDivider2;
    PathView* pathView;
};

class DirectoryPathView : public Control
{
public:
    DirectoryPathView(DirectoryPathViewCreateParams& createParams);
    void SetDirectoryPath(const std::string& directoryPath_);
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    std::string directoryPath;
    SolidBrush blackBrush;
};

enum class ParentPathSelectorState : int
{
    idle, mouseOver, clicked
};

class ParentPathSelector : public Control
{
public:
    ParentPathSelector(ParentPathSelectorCreateParams& createParams);
    void SetState(ParentPathSelectorState state_);
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
private:
    ImageList* imageList;
    ParentPathSelectorState state;
    SolidBrush mouseOverBrush;
    SolidBrush mouseClickBrush;
};

class PathDivider : public Control
{
public:
    PathDivider(PathDividerCreateParams& createParams);
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    Pen pen;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PATH_BAR_INCLUDED
