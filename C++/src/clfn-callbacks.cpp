#include <gepit/gepit.hpp>

GEPITData::GEPITData()
{
    gil_scoped_release = std::make_unique<pybind11::gil_scoped_release>();
}

// https://lavag.org/topic/22215-call-function-library-node-callbacks/

MgErr reserve(GEPITDataHandlePtr LVInstanceDataPtr)
{
    try
    {
        if(*LVInstanceDataPtr){
            pybind11::initialize_interpreter();
        }
        *LVInstanceDataPtr = new GEPITData();
    }
    catch (std::runtime_error const &e)
    {
        return errorCodes::InterpreterAlreadyRunning;
    }
    catch (std::exception const &e)
    {
        return errorCodes::StdExceptionErr;
    }
    catch (...)
    {
        return errorCodes::UnknownErr;
    }
    return 0;
}

MgErr unreserve(GEPITDataHandlePtr LVInstanceDataPtr)
{
    try
    {
        delete (*LVInstanceDataPtr);
        // do not set the pointer to null so we know if the interpreter is already running
    }
    catch (std::exception const &e)
    {
        return errorCodes::StdExceptionErr;
    }
    catch (...)
    {
        return errorCodes::UnknownErr;
    }
    return 0;
}