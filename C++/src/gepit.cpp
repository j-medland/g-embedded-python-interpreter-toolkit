#include <algorithm>

#include "gepit/gepit.hpp"

inline MgErr writePythonExceptionErr(LVErrorClusterPtr, std::string, std::string);
inline MgErr writeInvalidPythonObjectRefErr(LVErrorClusterPtr errorPtr, std::string functionName);
inline MgErr writeStdExceptionErr(LVErrorClusterPtr, std::string, std::string);
inline MgErr writeInvalidSessionHandleErr(LVErrorClusterPtr, std::string);
inline MgErr writeUnkownErr(LVErrorClusterPtr, std::string);

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

int32_t read_session_attribute_as_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle attributeNameStrHandle, LVBoolean* found, LVStrHandlePtr valueStrHandlePtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        std::string attrName = lvStrHandleToStdString(attributeNameStrHandle);
        if(session->scope.contains(attrName)){
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

int32_t destroy_py_object(LVErrorClusterPtr errorPtr, SessionHandle session, LVRefNum* objectPtr){
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        session->dropObject(*objectPtr);
        *objectPtr = 0;
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

int32_t create_py_object_int(LVErrorClusterPtr errorPtr, SessionHandle session, int32_t value, LVRefNum* returnValuePtr){
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        *returnValuePtr = session->keepObject(pybind11::int_(value));
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

int32_t cast_py_object_to_int(LVErrorClusterPtr errorPtr, SessionHandle session, LVRefNum* objectPtr, int32_t* returnValuePtr){
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        if(session->isNullObject(objectPtr)){
            return writeInvalidPythonObjectRefErr(errorPtr, __func__);
        }
        *returnValuePtr = session->getObject(*objectPtr).cast<int32_t>();
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

int32_t call_function(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle fnNameStrHandle, LVArray_t<1,LVRefNum>** argArrayHandle, LVRefNum* returnValuePtr){
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        pybind11::object result = pybind11::none();

        // convert array of LVRefNums to vector of Python Objects
        size_t nargs = argArrayHandle && *argArrayHandle && (*argArrayHandle)->dims ? (*argArrayHandle)->dims[0] : 0;
        std::vector<pybind11::object> argObjects;
        argObjects.reserve(nargs);
        LVRefNum* refPtr = (*argArrayHandle)->data();
        std::generate_n(std::back_inserter(argObjects), nargs, [&] { return session->getObject(*refPtr++);});

        auto fn = session->scope[lvStrHandleToStdString(fnNameStrHandle).c_str()];

        switch (argObjects.size()){
            case 0:
                result = fn();
                break;
            case 1:
                result = fn(argObjects[0]);
                break;
            case 2:
                result = fn(argObjects[0],argObjects[1]);
                break;
            case 3:
                result = fn(argObjects[0],argObjects[1],argObjects[2]);
                break;
            case 4:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3]);
                break;
            case 5:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4]);
                break;
            case 6:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5]);
                break;
            case 7:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5], argObjects[6]);
                break;
            case 8:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5], argObjects[6], argObjects[7]);
                break;
            case 9:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5], argObjects[6], argObjects[7], argObjects[8]);
                break;
            case 10:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5], argObjects[6], argObjects[7], argObjects[8], argObjects[9]);
                break;
            case 11:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5], argObjects[6], argObjects[7], argObjects[8], argObjects[9], argObjects[10]);
                break;
            case 12:
                result = fn(argObjects[0],argObjects[1],argObjects[2],argObjects[3], argObjects[4], argObjects[5], argObjects[6], argObjects[7], argObjects[8], argObjects[9], argObjects[10], argObjects[11]);
                break;
            default:
                throw std::out_of_range("Number of arguments cannot exceed 12. Consider creating a python function which can use *args");
        }
        *returnValuePtr = session->keepObject(result);
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