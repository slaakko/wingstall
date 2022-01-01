// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <package_editor/error.hpp>

namespace wingstall { namespace package_editor {

std::string MakePackageXmlErrorMessage(const std::string& message, const std::string& filePath, const soulng::lexer::SourcePos& sourcePos)
{
    std::string errorMessage = message;
    errorMessage.append(" in file '").append(filePath).append("'").append(" line ").append(std::to_string(sourcePos.line)).append(", column ").append(std::to_string(sourcePos.col));
    return errorMessage;
}

PackageXMLException::PackageXMLException(const std::string& message_, const std::string& xmlFilePath_, sngxml::dom::Element* element) :
    std::runtime_error(MakePackageXmlErrorMessage(message_, xmlFilePath_, element->GetSourcePos())), 
    message(message_), xmlFilePath(xmlFilePath_), line(element->GetSourcePos().line), column(element->GetSourcePos().col)
{
}

} } // wingstall::package_editor
