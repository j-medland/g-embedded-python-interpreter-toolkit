// LabVIEW <-> C++ interoperability
// implement some of the functionality usually found in labviewv.lib and extcode.h
// this allows us to link at runtime and avoids messing with statically linking
// with NI provided libraries at compile time

#pragma once
#include <numeric>
#include <string>
#include <type_traits> // used to define function types

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// LV NumericArrayResize Type Codes
#define LV_U8_TYPECODE 5
#define LV_U32_TYPECODE 7
#define LV_U64_TYPECODE 8
#define LV_FLOAT_TYPECODE 9
#define LV_DOUBLE_TYPECODE 10

typedef int32_t MgErr;

// define types with byte-packing specified
// cmake sizeof void used to determine bitness
#ifdef _32_BIT_ENV_
#pragma pack(push, 1)
#endif

typedef uint8_t LVBoolean;

#define LVBooleanTrue static_cast<LVBoolean>(1)
#define LVBooleanFalse static_cast<LVBoolean>(0)


// LabVIEW String Types
typedef struct {
    int32_t cnt;
    uint8_t str[1];
} LVStr, *LVStrPtr, **LVStrHandle;
    
// LabVIEW Array Template
template <unsigned ndims, typename datatype>
struct LVArray_t
{
    unsigned dims[ndims];
    uint8_t buffer[1];

    datatype* data(size_t byteOffset=0)
        {
    #ifndef _32_BIT_ENV_
            if (sizeof(datatype) < 8)
            {
                return reinterpret_cast<datatype*>(buffer + byteOffset);
            }
            return reinterpret_cast<datatype*>(buffer + 4 + byteOffset); // 8-byte aligned data
    #else
            datatype* p = reinterpret_cast<datatype*>(buffer + byteOffset);
            return p;
    #endif
        };
};

typedef struct{
    LVBoolean status;
    int32_t code;
    LVStrHandle source;
} LVErrorCluster, *LVErrorClusterPtr;


// reset packing
#ifdef _32_BIT_ENV_
#pragma pack(pop)
#endif


// setup to import some functions exported from LabVIEW.exe / RTE at runtime
MgErr LVNumericArrayResize(int32_t typeCode, int32_t numDims, void* handle, size_t size);
// define the numericArrayResize function pointer type
using numericArrayResizePtr = std::add_pointer<MgErr(int32_t, int32_t, void *handle, size_t size)>::type;
// create a static variable to store the pointer
static numericArrayResizePtr numericArrayResizeImp = nullptr;

// write a std::string to a LabVIEW String-Handle Pointer
MgErr writeStringToStringHandlePtr(LVStrHandle*,std::string);

// write to an LabVIEW Error
MgErr writeErrorToErrorClusterPtr(LVErrorClusterPtr, int, std::string, std::string);