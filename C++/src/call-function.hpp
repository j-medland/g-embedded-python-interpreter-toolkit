#include <gepit/gepit.hpp>

// create pybind11 format string
template <typename T>
std::string create_format_descriptor()
{
    return pybind11::format_descriptor<T>::format();
}

// create pybind11 dType
template <typename T>
pybind11::dtype create_dtype()
{
    return pybind11::dtype(create_format_descriptor<T>());
}

// create pybind11 array
template <typename T>
pybind11::array cast_untyped_LVArrayHandle_to_numpy_array(LVVoid_t handle, size_t ndims)
{

    int32_t* dimsPtr = *(reinterpret_cast<int32_t**>(handle));
    // get buffer by offsetting dimsPtr by ndims
    T *buffer = reinterpret_cast<T*>(dimsPtr + ndims);

    // get dims as pybind11::ssize_t vector
    std::vector<pybind11::ssize_t> shape;
    shape.reserve(ndims);

    for (const auto &d : std::span<int32_t>(dimsPtr, ndims))
    {
        shape.push_back(pybind11::ssize_t_cast(d));
    }

    // build strides pybind11::ssize_t vector
    std::vector<pybind11::ssize_t> strides(ndims);

    // reverse iterate over strides and shape
    auto shapeIter = shape.rbegin();
    for (auto it = strides.rbegin(); it != strides.rend(); it++)
    {
        if (it == strides.rbegin())
        {
            // last element
            *it = pybind11::ssize_t_cast(sizeof(T));
            continue;
        }
        // other values: take the previous value of strides
        // and multiply by the current value in the shapeIter
        *it = *shapeIter * (*(it - 1));
        shapeIter++;
    }
    
    auto dtype = create_dtype<T>();
    // add a dummy base array param to set ndarray.owndata to false;
    return pybind11::array(dtype, shape, strides, buffer, pybind11::array());
}

// templates to help with calling a handle's () operator with a std::vector of args
template<std::size_t... S>
pybind11::object function_call_with_args_vector(pybind11::handle &handle, const std::vector<pybind11::object>& args, std::index_sequence<S...>) {
    return handle(args[S]...);
}

template<std::size_t size>
pybind11::object function_call_with_args_vector(pybind11::handle &handle, const std::vector<pybind11::object>& args) {
    return function_call_with_args_vector(handle, args, std::make_index_sequence<size>());
}