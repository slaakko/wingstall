// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_CONTENT_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_CONTENT_INCLUDED
#include <package_editor/rule.hpp>

namespace wingstall { namespace package_editor {

class ContentDirectory;
class ContentFile;

class Content : public Node
{
public:
    Content();
    std::string ImageName() const override { return "document.collection.bitmap"; }
    TreeViewNode* ToTreeViewNode(TreeView* view);
    Control* CreateView(ImageList* imageList) override;
    void AddContentDirectory(ContentDirectory* contentDirectory);
    void AddContentFile(ContentFile* contentFile);
    int RuleCount() const override;
    Rule* GetRule(int index) const override;
    Rule* GetRule(const std::string& name) const override;
    Node* GetRuleContainerNode() override;
    void Clear();
    void AddNode(Node* node) override;
private:
    std::vector<std::unique_ptr<ContentDirectory>> contentDirectories;
    std::vector<std::unique_ptr<ContentFile>> contentFiles;
};

class ContentDirectory : public Node
{
public:
    ContentDirectory(const std::string& name_);
    std::string ImageName() const override { return "folder.closed.bitmap"; }
    std::string DirectoryPath() const override;
    void AddContentDirectory(ContentDirectory* contentDirectory);
    void AddContentFile(ContentFile* contentFile);
    Control* CreateView(ImageList* imageList) override;
    int RuleCount() const override;
    Rule* GetRule(int index) const override;
    Rule* GetRule(const std::string& name) const override;
    Node* GetRuleContainerNode() override;
    void Clear();
    void AddNode(Node* node) override;
    bool CanDisable() const { return true; }
    bool IsDisabled() const override { return !enabled; }
    void Enable() override { enabled = true; }
    void Disable() override { enabled = false; }
    bool CanOpen() const override;
    bool CanExclude() const override;
    bool CanInclude() const override;
private:
    std::vector<std::unique_ptr<ContentDirectory>> contentDirectories;
    std::vector<std::unique_ptr<ContentFile>> contentFiles;
    bool enabled;
};

class ContentFile : public Node
{
public:
    ContentFile(const std::string& name_);
    std::string ImageName() const override { return "file.bitmap"; }
    bool CanDisable() const { return true; }
    bool IsDisabled() const override { return !enabled; }
    void Enable() override { enabled = true; }
    void Disable() override { enabled = false; }
    bool CanOpen() const override { return false; } 
    void ExecuteDefaultAction() override { Parent()->Open(); }
    bool CanExclude() const override;
    bool CanInclude() const override;
private:
    bool enabled;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_CONTENT_INCLUDED
