// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_MAKE_SETUP_INCLUDED
#define WINGSTALL_WINGPACKAGE_MAKE_SETUP_INCLUDED
#include <string>

namespace wingstall { namespace wingpackage {

class Logger
{
public:
    virtual void WriteLine(const std::string& line) = 0;
};

class CoutLogger : public Logger
{
public:
    void WriteLine(const std::string& line) override;
};

void MakeSetup(const std::string& packageBinFilePath, bool verbose);
void MakeSetup(const std::string& packageBinFilePath, bool verbose, Logger* logger);

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_MAKE_SETUP_INCLUDED
