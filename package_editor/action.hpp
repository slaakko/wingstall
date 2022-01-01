// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ACTION_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ACTION_INCLUDED
#include <wing/Menu.hpp>

namespace wingstall { namespace package_editor {

using namespace wing;

class Node;
enum class NodeKind;

class OpenAndExpandAction : public ClickAction
{
public:
    OpenAndExpandAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

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

class AddVSRulesAction : public ClickAction
{
public:
    AddVSRulesAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class ExcludeAction : public ClickAction
{
public:
    ExcludeAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class IncludeAction : public ClickAction
{
public:
    IncludeAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class SaveAction : public ClickAction
{
public:
    SaveAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class BuildAction : public ClickAction
{
public:
    BuildAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class CloseAction : public ClickAction
{
public:
    CloseAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

class ClearAction : public ClickAction
{
public:
    ClearAction(MenuItem* menuItem_, Node* node_);
    void Execute() override;
private:
    Node* node;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ACTION_INCLUDED
