// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED
#include <package_editor/node.hpp>
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace package_editor {

class EnvironmentVariable;
class PathDirectory;

class Environment : public Node
{
public:
    Environment();
    Environment(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    std::string ImageName() const override { return "environment.bitmap"; }
    void AddEnvironentVariable(EnvironmentVariable* environmentVariable);
    void AddPathDirectory(PathDirectory* pathDirectory);
private:
    std::vector<std::unique_ptr<EnvironmentVariable>> environmentVariables;
    std::vector<std::unique_ptr<PathDirectory>> pathDirectories;
};

class EnvironmentVariable : public Node
{
public:
    EnvironmentVariable();
    EnvironmentVariable(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    std::string ImageName() const override { return "environment.var.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
    TreeViewNode* ToTreeViewNode(TreeView* view);
private:
    std::string value;
};

class PathDirectory : public Node
{
public:
    PathDirectory();
    PathDirectory(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    std::string ImageName() const override { return "path.directory.bitmap"; }
    void SetData(ListViewItem* item, ImageList* imageList) override;
    TreeViewNode* ToTreeViewNode(TreeView* view);
private:
    std::string value;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ENVIRONMENT_INCLUDED

