#include <gepit/gepit.hpp>

int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, InterpreterData** dataHandlePtr )
{
    try
    {
        // try starting the interpreter (it might already be running)
        pybind11::initialize_interpreter();
        *dataHandlePtr = new InterpreterData();
        (*dataHandlePtr)->gil_scoped_release = std::make_unique<pybind11::gil_scoped_release>();
    }
    catch (std::runtime_error const &e)
    {
        // already running
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

int32_t finalize_interpreter(LVErrorClusterPtr errorPtr, InterpreterData* dataHandle)
{
    try
    {   
        delete(dataHandle);
        // finalizing mostly breaks stuff so skip it
        // pybind11::finalize_interpreter();
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