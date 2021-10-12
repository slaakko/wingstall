// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_PREINSTALL_COMPONENT_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_PREINSTALL_COMPONENT_INCLUDED
#include <sngxml/dom/Element.hpp>

namespace wingstall { namespace package_editor {

class PreinstallComponent
{
public:
    PreinstallComponent(const std::string& packageXMLFilePath, sngxml::dom::Element* element);
    sngxml::dom::Element* ToXml() const;
private:
    std::vector<std::string> includeFilePaths;
    std::vector<std::string> runCommands;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_PREINSTALL_COMPONENT_INCLUDED
