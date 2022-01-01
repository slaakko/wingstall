// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_UNINSTALL_EXE_FILE_INCLUDED
#define WINGSTALL_WINGPACKAGE_UNINSTALL_EXE_FILE_INCLUDED
#include <wingpackage/file.hpp>

namespace wingstall { namespace wingpackage {

class UninstallExeFile : public File
{
public:
    UninstallExeFile();
    void SetInfo();
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    void Uninstall() override;
private:
    bool Rename();
    void ScheduleToBeRemoved();
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_UNINSTALL_EXE_FILE_INCLUDED
