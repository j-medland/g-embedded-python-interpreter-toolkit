#include <gepit/gepit.hpp>

Session::Session() : scope(pybind11::module::import("__main__").attr("__dict__")),
                     objStoreNextKey(1) // start at non-zero value
{
    // nothing to construct
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
        pybind11::gil_scoped_acquire gil;
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
        delete session;
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