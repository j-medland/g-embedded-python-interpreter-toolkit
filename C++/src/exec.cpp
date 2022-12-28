#include <gepit/gepit.hpp>

int32_t exec_string(LVErrorClusterPtr errorPtr, SessionHandle session, LVStrHandle stringHandle)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::exec(lvStrHandleToStdString(stringHandle), session->scope);
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