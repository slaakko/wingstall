// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_UNINSTALL_COMPONENT_INCLUDED
#define WINGSTALL_WINGPACKAGE_UNINSTALL_COMPONENT_INCLUDED
#include <wingpackage/component.hpp>

namespace wingstall { namespace wingpackage {

class UninstallComponent : public Component
{
public:
    UninstallComponent();
    void Initialize();
    void AddFile(File* file);
    void WriteIndex(BinaryStreamWriter& writer) override;
    void ReadIndex(BinaryStreamReader& reader) override;
    void WriteData(BinaryStreamWriter& writer) override;
    void ReadData(BinaryStreamReader& reader) override;
    void Uninstall() override;
private:
    std::vector<std::unique_ptr<File>> files;
    void ScheduleRootDirToBeRemoved();
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_UNINSTALL_COMPONENT_INCLUDED
