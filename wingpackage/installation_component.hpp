// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WINGSTALL_WINGPACKAGE_INSTALLATION_COMPONENT_INCLUDED
#define WINGSTALL_WINGPACKAGE_INSTALLATION_COMPONENT_INCLUDED
#include <wingpackage/component.hpp>
#include <wingpackage/directory.hpp>
#include <wingpackage/file.hpp>
#include <boost/uuid/uuid.hpp>

namespace wingstall { namespace wingpackage {

class InstallationComponent : public Component
{
public:
    InstallationComponent();
    void CreateInstallationInfo() override;
    void RemoveInstallationInfo() override;
private:
    std::string SoftwareKey(const boost::uuids::uuid& packageId) const;
};

} } // namespace wingstall::wingpackage

#endif // WINGSTALL_WINGPACKAGE_INSTALLATION_COMPONENT_INCLUDED
