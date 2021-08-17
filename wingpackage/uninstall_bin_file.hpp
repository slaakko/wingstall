// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_UNINSTALL_BIN_FILE_INCLUDED
#define WINGSTALL_WINGPACKAGE_UNINSTALL_BIN_FILE_INCLUDED
#include <wingpackage/file.hpp>

namespace wingstall { namespace wingpackage {

class UninstallBinFile : public File
{
public:
    UninstallBinFile();
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    void Uninstall() override;
private:
    void Remove();
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_UNINSTALL_BIN_FILE_INCLUDED
