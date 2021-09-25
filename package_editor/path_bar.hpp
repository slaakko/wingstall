// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PATH_BAR_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PATH_BAR_INCLUDED
#include <wing/ContainerControl.hpp>
#include <wing/ListView.hpp>
#include <wing/Panel.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class DirectoryPathView;

Color DefaultPathBarBackgroundColor();
Color DefaultPathBarFrameColor();

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
    Color frameColor;
};

class PathBar : public ContainerControl
{
public:
    PathBar(PathBarCreateParams& createParams);
    void SetDirectoryPath(const std::string& directoryPath);
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    DirectoryPathView* directoryPathView;
};

class DirectoryPathView : public Control
{
public:
    DirectoryPathView();
    void SetDirectoryPath(const std::string& directoryPath_);
    void Measure(Graphics& graphics);
    const Size& GetMeasuredSize() const { return measuredSize; }
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    Size measuredSize;
    std::string directoryPath;
    SolidBrush blackBrush;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PATH_BAR_INCLUDED
