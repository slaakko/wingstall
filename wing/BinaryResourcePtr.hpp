// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_BINARY_RESOURCE_PTR_INCLUDED
#define WING_BINARY_RESOURCE_PTR_INCLUDED
#include <wing/WingApi.hpp>
#include <string>
#include <stdint.h>

namespace wing {

class WING_API BinaryResourcePtr
{
public:
    BinaryResourcePtr(const std::string& moduleName, const std::string& resourceName_);
    const std::string& ResourceName() const { return resourceName; }
    uint8_t* Data() const { return data; }
    int64_t Size() const { return size; }
private:
    std::string resourceName;
    uint8_t* data;
    int64_t size;
};

} // wing

#endif // WING_BINARY_RESOURCE_PTR_INCLUDED
