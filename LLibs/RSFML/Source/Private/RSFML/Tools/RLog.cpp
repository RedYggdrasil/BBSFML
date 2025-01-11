#include "RSFML/Tools/RLog.h"
#include <iostream>

static const std::string ESEVERITY_DISPLAY = "Display";
static const std::string ESEVERITY_WARNING = "Warning";
static const std::string ESEVERITY_ERROR = "Error";
static const std::string ESEVERITY_EXCEPTION = "Exception";
static const std::string ESEVERITY_UNKNOWN = "Unknown";

using std::cin, std::cout, std::endl;

void RLog::Log(ESeverity EInSeverity, const std::string_view InDisplayError)
{
    cout << "[" << ToString(EInSeverity) << "] : " << InDisplayError << endl;
}

const std::string& ToString(RLog::ESeverity EInSeverity)
{
    switch (EInSeverity)
    {
    case RLog::ESeverity::Display:
        return ESEVERITY_DISPLAY;
    case RLog::ESeverity::Warning:
        return ESEVERITY_WARNING;
    case RLog::ESeverity::Error:
        return ESEVERITY_ERROR;
    case RLog::ESeverity::Exception:
        return ESEVERITY_EXCEPTION;
    default:
        cout << "[UnimplExcep] Case not handled in switch of type 'RLog::ESeverity' in RLog.cpp" << endl;
        return ESEVERITY_UNKNOWN;
    }
}
