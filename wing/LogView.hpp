// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_LOG_VIEW_INCLUDED
#define WING_LOG_VIEW_INCLUDED
#include <wing/TextView.hpp>

namespace cmajor { namespace wing {

class WING_API LogView : public TextView
{
public:
    LogView(TextViewCreateParams& createParams);
    virtual void WriteLine(const std::string& text);
};

} } // cmajor::wing

#endif // WING_LOG_VIEW_INCLUDED
