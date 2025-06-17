// AUImusic, a music player based on AUI Framework
// Copyright (C) 2025 Alex2772
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// SPDX-License-Identifier: GPL-3.0

#include <AUI/Platform/Entry.h>

#if AUI_PLATFORM_LINUX
#include <AUI/Platform/linux/APlatformAbstractionOptions.h>
#endif

#include "MainWindow.h"

AUI_ENTRY {
#if AUI_PLATFORM_LINUX
    APlatformAbstractionOptions::set({
      .initializationOrder = {
        APlatformAbstractionOptions::Adwaita1{},
        APlatformAbstractionOptions::Gtk4{},
        APlatformAbstractionOptions::X11{},
      },
    });
#endif

    _new<MainWindow>()->show();
    return 0;
};