#include <gepit/gepit.hpp>

template <typename T>
void single_channel_imaq_image_parameters(
    LVIMAQImage image, 
    pybind11::dtype &dtype,
    std::vector<pybind11::ssize_t> &shape,
    std::vector<pybind11::ssize_t> &strides
){
    shape.push_back(pybind11::ssize_t_cast(image.height));
    shape.push_back(pybind11::ssize_t_cast(image.width));
    strides.push_back(pybind11::ssize_t_cast(image.lineWidth * sizeof(T)));
    strides.push_back(pybind11::ssize_t_cast(sizeof(T)));
    dtype = pybind11::dtype(pybind11::format_descriptor<T>::format());
}