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
    CSG_ARRAY = 13,
    CDB_ARRAY = 14,
    CXT_ARRAY = 15,
    BOOLEAN_ARRAY = 26,
    PYOBJ = 40
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
    GEPIT_EXPORT int32_t cast_py_object_to_int(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, int32_t *returnValuePtr);
    GEPIT_EXPORT int32_t call_function(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef classInstance, LVStrHandle fnNameStrHandle, LVArgumentClusterPtr argsPtr, LVArgumentTypeInfoHandle argTypesInfoHandle, LVPythonObjRef *returnObjectPtr);
    GEPIT_EXPORT int32_t scope_as_str(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandlePtr handle);
    GEPIT_EXPORT int32_t cast_py_object_to_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, LVStrHandlePtr strHandlePtr);
    GEPIT_EXPORT int32_t py_object_print_to_str(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, LVStrHandlePtr strHandlePtr);
}

// template functions

// create pybind11 format string
template <typename T>
std::string create_format_descriptor()
{
    return pybind11::format_descriptor<T>::format();
}

// create pybind11 dType
template <typename T>
pybind11::dtype create_dtype()
{
    return pybind11::dtype(create_format_descriptor<T>());
}

// create pybind11 array
template <typename T>
pybind11::array cast_untyped_LVArrayHandle_to_numpy_array(LVVoid_t handle, size_t ndims)
{

    int32_t *dimsPtr = nullptr;
    T *buffer = nullptr;

    switch (ndims)
    {
    case 1:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<1, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 2:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<2, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 3:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<3, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 4:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<4, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 5:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<5, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 6:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<6, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 7:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<7, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    case 8:
    {
        auto typedHandle = reinterpret_cast<LVArray_t<8, T> **>(handle);
        dimsPtr = (*typedHandle)->dims;
        buffer = (*typedHandle)->data();
    }
        break;
    default:
        throw std::out_of_range("Array exceeds 8 dimensions");
    }

    // get dims as pybind11::ssize_t vector
    std::vector<pybind11::ssize_t> shape;
    shape.reserve(ndims);

    for (const auto &d : std::span<int32_t>(dimsPtr, ndims))
    {
        shape.push_back(pybind11::ssize_t_cast(d));
    }

    // build strides pybind11::ssize_t vector
    std::vector<pybind11::ssize_t> strides(ndims);

    // reverse iterate over strides and shape
    auto shapeIter = shape.rbegin();
    for (auto it = strides.rbegin(); it != strides.rend(); it++)
    {
        if (it == strides.rbegin())
        {
            // last element
            *it = pybind11::ssize_t_cast(sizeof(T));
            continue;
        }
        // other values: take the previous value of strides
        // and multiply by the current value in the shapeIter
        *it = *shapeIter * (*(it - 1));
        shapeIter++;
    }

    return pybind11::array(create_dtype<T>(), shape, strides, buffer);
}