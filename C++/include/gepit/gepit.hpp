#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

#include "gepit/lv-interop.hpp"
#include "gepit_export.h"

enum errorCodes : int32_t
{
    NoErr = 0,
    UnknownErr = -1,
    PythonExceptionErr = -2,
    StdExceptionErr = -3,
    InvalidSessionHandle = -4
};

// C++ Object to be passed back to LabVIEW between DLL calls
struct Session
{
    const pybind11::dict scope;
    Session() : scope(pybind11::module::import("__main__").attr("__dict__"))
    {
        // nothing else to construct
    }
};

typedef Session *SessionHandle, **SessionHandlePtr;

extern "C"{
    GEPIT_EXPORT int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, LVBoolean *alreadyRunningPtr);
    GEPIT_EXPORT int32_t finalize_interpreter(LVErrorClusterPtr errorPtr);
    GEPIT_EXPORT int32_t create_session(LVErrorClusterPtr errorPtr, SessionHandlePtr sessionPtr);
    GEPIT_EXPORT int32_t destroy_session(LVErrorClusterPtr errorPtr, SessionHandle session);
    GEPIT_EXPORT int32_t evaluate_script(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle filePathStrHandle);
}