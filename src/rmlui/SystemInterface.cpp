#include <RmlUi/Core.h>

#include "../shared/shared.h"
#include "../common/common.h"

#include "SystemInterface.h"

double WIQ2RmlUi_SystemInterface::GetElapsedTime() {
    return Sys_Milliseconds();
}