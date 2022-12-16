#include <algorithm>
#include <span>
#include <complex>

#include "gepit/gepit.hpp"

inline MgErr writePythonExceptionErr(LVErrorClusterPtr, std::string, std::string);
inline MgErr writeInvalidPythonObjectRefErr(LVErrorClusterPtr errorPtr, std::string functionName);
inline MgErr writeStdExceptionErr(LVErrorClusterPtr, std::string, std::string);
inline MgErr writeInvalidSessionHandleErr(LVErrorClusterPtr, std::string);
inline MgErr writeUnkownErr(LVErrorClusterPtr, std::string);
std::vector<pybind11::object> convertArgsToPythonObjects(SessionHandle session, LVArgumentClusterPtr argsPtr, LVArgumentTypeInfoHandle argTypesInfoHandle);

int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, LVBoolean *alreadyRunningPtr)
{
    try
    {
        // try starting the interpreter (it might already be running)
        pybind11::initialize_interpreter();
    }
    catch (std::runtime_error const &e)
    {
        //"The interpreter is already running"
        *alreadyRunningPtr = LVBooleanTrue;
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t finalize_interpreter(LVErrorClusterPtr errorPtr)
{
    try
    {
        pybind11::finalize_interpreter();
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t create_session(LVErrorClusterPtr errorPtr, SessionHandlePtr sessionPtr)
{
    try
    {
        *sessionPtr = new Session();
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t destroy_session(LVErrorClusterPtr errorPtr, SessionHandle session)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        delete (session);
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t evaluate_script(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle filePathStrHandle)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        pybind11::eval_file(lvStrHandleToStdString(filePathStrHandle), session->scope);
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t read_session_attribute_as_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle attributeNameStrHandle, LVBoolean *found, LVStrHandlePtr valueStrHandlePtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        std::string attrName = lvStrHandleToStdString(attributeNameStrHandle);
        if (session->scope.contains(attrName))
        {
            *found = LVBooleanTrue;
            auto value = session->scope[attrName.c_str()];
            return writeStringToStringHandlePtr(valueStrHandlePtr, pybind11::str(value));
        }
        *found = LVBooleanFalse;
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t destroy_py_object(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        session->dropObject(object);
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
};

int32_t create_py_object_int(LVErrorClusterPtr errorPtr, SessionHandle session, int32_t value, LVPythonObjRef *returnObjectPtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        *returnObjectPtr = session->keepObject(pybind11::int_(value));
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t cast_py_object_to_int(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, int32_t *returnValuePtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        if (session->isNullObject(object))
        {
            return writeInvalidPythonObjectRefErr(errorPtr, __func__);
        }
        *returnValuePtr = session->getObject(object).cast<int32_t>();
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t cast_py_object_to_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, LVStrHandlePtr strHandlePtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        if (session->isNullObject(object))
        {
            return writeInvalidPythonObjectRefErr(errorPtr, __func__);
        }
        writeStringToStringHandlePtr(strHandlePtr, session->getObject(object).cast<std::string>());
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t py_object_print_to_str(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, LVStrHandlePtr strHandlePtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        if (session->isNullObject(object))
        {
            return writeInvalidPythonObjectRefErr(errorPtr, __func__);
        }
        writeStringToStringHandlePtr(strHandlePtr, pybind11::str(session->getObject(object)));
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

GEPIT_EXPORT int32_t call_function(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef classInstance, LVStrHandle fnNameStrHandle, LVArgumentClusterPtr argsPtr, LVArgumentTypeInfoHandle argTypesInfoHandle, LVPythonObjRef *returnObjectPtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        pybind11::object result = pybind11::none();

        // convert array of LVRefNums to vector of Python Objects
        auto argObjects = convertArgsToPythonObjects(session, argsPtr, argTypesInfoHandle);

        // Get a Ref to the Function in the session scope or the class
        std::string fnNameString = lvStrHandleToStdString(fnNameStrHandle);
        pybind11::handle fnHandle;

        if (session->isNullObject(classInstance))
        {
            // handle is an item of the scope dict
            fnHandle = session->scope[fnNameString.c_str()];
        }
        else
        {
            // handle is an attribute of a class
            fnHandle = session->getObject(classInstance).attr(fnNameString.c_str());
        }

        // C++ needs the argument expanded function calls to exist at compile time
        // hence this switch and a physical limit on the number of args
        switch (argObjects.size())
        {
        case 0:
            result = fnHandle();
            break;
        case 1:
            result = function_call_with_args_vector<1>(fnHandle, argObjects);
            break;
        case 2:
            result = function_call_with_args_vector<2>(fnHandle, argObjects);
            break;
        case 3:
            result = function_call_with_args_vector<3>(fnHandle, argObjects);
            break;
        case 4:
            result = function_call_with_args_vector<4>(fnHandle, argObjects);
            break;
        case 5:
            result = function_call_with_args_vector<5>(fnHandle, argObjects);
            break;
        case 6:
            result = function_call_with_args_vector<6>(fnHandle, argObjects);
            break;
        case 7:
            result = function_call_with_args_vector<7>(fnHandle, argObjects);
            break;
        case 8:
            result = function_call_with_args_vector<8>(fnHandle, argObjects);
            break;
        case 9:
            result = function_call_with_args_vector<9>(fnHandle, argObjects);
            break;
        case 10:
            result = function_call_with_args_vector<10>(fnHandle, argObjects);
            break;
        case 11:
            result = function_call_with_args_vector<11>(fnHandle, argObjects);
            break;
        case 12:
            result = function_call_with_args_vector<12>(fnHandle, argObjects);
            break;
        default:
            throw std::out_of_range("Number of arguments cannot exceed 12. Consider creating a python function which can use *args");
        }
        *returnObjectPtr = session->keepObject(result);
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

int32_t scope_as_str(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandlePtr handle)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        writeStringToStringHandlePtr(handle, pybind11::str(session->scope));
    }
    catch (pybind11::error_already_set const &e)
    {
        return writePythonExceptionErr(errorPtr, __func__, e.what());
    }
    catch (std::exception const &e)
    {
        return writeStdExceptionErr(errorPtr, __func__, e.what());
    }
    catch (...)
    {
        return writeUnkownErr(errorPtr, __func__);
    }
    return 0;
}

// error output implementations
inline MgErr writeInvalidSessionHandleErr(LVErrorClusterPtr errorPtr, std::string functionName)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::InvalidSessionHandle, functionName, "The session-handle supplied is invalid.");
}

inline MgErr writeInvalidPythonObjectRefErr(LVErrorClusterPtr errorPtr, std::string functionName)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::PythonObjectInvalid, functionName, "The Python Object Reference is invalid.");
}

inline MgErr writePythonExceptionErr(LVErrorClusterPtr errorPtr, std::string functionName, std::string what)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::PythonExceptionErr, functionName, what);
}

inline MgErr writeStdExceptionErr(LVErrorClusterPtr errorPtr, std::string functionName, std::string what)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::StdExceptionErr, functionName, what);
}

