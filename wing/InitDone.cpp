// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/InitDone.hpp>
#include <wing/Metrics.hpp>
#include <wing/Wing.hpp>
#include <wing/Application.hpp>
#include <wing/Shell.hpp>

namespace wing {

void Init(HINSTANCE instance)
{
    WingInit(instance);
    ApplicationInit();
    LoadMetrics();
    ShellInit();
}

void Done()
{
    ShellDone();
    ApplicationDone();
    WingDone();
}

} // wing