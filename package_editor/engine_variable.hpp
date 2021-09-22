// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_ENGINE_VARIABLE_INCLUDED
#define WINGSTALL_PACKAGE_ENGINE_VARIABLE_INCLUDED
#include <package_editor/node.hpp>

namespace wingstall { namespace package_editor {

class EngineVariable;

class EngineVariables : public Node
{
public:
    EngineVariables();
    void Fetch();
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "engine.variables.bitmap"; }
private:
    std::vector<std::unique_ptr<EngineVariable>> engineVariables;
};

class EngineVariable : public Node
{
public:
    EngineVariable();
    TreeViewNode* ToTreeViewNode(TreeView* view);
    std::string ImageName() const override { return "engine.variable.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
    virtual std::string Value() const = 0;
};

class TargetRootDirVariable : public EngineVariable
{
public:
    TargetRootDirVariable();
    std::string Value() const override;
};

class AppNameVariable : public EngineVariable
{
public:
    AppNameVariable();
    std::string Value() const override;
};

class AppVersionVariable : public EngineVariable
{
public:
    AppVersionVariable();
    std::string Value() const override;
};

class ProductIdVariable : public EngineVariable
{
public:
    ProductIdVariable();
    std::string Value() const override;
};

class PublisherVariable : public EngineVariable
{
public:
    PublisherVariable();
    std::string Value() const override;
};

class StartMenuProgramsFolderVariable : public EngineVariable
{
public:
    StartMenuProgramsFolderVariable();
    std::string Value() const override;
};

class DesktopFolderVariable : public EngineVariable
{
public:
    DesktopFolderVariable();
    std::string Value() const override;
};

class ProgramFilesDirVariable : public EngineVariable
{
public:
    ProgramFilesDirVariable();
    std::string Value() const override;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_ENGINE_VARIABLE_INCLUDED
