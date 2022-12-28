#include <gepit/gepit.hpp>

// Session Constructor
// Pybind11 provides a nice global-interpreter-lock (GIL) API with scoped-guard type 
// lock-obtain/lock-release functionality. To be able to make shared-library calls
// from "any-thread" in LabVIEW we are going to store a scoped release in the session object
// to force the GIL release when the interpreter is initilaized. "disarming" the scoped_release
// immediately after obtaining it leaves it dangling in memory and causes more problems so this
// appears to be the best solution.
// This approach would appear to defeat the point of the scoped-guard but doing it this way allows
// pybind11::gil_scoped_acquire calls without getting a deadlock or having to use the basic Python 
// provided implementation which has its own quirks or issues.

Session::Session() : 
    scope(pybind11::module::import("__main__").attr("__dict__")), 
    objStoreNextKey(1)
{
    // create scoped_release here (not in initializer list)
    gil_scoped_release = std::make_unique<pybind11::gil_scoped_release>();
}

uint32_t Session::keepObject(pybind11::object obj)
{
    const std::lock_guard lock(objStoreMutex);
    objStore[objStoreNextKey] = obj;
    return objStoreNextKey++;
}
pybind11::object Session::getObject(uint32_t key)
{
    return objStore.at(key);
}
void Session::dropObject(uint32_t key)
{
    const std::lock_guard lock(objStoreMutex);
    objStore.erase(key);
}
bool Session::isNullObject(uint32_t key)
{
    const std::lock_guard lock(objStoreMutex);
    return !(key > 0 && objStore.count(key));
}

int32_t create_session(LVErrorClusterPtr errorPtr, SessionHandlePtr sessionPtr)
{ 
    try
    {
        // try starting the interpreter (it might already be running)
        pybind11::initialize_interpreter();
    }
    catch (std::runtime_error const &e)
    {
        // already running
    }
    try{
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