// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/package_content_view.hpp>
#include <package_editor/main_window.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <wing/LogView.hpp>

namespace wingstall { namespace package_editor {

EmptyView::EmptyView() : Control(ControlCreateParams().WindowClassName("wingstall.package_editor.empty_view").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White))
{
}

void EmptyView::OnPaint(PaintEventArgs& args)
{
    args.graphics.Clear(BackgroundColor());
    Control::OnPaint(args);
}

Color DefaultPackageContentViewBackgroundColor()
{
    return Color::White;
}

Color DefaultPackageContentViewFrameColor()
{
    return Color(204, 206, 219);
}

PackageContentViewCreateParams::PackageContentViewCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("wingstall.package_editor.package_content_view");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultPackageContentViewBackgroundColor());
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    frameColor = DefaultPackageContentViewFrameColor();
}

PackageContentViewCreateParams& PackageContentViewCreateParams::Defaults()
{
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::BackgroundColor(const Color& backgroundColor_) 
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

PackageContentViewCreateParams& PackageContentViewCreateParams::FrameColor(const Color& frameColor_)
{
    frameColor = frameColor_;
    return* this;
}

PackageContentView::PackageContentView(PackageContentViewCreateParams& createParams) : 
    ContainerControl(createParams.controlCreateParams), frameColor(createParams.frameColor), imageList(nullptr), framedChild(nullptr), child(nullptr), mainWindow(nullptr)
{
    AddChild(MakeFramedControl(new EmptyView()));
}

void PackageContentView::OnPaint(PaintEventArgs& args)
{
    try
    {
        if (child)
        {
            child->Invalidate();
        }
        if (framedChild)
        {
            framedChild->Invalidate();
        }
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void PackageContentView::ViewContent(Node* node)
{
    if (node)
    {
        bool cancel = false;
        CancelArgs cancelArgs(cancel);
        mainWindow->ExitView().Fire(cancelArgs);
        if (cancelArgs.cancel)
        {
            return;
        }
        if (framedChild)
        {
            framedChild->Hide();
            RemoveChild(framedChild);
            framedChild = nullptr;
            child = nullptr;
        }
        node->ResetDirectoryPath();
        Control* view = node->CreateView(imageList);
        if (!view)
        {
            view = new EmptyView();
        }
        std::unique_ptr<Control> framedView(MakeFramedControl(view));
        AddChild(framedView.release());
    }
    else
    {
        if (framedChild)
        {
            framedChild->Hide();
            RemoveChild(framedChild);
            framedChild = nullptr;
            child = nullptr;
        }
    }
}

Control* PackageContentView::MakeFramedControl(Control* child_)
{
    child = child_;
    child->SetFlag(ControlFlags::scrollSubject);
    PaddedControl* paddedControl = new PaddedControl(PaddedControlCreateParams(child).Defaults());
    BorderedControl* borderedControl = new BorderedControl(BorderedControlCreateParams(paddedControl).SetBorderStyle(BorderStyle::single).NormalSingleBorderColor(frameColor));
    ScrollableControl* scrollableControl = new ScrollableControl(ScrollableControlCreateParams(borderedControl).SetDock(Dock::fill));
    framedChild = scrollableControl;
    return scrollableControl;
}

} } // wingstall::package_editor
