#include "gepit/lv-interop.hpp"

MgErr LVNumericArrayResize(int32_t typeCode, int32_t numDims, void *handle, size_t size)
{
    if (numericArrayResizeImp == nullptr)
    {
        // import function
        auto lvModule = GetModuleHandle("LabVIEW.exe");
        if (lvModule == nullptr)
        {
            lvModule = GetModuleHandle("lvffrt.dll");
        }
        if (lvModule == nullptr)
        {
            lvModule = GetModuleHandle("lvrt.dll");
        }
        numericArrayResizeImp = (numericArrayResizePtr)GetProcAddress(lvModule, "NumericArrayResize");
    }

    return numericArrayResizeImp(typeCode, numDims, handle, size);
}

MgErr writeStringToStringHandlePtr(LVStrHandle *handlePtr, std::string s)
{
    size_t currentSize = (*handlePtr) && (**handlePtr) ? (**handlePtr)->cnt : 0;
    if (s.length() > currentSize)
    {
        auto result = LVNumericArrayResize(LV_U8_TYPECODE, 1, handlePtr, s.length());
        if (result != 0)
        {
            return result;
        }
    }
    std::memcpy((**handlePtr)->str, s.c_str(), s.length());
    (**handlePtr)->cnt = s.length();

    return 0;
}

// write to an LabVIEW Error
MgErr writeErrorToErrorClusterPtr(LVErrorClusterPtr errorPtr, int32_t code, std::string func, std::string message)
{
    // set status and code
    errorPtr->status = code != 0 ? LVBooleanTrue : LVBooleanFalse;
    errorPtr->code = code;

    // build outputString
    std::string outputString = "" + func + "()" + (message.empty() ? "" : ("\n<ERR>" + message));

    // write outputString to error source string handle
    return writeStringToStringHandlePtr(&(errorPtr->source), outputString);
}


std::string lvStrHandleToStdString(LVStrHandle handle){
    size_t length = handle && (*handle) ? (*handle)->cnt : 0;
    return std::string(reinterpret_cast<char*>((*handle)->str), length);
}