// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Component.hpp>

namespace wing {

Component::Component() : container(nullptr), nextSibling(nullptr), prevSibling(nullptr)
{
}

Component::~Component()
{
}

} // wing
