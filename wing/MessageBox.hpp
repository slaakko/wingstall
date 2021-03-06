// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef WING_MESSAGE_BOX_INCLUDED
#define WING_MESSAGE_BOX_INCLUDED
#include <wing/WingApi.hpp>
#include <string>

#undef MessageBox

namespace wing {

class Control;

enum class MessageBoxResult : int
{
    ok = 1,
    cancel = 2,
    abort = 3,
    retry = 4,
    ignore = 5,
    yes = 6,
    no = 7,
    tryAgain = 10,
    continue_ = 11
};

class WING_API MessageBox
{
public:
    static MessageBoxResult Show(const std::string& message, const std::string& caption, Control* owner, int messageBoxType);
};

} // wing

#endif // WING_MESSAGE_BOX_INCLUDED
