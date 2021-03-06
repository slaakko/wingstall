// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_COMTAINER_INCLUDED
#define WING_COMTAINER_INCLUDED
#include <wing/WingApi.hpp>
#include <memory>

namespace wing {

class Component;

class WING_API Container
{
public:
    Container(Component* parent_);
    ~Container();
    bool IsEmpty() const { return firstChild == nullptr; }
    Component* Parent() const { return parent; }
    Component* FirstChild() const { return firstChild; }
    Component* LastChild() const { return lastChild; }
    void AddChild(Component* child);
    std::unique_ptr<Component> RemoveChild(Component* child);
    void InsertBefore(Component* child, Component* before);
    void InsertAfter(Component* child, Component* after);
private:
    Component* parent;
    Component* firstChild;
    Component* lastChild;
};

} // wing

#endif // WING_COMTAINER_INCLUDED
