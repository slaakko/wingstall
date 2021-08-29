// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/directory.hpp>
#include <package_editor/error.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

Directory::Directory() : Node(NodeKind::directory, std::string())
{
    rules.reset(new Rules());
    rules->SetParent(this);
    content.reset(new Content());
    content->SetParent(this);
}

Directory::Directory(const std::string& packageXMLFilePath, sngxml::dom::Element* element) : Node(NodeKind::directory, std::string())
{
    rules.reset(new Rules());
    rules->SetParent(this);
    content.reset(new Content());
    content->SetParent(this);
    std::u32string nameAttr = element->GetAttribute(U"name");
    if (!nameAttr.empty())
    {
        SetName(ToUtf8(nameAttr));
    }
    else
    {
        throw PackageXMLException("'directory' element has no 'name' attribute", packageXMLFilePath, element);
    }
    std::unique_ptr<sngxml::xpath::XPathObject> ruleObject = sngxml::xpath::Evaluate(U"*", element);
    if (ruleObject)
    {
        if (ruleObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(ruleObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    Rule* rule = new Rule(packageXMLFilePath, element);
                    rules->AddRule(rule);
                }
            }
        }
    }
}

TreeViewNode* Directory::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode(Name());
    SetTreeViewNode(node);
    node->SetData(this);
    node->SetImageIndex(view->GetImageIndex("folder.closed.bitmap"));
    node->AddChild(rules->ToTreeViewNode(view));
    node->AddChild(content->ToTreeViewNode(view));
    return node;
}

Content::Content() : Node(NodeKind::content, std::string())
{
}

TreeViewNode* Content::ToTreeViewNode(TreeView* view)
{
    TreeViewNode* node = new TreeViewNode("Content");
    SetTreeViewNode(node);
    node->SetData(this);
    node->SetImageIndex(view->GetImageIndex("document.collection.bitmap"));
    return node;
}

} } // wingstall::package_editor
