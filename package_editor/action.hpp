// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ACTION_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ACTION_INCLUDED
#include <wing/Menu.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class Node;

class OpenAction : public ClickAction
{
public:
    OpenAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class RemoveAction : public ClickAction
{
public:
    RemoveAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class MoveUpAction : public ClickAction
{
public:
    MoveUpAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class MoveDownAction : public ClickAction
{
public:
    MoveDownAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ACTION_INCLUDED
