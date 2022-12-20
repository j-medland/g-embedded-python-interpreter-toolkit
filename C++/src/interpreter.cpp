#include <gepit/gepit.hpp>

std::unique_ptr<pybind11::gil_scoped_release> _gil_scoped_release_global_;

int32_t initialize_interpreter(LVErrorClusterPtr errorPtr, LVBoolean *alreadyRunningPtr)
{
    try
    {
        // try starting the interpreter (it might already be running)
        pybind11::initialize_interpreter();
    }
    catch (std::runtime_error const &e)
    {
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

    _gil_scoped_release_global_ = std::make_unique<pybind11::gil_scoped_release>();

    return 0;
}

int32_t finalize_interpreter(LVErrorClusterPtr errorPtr)
{
    try
    {   
         _gil_scoped_release_global_.reset();
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