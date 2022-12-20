#include <gepit/gepit.hpp>

int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, LVBoolean *alreadyRunningPtr)
{
    try
    {
        PyGILState_STATE gstate;
        // try starting the interpreter (it might already be running)
        pybind11::initialize_interpreter();
        // immeditely grab and release the GIL
        PyGILState_Release(PyGILState_Ensure());
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