// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/uninstall_component.hpp>
#include <package_editor/error.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

UninstallComponent::UninstallComponent(const std::string& packageXMLFilePath, sngxml::dom::Element* element)
{
    std::unique_ptr<sngxml::xpath::XPathObject> runObject = sngxml::xpath::Evaluate(U"run", element);
    if (runObject)
    {
        if (runObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(runObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string commandAttr = element->GetAttribute(U"command");
                    if (!commandAttr.empty())
                    {
                        runCommands.push_back(ToUtf8(commandAttr));
                    }
                    else
                    {
                        throw PackageXMLException("uninstall/run' element does not contain a 'command' attribute", packageXMLFilePath, element);
                    }
                }
            }
        }
    }
}

sngxml::dom::Element* UninstallComponent::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"uninstall");
    for (const std::string& runCommand : runCommands)
    {
        sngxml::dom::Element* runElement = new sngxml::dom::Element(U"run");
        runElement->SetAttribute(U"command", ToUtf32(runCommand));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(runElement));
    }
    return element;
}

} } // wingstall::package_editor
