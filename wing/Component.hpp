// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_COMPONENT_INCLUDED
#define WING_COMPONENT_INCLUDED
#include <wing/WingApi.hpp>
#include <stdint.h>

namespace wing {

class Container;

class WING_API Component
{
public:
    Component();
    virtual ~Component();
    virtual bool IsControl() const { return false; }
    virtual bool IsMenuItem() const { return false; }
    virtual bool IsMenuItemBase() const { return false; }
    virtual bool IsMenuControl() const { return false; }
    virtual bool IsTreeViewNode() const { return false; }
    virtual bool IsTabPage() const { return false; }
    virtual bool IsToolBar() const { return false; }
    virtual bool IsToolButtonBase() const { return false; }
    virtual bool IsToolButton() const { return false; }
    virtual bool IsStatusBar() const { return false; }
    virtual bool IsStatusBarItem() const { return false; }
    virtual bool IsStatusBarSpringItem() const { return false; }
    virtual bool IsStatusBarControlItem() const { return false; }
    virtual bool IsIconListView() const { return false; }
    virtual bool IsIconListViewItem() const { return false; }
    virtual bool IsPathComponent() const { return false; }
    Container* GetContainer() const { return container; }
    void SetContainer(Container* container_) { container = container_; }
    Component* NextSibling() const { return nextSibling; }
    void SetNextSibling(Component* nextSibling_) { nextSibling = nextSibling_; }
    Component* PrevSibling() const { return prevSibling; }
    void SetPrevSibling(Component* prevSibling_) { prevSibling = prevSibling_; }
    void LinkBefore(Component* component) 
    {
        if (prevSibling)
        {
            prevSibling->nextSibling = component;
        }
        component->prevSibling = prevSibling;
        component->nextSibling = this;
        prevSibling = component;
    }
    void LinkAfter(Component* component)
    {
        if (nextSibling)
        {
            nextSibling->prevSibling = component;
        }
        component->prevSibling = this;
        component->nextSibling = nextSibling;
        nextSibling = component;
    }
    void Unlink()
    {
        if (prevSibling)
        {
            prevSibling->nextSibling = nextSibling;
        }
        if (nextSibling)
        { 
            nextSibling->prevSibling = prevSibling;
        }
    }
private:
    Container* container;
    Component* nextSibling;
    Component* prevSibling;
};

} // wing

#endif // WING_COMPONENT_INCLUDED
