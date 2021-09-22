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
enum class NodeKind;

class OpenAction : public ClickAction
{
public:
    OpenAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class AddAction : public ClickAction
{
public:
    AddAction(MenuItem* menuItem_, Node* node_, NodeKind kind_);
    void Execute() override;
private:
    Node* node;
    NodeKind kind;
};

class EditAction : public ClickAction
{
public:
    EditAction(MenuItem* menuItem_, Node* node_);
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