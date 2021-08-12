// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_MANUAL_RESET_EVENT_INCLUDED
#define WING_MANUAL_RESET_EVENT_INCLUDED
#include <wing/WingApi.hpp>

namespace cmajor { namespace wing {

class WING_API ManualResetEvent
{
public:
    ManualResetEvent();
    ~ManualResetEvent();
    void Reset();
    void Set();
    void WaitFor();
private:
    void* handle;
};

} } // cmajor::wing

#endif // WING_MANUAL_RESET_EVENT_INCLUDED
