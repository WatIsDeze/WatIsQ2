#include <RmlUi/Core.h>

#include "../shared/shared.h"
#include "../common/common.h"

#include "SystemInterface.h"

// Define where to output log messages.
static const comPrint_t logOutputTarget = eComPrint_t::PRNT_DEFAULT;

/*
==================
RmlUi_SystemInterface::RmlUi_SystemInterface

Constructor.
==================
*/
RmlUi_SystemInterface::RmlUi_SystemInterface() {

}

/*
==================
RmlUi_SystemInterface::RmlUi_SystemInterface

Destructor.
==================
*/
RmlUi_SystemInterface::~RmlUi_SystemInterface() {
    
}

/*
==================
RmlUi_SystemInterface::GetElapsedTime

Calls Sys_Milliseconds and returns its value.
==================
*/
double RmlUi_SystemInterface::GetElapsedTime() 
{
    // Divide by 1000, assuming this gives the same results as SDL_GetTicks() does.
    return static_cast<double>(Sys_Milliseconds()) / 1000.0;
}

/*
==================
RmlUi_SystemInterface::GetElapsedTime

Log RmlUi messages with Com_Printf as PRNT_WARNING or PRNT_DEFAULT.
==================
*/
bool RmlUi_SystemInterface::LogMessage(Rml::Log::Type type, const Rml::String& message) {
    // Default outputTarget.
    comPrint_t outputTarget = logOutputTarget;

    // Empty output string.
    std::string outputString = "";

    switch(type) {
        case Rml::Log::Type::LT_ALWAYS:
                outputString = "[RmlUi - Always]: ";
            break;
        case Rml::Log::Type::LT_ASSERT:
                outputTarget = eComPrint_t::PRNT_WARNING;
                outputString = "[RmlUi - Assert]: ";
            break;
        case Rml::Log::Type::LT_DEBUG:
                outputString = "[RmlUi - Debug]: ";
            break;
        case Rml::Log::Type::LT_ERROR:
                outputTarget = eComPrint_t::PRNT_WARNING;
                outputString = "[RmlUi - Error]: ";
            break;
        case Rml::Log::Type::LT_INFO:
                outputString = "[RmlUi - Info]: ";
            break;
        case Rml::Log::Type::LT_WARNING:
                outputTarget = eComPrint_t::PRNT_WARNING;
                outputString = "[RmlUi - Warning]: ";
            break;
        default:
            outputTarget = eComPrint_t::PRNT_WARNING;
            outputString = "[RmlUi - UNKNOWN]: Unknown log type";
            break;
    }

    // .c_str to ensure it will still compile if RmlUi wasn't built with std as its default lib.
    outputString += message.c_str(); // (Or so, I assume )

    // Last but not least, output our message.
    Com_Printf(outputTarget, "%s", outputString.c_str());

    return true;
}