// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/preinstall_component.hpp>
#include <package_editor/error.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>

namespace wingstall { namespace package_editor {

using namespace soulng::unicode;

PreinstallComponent::PreinstallComponent(const std::string& packageXMLFilePath, sngxml::dom::Element* element)
{
    std::unique_ptr<sngxml::xpath::XPathObject> includeObject = sngxml::xpath::Evaluate(U"include", element);
    if (includeObject)
    {
        if (includeObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(includeObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string filePathAttr = element->GetAttribute(U"filePath");
                    if (!filePathAttr.empty())
                    {
                        includeFilePaths.push_back(ToUtf8(filePathAttr));
                    }
                    else
                    {
                        throw PackageXMLException("preinstall/include' element does not contain a 'filePath' attribute", packageXMLFilePath, element);
                    }
                }
            }
        }
    }
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
                        throw PackageXMLException("preinstall/run' element does not contain a 'command' attribute", packageXMLFilePath, element);
                    }
                }
            }
        }
    }
}

sngxml::dom::Element* PreinstallComponent::ToXml() const
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"preinstall");
    for (const std::string& includeFilePath : includeFilePaths)
    {
        sngxml::dom::Element* includeElement = new sngxml::dom::Element(U"include");
        includeElement->SetAttribute(U"filePath", ToUtf32(includeFilePath));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(includeElement));
    }
    for (const std::string& runCommand : runCommands)
    {
        sngxml::dom::Element* runElement = new sngxml::dom::Element(U"run");
        runElement->SetAttribute(U"command", ToUtf32(runCommand));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(runElement));
    }
    return element;
}

} } // wingstall::package_editor
