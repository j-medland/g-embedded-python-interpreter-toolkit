#include <gepit/gepit.hpp>

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

int32_t evaluate_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle expressionHandle, LVPythonObjRef *returnObjectPtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        auto string = lvStrHandleToStdString(expressionHandle);
        *returnObjectPtr = session->keepObject(pybind11::eval(string, session->scope));
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