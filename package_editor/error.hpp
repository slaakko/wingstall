// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_PACKAGE_EDITOR_ERROR_INCLUDED
#define WINGSTALL_PACKAGE_EDITOR_ERROR_INCLUDED
#include <sngxml/dom/Element.hpp>
#include <stdexcept>

namespace wingstall { namespace package_editor {

class PackageXMLException : public std::runtime_error
{
public:
    PackageXMLException(const std::string& message_, const std::string& xmlFilePath_, sngxml::dom::Element* element);
    const std::string& Message() const { return message; }
    const std::string& XMLFilePath() const { return xmlFilePath; }
    int Line() const { return line; }
    int Coumn() const { return column; }
private:
    std::string message;
    std::string xmlFilePath;
    int line;
    int column;
};

} } // wingstall::package_editor

#endif // WINGSTALL_PACKAGE_EDITOR_ERROR_INCLUDED
