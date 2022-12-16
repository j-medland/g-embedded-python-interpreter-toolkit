#include "gepit/gepit.hpp"

// error output implementations
MgErr writeInvalidSessionHandleErr(LVErrorClusterPtr errorPtr, std::string functionName)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::InvalidSessionHandle, functionName, "The session-handle supplied is invalid.");
}

MgErr writeInvalidPythonObjectRefErr(LVErrorClusterPtr errorPtr, std::string functionName)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::PythonObjectInvalid, functionName, "The Python Object Reference is invalid.");
}

MgErr writePythonExceptionErr(LVErrorClusterPtr errorPtr, std::string functionName, std::string what)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::PythonExceptionErr, functionName, what);
}

MgErr writeStdExceptionErr(LVErrorClusterPtr errorPtr, std::string functionName, std::string what)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::StdExceptionErr, functionName, what);
}

MgErr writeUnkownErr(LVErrorClusterPtr errorPtr, std::string functionName)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::UnknownErr, functionName, "");
}