inline MgErr writeUnkownErr(LVErrorClusterPtr errorPtr, std::string functionName)
{
    return writeErrorToErrorClusterPtr(errorPtr, errorCodes::UnknownErr, functionName, "");
}

std::vector<pybind11::object> convertArgsToPythonObjects(SessionHandle session, LVArgumentClusterPtr argsPtr, LVArgumentTypeInfoHandle argTypesInfoHandle)
{
    size_t nargs = argTypesInfoHandle && (*argTypesInfoHandle) && (*argTypesInfoHandle)->dims ? (*argTypesInfoHandle)->dims[0] : 0;
    std::vector<pybind11::object> argObjects;
    argObjects.reserve(nargs);

    auto argsTypesInfoSpan = std::span{(*argTypesInfoHandle)->data(), nargs};
    auto argsSpan = std::span{argsPtr, nargs};

    auto argIter = argsSpan.begin();

    for (const auto &typeInfo : argsTypesInfoSpan)
    {
        switch (typeInfo.type)
        {
        case LVNumericType::I8_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<int8_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::I16_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<int16_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::I32_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<int32_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::I64_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<int64_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::U8_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<uint8_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::U16_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<uint16_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::U32_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<uint32_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::U64_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<uint64_t>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::SGL_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<float>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::DBL_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<double>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::EXT_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<long double>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::CSG_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<std::complex<float>>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::CDB_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<std::complex<double>>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::CXT_ARRAY:
            argObjects.push_back(cast_untyped_LVArrayHandle_to_numpy_array<std::complex<long double>>(*argIter, typeInfo.ndims));
            break;

        case LVNumericType::PYOBJ:
            if (session->isNullObject(*argIter))
            {
                throw std::out_of_range("Null or Invalid Python Object supplied as a Function Argument.");
            }
            argObjects.push_back(session->getObject(*argIter));
            break;

        default:
            throw std::out_of_range("Non-supported type supplied as a Function Argument.");
        }
    }
    return argObjects;
}