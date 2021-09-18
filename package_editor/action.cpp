// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/action.hpp>
#include <package_editor/node.hpp>
#include <package_editor/main_window.hpp>

namespace wingstall { namespace package_editor {

OpenAction::OpenAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void OpenAction::Execute() 
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->HideContextMenu();
        node->Open();
    }
}

AddAction::AddAction(MenuItem* menuItem_, Node* node_, NodeKind kind_) : ClickAction(menuItem_), node(node_), kind(kind_)
{
}

void AddAction::Execute()
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->HideContextMenu();
        node->AddNew(kind);
    }
}

RemoveAction::RemoveAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void RemoveAction::Execute() 
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->HideContextMenu();
        node->Remove();
    }
}

MoveUpAction::MoveUpAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void MoveUpAction::Execute()
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->HideContextMenu();
        node->MoveUp();
    }
}

MoveDownAction::MoveDownAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void MoveDownAction::Execute()
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        mainWindow->HideContextMenu();
        node->MoveDown();
    }
}

} } // wingstall::package_editor