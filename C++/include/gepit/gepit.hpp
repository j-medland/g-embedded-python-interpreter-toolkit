#include <map>
#include <mutex>

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
    uint32_t keepObject(pybind11::object obj){
        const std::lock_guard lock(objStoreMutex);
        objStore[objStoreNextKey] = obj;
        return objStoreNextKey++;
    }
    pybind11::object getObject(uint32_t key){
        return objStore.at(key);
    }
    void dropObject(uint32_t key){
        const std::lock_guard lock(objStoreMutex);
        objStore.erase(key);
    }
    bool isNullObject(uint32_t key){
        const std::lock_guard lock(objStoreMutex);
        return !(key>0 && objStore.count(key));
    }
    bool isNullObject(uint32_t* keyPtr){
        return isNullObject(*keyPtr);
    }
};

typedef Session *SessionHandle, **SessionHandlePtr;

extern "C"{
    GEPIT_EXPORT int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, LVBoolean *alreadyRunningPtr);
    GEPIT_EXPORT int32_t finalize_interpreter(LVErrorClusterPtr errorPtr);
    GEPIT_EXPORT int32_t create_session(LVErrorClusterPtr errorPtr, SessionHandlePtr sessionPtr);
    GEPIT_EXPORT int32_t destroy_session(LVErrorClusterPtr errorPtr, SessionHandle session);
    GEPIT_EXPORT int32_t evaluate_script(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle filePathStrHandle);
    GEPIT_EXPORT int32_t read_session_attribute_as_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle attributeNameStrHandle, LVBoolean* found, LVStrHandlePtr valueStrHandlePtr);
    GEPIT_EXPORT int32_t destroy_py_object(LVErrorClusterPtr errorPtr, SessionHandle session, LVRefNum* objectPtr);
    GEPIT_EXPORT int32_t create_py_object_int(LVErrorClusterPtr errorPtr, SessionHandle session, int32_t value, LVRefNum* returnValuePtr);
    GEPIT_EXPORT int32_t cast_py_object_to_int(LVErrorClusterPtr errorPtr, SessionHandle session, LVRefNum* objectPtr, int32_t* returnValuePtr);
    GEPIT_EXPORT int32_t call_function(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle fnNameStrHandle, LVArray_t<1,LVRefNum>** argArrayHandle, LVRefNum* returnValuePtr);
}