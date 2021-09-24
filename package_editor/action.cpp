// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/action.hpp>
#include <package_editor/node.hpp>
#include <package_editor/main_window.hpp>

namespace wingstall { namespace package_editor {

OpenAndExpandAction::OpenAndExpandAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void OpenAndExpandAction::Execute()
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        try
        {
            mainWindow->HideContextMenu();
            node->OpenAndExpand();
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
    }
}

OpenAction::OpenAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void OpenAction::Execute() 
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        try
        {
            mainWindow->HideContextMenu();
            node->Open();
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
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
        try
        {
            mainWindow->HideContextMenu();
            node->AddNew(kind);
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
    }
}

EditAction::EditAction(MenuItem* menuItem_, Node* node_) : ClickAction(menuItem_), node(node_)
{
}

void EditAction::Execute()
{
    MainWindow* mainWindow = node->GetMainWindow();
    if (mainWindow)
    {
        try
        {
            mainWindow->HideContextMenu();
            node->Edit();
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
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
        try
        {
            mainWindow->HideContextMenu();
            node->Remove();
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
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
        try
        {
            mainWindow->HideContextMenu();
            node->MoveUp();
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
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
        try
        {
            mainWindow->HideContextMenu();
            node->MoveDown();
        }
        catch (const std::exception& ex)
        {
            ShowErrorMessageBox(mainWindow->Handle(), ex.what());
        }
    }
}

} } // wingstall::package_editor