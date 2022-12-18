#pragma once

#include <map>
#include <mutex>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>

#include "gepit/lv-interop.hpp"
#include "gepit_export.h"

enum errorCodes : int32_t
{
    NoErr = 0,
    UnknownErr = -1,
    PythonExceptionErr = -2,
    StdExceptionErr = -3,
    InvalidSessionHandle = -4,
    PythonObjectInvalid = -5
};

// C++ Object to be passed back to LabVIEW between DLL calls
class Session
{
private:
    std::map<int32_t, pybind11::object> objStore;
    uint32_t objStoreNextKey;
    std::mutex objStoreMutex;

public:
    const pybind11::dict scope;
    Session() : scope(pybind11::module::import("__main__").attr("__dict__")), objStoreNextKey(1) // start at non-zero-value
    {
        // nothing else to construct
    }
    uint32_t keepObject(pybind11::object obj)
    {
        const std::lock_guard lock(objStoreMutex);
        objStore[objStoreNextKey] = obj;
        return objStoreNextKey++;
    }
    pybind11::object getObject(uint32_t key)
    {
        return objStore.at(key);
    }
    void dropObject(uint32_t key)
    {
        const std::lock_guard lock(objStoreMutex);
        objStore.erase(key);
    }
    bool isNullObject(uint32_t key)
    {
        const std::lock_guard lock(objStoreMutex);
        return !(key > 0 && objStore.count(key));
    }
};

typedef Session *SessionHandle, **SessionHandlePtr;

enum LVNumericType : uint8_t
{
    I8_ARRAY = 2,
    I16_ARRAY = 3,
    I32_ARRAY = 4,
    I64_ARRAY = 5,
    U8_ARRAY = 6,
    U16_ARRAY = 7,
    U32_ARRAY = 8,
    U64_ARRAY = 9,
    SGL_ARRAY = 10,
    DBL_ARRAY = 11,
    EXT_ARRAY = 12,
    CSG_ARRAY = 13, // not used
    CDB_ARRAY = 14, // not used
    CXT_ARRAY = 15, // not used
    PYOBJ = 40
};

enum ImaqImageDataTypes : uint32_t {
    Grayscale_U8 = 0,
    Grayscale_I16 = 1,
    Grayscale_SGL = 2,
    Complex_CSG = 3,
    RGB_U32 = 4,
    HSL_U32 = 5,
    RGB_U64 = 6,
    Grayscale_U16 = 7
};

// specify void size
#ifdef _32_BIT_ENV_
typedef uint32_t LVVoid_t;
#else
typedef uint64_t LVVoid_t
#endif

// define bitness dependent value types
typedef LVVoid_t LVPythonObjRef;
typedef LVVoid_t *LVArgumentClusterPtr;

// set packing for LabVIEW Types
#ifdef _32_BIT_ENV_
#pragma pack(push, 1)
#endif

typedef struct
{
    LVNumericType type;
    uint8_t ndims;
} LVTypeInfo;

typedef LVArray_t<1, LVTypeInfo> **LVArgumentTypeInfoHandle;

typedef struct{
    uint64_t pixelPointer;
    int32_t lineWidth, width, height;
    ImaqImageDataTypes type;
} LVIMAQImage, *LVIMAQImagePtr;

// reset packing
#ifdef _32_BIT_ENV_
#pragma pack(pop)
#endif

extern "C"
{
    GEPIT_EXPORT int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, LVBoolean *alreadyRunningPtr);
    GEPIT_EXPORT int32_t finalize_interpreter(LVErrorClusterPtr errorPtr);
    GEPIT_EXPORT int32_t create_session(LVErrorClusterPtr errorPtr, SessionHandlePtr sessionPtr);
    GEPIT_EXPORT int32_t destroy_session(LVErrorClusterPtr errorPtr, SessionHandle session);
    GEPIT_EXPORT int32_t evaluate_script(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle filePathStrHandle);
    GEPIT_EXPORT int32_t read_session_attribute_as_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle attributeNameStrHandle, LVBoolean *found, LVStrHandlePtr valueStrHandlePtr);
    GEPIT_EXPORT int32_t destroy_py_object(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object);
    GEPIT_EXPORT int32_t create_py_object_int(LVErrorClusterPtr errorPtr, SessionHandle session, int32_t value, LVPythonObjRef *returnObjectPtr);
    GEPIT_EXPORT int32_t create_py_object_IMAQ(LVErrorClusterPtr errorPtr, SessionHandle session, LVIMAQImagePtr imaqImagePtr, LVPythonObjRef *returnObjectPtr);
    GEPIT_EXPORT int32_t cast_py_object_to_int(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, int32_t *returnValuePtr);
    GEPIT_EXPORT int32_t cast_py_object_to_dbl(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, double *returnValuePtr);
    GEPIT_EXPORT int32_t call_function(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef classInstance, LVStrHandle fnNameStrHandle, LVArgumentClusterPtr argsPtr, LVArgumentTypeInfoHandle argTypesInfoHandle, LVPythonObjRef *returnObjectPtr);
    GEPIT_EXPORT int32_t scope_as_str(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandlePtr handle);
    GEPIT_EXPORT int32_t cast_py_object_to_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, LVStrHandlePtr strHandlePtr);
    GEPIT_EXPORT int32_t py_object_print_to_str(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, LVStrHandlePtr strHandlePtr);
}

// utility functions
MgErr writePythonExceptionErr(LVErrorClusterPtr, std::string, std::string);
MgErr writeInvalidPythonObjectRefErr(LVErrorClusterPtr errorPtr, std::string functionName);
MgErr writeStdExceptionErr(LVErrorClusterPtr, std::string, std::string);
MgErr writeInvalidSessionHandleErr(LVErrorClusterPtr, std::string);
MgErr writeUnkownErr(LVErrorClusterPtr, std::string);