// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_CONTAINER_CONTROL_INCLUDED
#define WING_CONTAINER_CONTROL_INCLUDED
#include <wing/Control.hpp>
#include <wing/Container.hpp>

namespace wing {

class WING_API ContainerControl : public Control
{
public:
    ContainerControl(ControlCreateParams& createParams);
    bool IsContainerControl() const override { return true; }
    void PaintAll(PaintEventArgs& args, bool skipMenuBar) override;
    ContainerControl* GetContainerControl() const override;
    void AddChild(Control* child);
    std::unique_ptr<Control> RemoveChild(Control* child);
    const Container& Children() const { return children; }
    Container& Children() { return children; }
    void InsertChildBefore(Control* child, Control* before);
    void InsertChildAfter(Control* child, Control* after);
    void DockChildren();
    void DockChildren(Rect& parentRect);
    void MoveChildren(int dx, int dy);
    Control* GetFirstEnabledTabStopControl() const override;
    Control* GetLastEnabledTabStopControl() const override;
protected:
    bool ProcessMessage(Message& msg) override;
    void OnSizeChanging(SizeChangingEventArgs& args) override;
    void OnChildSizeChanged(ControlEventArgs& args) override;
    void OnChildContentChanged(ControlEventArgs& args) override;
    void OnChildContentLocationChanged(ControlEventArgs& args) override;
    void OnChildContentSizeChanged(ControlEventArgs& args) override;
    void OnChildGotFocus(ControlEventArgs& args) override;
    void OnChildLostFocus(ControlEventArgs& args) override;
private:
    Container children;
};

} // wing

#endif // WING_CONTAINER_CONTROL_INCLUDED
