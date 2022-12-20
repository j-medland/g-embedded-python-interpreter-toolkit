#include <algorithm>
#include <functional>

#include "py-object.hpp"

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

int32_t create_py_object_IMAQ(LVErrorClusterPtr errorPtr, SessionHandle session, LVIMAQImagePtr imaqImagePtr, LVPythonObjRef *returnObjectPtr)
{

    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        std::vector<pybind11::ssize_t> shape;
        std::vector<pybind11::ssize_t> strides;
        pybind11::dtype dtype;

        // setup based on image type
        switch (imaqImagePtr->type)
        {
        case Grayscale_U8:
            single_channel_imaq_image_parameters<uint8_t>(*imaqImagePtr, dtype, shape, strides);
            break;
        case Grayscale_U16:
            single_channel_imaq_image_parameters<uint16_t>(*imaqImagePtr, dtype, shape, strides);
            break;
        case Grayscale_I16:
            single_channel_imaq_image_parameters<int16_t>(*imaqImagePtr, dtype, shape, strides);
            break;
        case Grayscale_SGL:
            single_channel_imaq_image_parameters<float>(*imaqImagePtr, dtype, shape, strides);
            break;
        case RGB_U32:
        case HSL_U32:
            // Make life easier by representing a LabVIEW U32 (ARGB) image as a 3D array (height, width, channels)
            // As a bonus - LabVIEW's U32 type is in little-endian format but Pythons is in big-endian
            // so the pixel bytes are actually already arranged in the order OpenCV  expects (BGRA)

            // start by setting the parameters as though this is a single channel image
            single_channel_imaq_image_parameters<uint8_t>(*imaqImagePtr, dtype, shape, strides);
            // multiply each element of strides by 4 (number of channels)
            std::transform(strides.begin(), strides.end(), strides.begin(), std::bind(std::multiplies<pybind11::ssize_t>(), std::placeholders::_1, 4));
            // extend shape and strides vectors
            shape.push_back(4);
            strides.push_back(pybind11::ssize_t_cast(sizeof(uint8_t)));
            break;
        case RGB_U64:
        case Complex_CSG:
            throw std::invalid_argument("Image types RGB (U64) and Complex (CSG) not supported");
            break;
        }
        // pass dummy array to pybind constructor to set OWNDATA to false
        *returnObjectPtr = session->keepObject(pybind11::array(dtype, shape, strides, reinterpret_cast<void*>(imaqImagePtr->pixelPointer), pybind11::array()));

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

int32_t cast_py_object_to_dbl(LVErrorClusterPtr errorPtr, SessionHandle session, LVPythonObjRef object, double *returnValuePtr)
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
        *returnValuePtr = session->getObject(object).cast<double>();

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

        return writeStringToStringHandlePtr(strHandlePtr, pybind11::str(session->getObject(object)));
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