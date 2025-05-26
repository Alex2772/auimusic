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