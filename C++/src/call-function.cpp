#include <algorithm>
#include <span>

#include "call-function.hpp"

pybind11::object convertHandleToPythonObject(SessionHandle session, LVVoid_t handle, LVTypeInfo typeInfo){
        switch (typeInfo.type)
        {
        case LVNumericType::I8_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<int8_t>(handle, typeInfo.ndims);

        case LVNumericType::I16_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<int16_t>(handle, typeInfo.ndims);

        case LVNumericType::I32_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<int32_t>(handle, typeInfo.ndims);

        case LVNumericType::I64_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<int64_t>(handle, typeInfo.ndims);

        case LVNumericType::U8_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<uint8_t>(handle, typeInfo.ndims);

        case LVNumericType::U16_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<uint16_t>(handle, typeInfo.ndims);

        case LVNumericType::U32_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<uint32_t>(handle, typeInfo.ndims);

        case LVNumericType::U64_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<uint64_t>(handle, typeInfo.ndims);

        case LVNumericType::SGL_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<float>(handle, typeInfo.ndims);

        case LVNumericType::DBL_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<double>(handle, typeInfo.ndims);

        case LVNumericType::EXT_ARRAY:
            return cast_untyped_LVArrayHandle_to_numpy_array<long double>(handle, typeInfo.ndims);

        case LVNumericType::PYOBJ:
            if (session->isNullObject(handle))
            {
                throw std::out_of_range("Null or Invalid Python Object supplied as a Function Argument.");
            }
            return session->getObject(handle);

        default:
            throw std::out_of_range("Non-supported type supplied as a Function Argument.");
        }
        // we should never get to here
        return pybind11::none();
    }

void convertArgsToPythonObjects(SessionHandle session, LVArgumentClusterPtr argsPtr, LVArgumentTypeInfoHandle argTypesInfoHandle, std::vector<pybind11::object>&argObjects)
{

    size_t nargs = argTypesInfoHandle && (*argTypesInfoHandle) && (*argTypesInfoHandle)->dims ? (*argTypesInfoHandle)->dims[0] : 0;
    argObjects.reserve(nargs);

    auto argsTypesInfoSpan = std::span{(*argTypesInfoHandle)->data(), nargs};

    if(nargs == 1){
        auto typeInfo = argsTypesInfoSpan[0];
        // check type
        // we actually just have a handle to the object, not a pointer to a cluster of handles
        LVVoid_t handle = reinterpret_cast<LVVoid_t>(argsPtr);
        argObjects.push_back(convertHandleToPythonObject(session, handle, typeInfo));
        return;
    }

    // where nargs !=1
    auto argsSpan = std::span{argsPtr, nargs};
    auto argIter = argsSpan.begin();

    for (const auto &typeInfo : argsTypesInfoSpan){
        argObjects.push_back(convertHandleToPythonObject(session, *argIter, typeInfo));
        argIter++;
    }
}

int32_t call_function(LVErrorClusterPtr errorPtr,
                                   SessionHandle session,
                                   LVPythonObjRef classInstance,
                                   LVStrHandle fnNameStrHandle,
                                   LVArgumentClusterPtr argsPtr,
                                   LVArgumentTypeInfoHandle argTypesInfoHandle,
                                   LVPythonObjRef *returnObjectPtr)
{
    if (!session)
    {
        return writeInvalidSessionHandleErr(errorPtr, __func__);
    }
    try
    {
        pybind11::object result = pybind11::none();
        std::vector<pybind11::object> argObjects;
        // convert array of LVRefNums to vector of Python Objects
        convertArgsToPythonObjects(session, argsPtr, argTypesInfoHandle, argObjects);

        // Get a Ref to the Function in the session scope or the class
        std::string fnNameString = lvStrHandleToStdString(fnNameStrHandle);
        pybind11::handle fnHandle;

        if (session->isNullObject(classInstance))
        {
            // handle is an item of the scope dict
            fnHandle = session->scope[fnNameString.c_str()];
        }
        else
        {
            // handle is an attribute of a class
            fnHandle = session->getObject(classInstance).attr(fnNameString.c_str());
        }

        // C++ needs the argument expanded function calls to exist at compile time
        // hence this switch and a physical limit on the number of args
        // this-is-fine.jpg
        switch (argObjects.size())
        {
        case 0:
            result = fnHandle();
            break;
         case 1:
            result = function_call_with_args_vector<1>(fnHandle, argObjects);
            break;
        case 2:
            result = function_call_with_args_vector<2>(fnHandle, argObjects);
            break;
        case 3:
            result = function_call_with_args_vector<3>(fnHandle, argObjects);
            break;
        case 4:
            result = function_call_with_args_vector<4>(fnHandle, argObjects);
            break;
        case 5:
            result = function_call_with_args_vector<5>(fnHandle, argObjects);
            break;
        case 6:
            result = function_call_with_args_vector<6>(fnHandle, argObjects);
            break;
        case 7:
            result = function_call_with_args_vector<7>(fnHandle, argObjects);
            break;
        case 8:
            result = function_call_with_args_vector<8>(fnHandle, argObjects);
            break;
        case 9:
            result = function_call_with_args_vector<9>(fnHandle, argObjects);
            break;
        case 10:
            result = function_call_with_args_vector<10>(fnHandle, argObjects);
            break;
        case 11:
            result = function_call_with_args_vector<11>(fnHandle, argObjects);
            break;
        case 12:
            result = function_call_with_args_vector<12>(fnHandle, argObjects);
            break;
        case 13:
            result = function_call_with_args_vector<13>(fnHandle, argObjects);
            break;
        case 14:
            result = function_call_with_args_vector<14>(fnHandle, argObjects);
            break;
        case 15:
            result = function_call_with_args_vector<15>(fnHandle, argObjects);
            break;
        case 16:
            result = function_call_with_args_vector<16>(fnHandle, argObjects);
            break;
        case 17:
            result = function_call_with_args_vector<17>(fnHandle, argObjects);
            break;
        case 18:
            result = function_call_with_args_vector<18>(fnHandle, argObjects);
            break;
        case 19:
            result = function_call_with_args_vector<19>(fnHandle, argObjects);
            break;
        case 20:
            result = function_call_with_args_vector<20>(fnHandle, argObjects);
            break;
        case 21:
            result = function_call_with_args_vector<21>(fnHandle, argObjects);
            break;
        case 22:
            result = function_call_with_args_vector<22>(fnHandle, argObjects);
            break;
        case 23:
            result = function_call_with_args_vector<23>(fnHandle, argObjects);
            break;
        case 24:
            result = function_call_with_args_vector<24>(fnHandle, argObjects);
            break;
        case 25:
            result = function_call_with_args_vector<25>(fnHandle, argObjects);
            break;
        case 26:
            result = function_call_with_args_vector<26>(fnHandle, argObjects);
            break;
        case 27:
            result = function_call_with_args_vector<27>(fnHandle, argObjects);
            break;
        case 28:
            result = function_call_with_args_vector<28>(fnHandle, argObjects);
            break;
        case 29:
            result = function_call_with_args_vector<29>(fnHandle, argObjects);
            break;
        case 30:
            result = function_call_with_args_vector<30>(fnHandle, argObjects);
            break;
        case 31:
            result = function_call_with_args_vector<31>(fnHandle, argObjects);
            break;
        case 32:
            result = function_call_with_args_vector<32>(fnHandle, argObjects);
            break;
        case 33:
            result = function_call_with_args_vector<33>(fnHandle, argObjects);
            break;
        case 34:
            result = function_call_with_args_vector<34>(fnHandle, argObjects);
            break;
        case 35:
            result = function_call_with_args_vector<35>(fnHandle, argObjects);
            break;
        case 36:
            result = function_call_with_args_vector<36>(fnHandle, argObjects);
            break;
        case 37:
            result = function_call_with_args_vector<37>(fnHandle, argObjects);
            break;
        case 38:
            result = function_call_with_args_vector<38>(fnHandle, argObjects);
            break;
        case 39:
            result = function_call_with_args_vector<39>(fnHandle, argObjects);
            break;
        case 40:
            result = function_call_with_args_vector<40>(fnHandle, argObjects);
            break;
        case 41:
            result = function_call_with_args_vector<41>(fnHandle, argObjects);
            break;
        case 42:
            result = function_call_with_args_vector<42>(fnHandle, argObjects);
            break;
        case 43:
            result = function_call_with_args_vector<43>(fnHandle, argObjects);
            break;
        case 44:
            result = function_call_with_args_vector<44>(fnHandle, argObjects);
            break;
        case 45:
            result = function_call_with_args_vector<45>(fnHandle, argObjects);
            break;
        case 46:
            result = function_call_with_args_vector<46>(fnHandle, argObjects);
            break;
        case 47:
            result = function_call_with_args_vector<47>(fnHandle, argObjects);
            break;
        case 48:
            result = function_call_with_args_vector<48>(fnHandle, argObjects);
            break;
        case 49:
            result = function_call_with_args_vector<49>(fnHandle, argObjects);
            break;
        case 50:
            result = function_call_with_args_vector<50>(fnHandle, argObjects);
            break;
        case 51:
            result = function_call_with_args_vector<51>(fnHandle, argObjects);
            break;
        case 52:
            result = function_call_with_args_vector<52>(fnHandle, argObjects);
            break;
        case 53:
            result = function_call_with_args_vector<53>(fnHandle, argObjects);
            break;
        case 54:
            result = function_call_with_args_vector<54>(fnHandle, argObjects);
            break;
        case 55:
            result = function_call_with_args_vector<55>(fnHandle, argObjects);
            break;
        case 56:
            result = function_call_with_args_vector<56>(fnHandle, argObjects);
            break;
        case 57:
            result = function_call_with_args_vector<57>(fnHandle, argObjects);
            break;
        case 58:
            result = function_call_with_args_vector<58>(fnHandle, argObjects);
            break;
        case 59:
            result = function_call_with_args_vector<59>(fnHandle, argObjects);
            break;
        case 60:
            result = function_call_with_args_vector<60>(fnHandle, argObjects);
            break;
        case 61:
            result = function_call_with_args_vector<61>(fnHandle, argObjects);
            break;
        case 62:
            result = function_call_with_args_vector<62>(fnHandle, argObjects);
            break;
        case 63:
            result = function_call_with_args_vector<63>(fnHandle, argObjects);
            break;
        case 64:
            result = function_call_with_args_vector<64>(fnHandle, argObjects);
            break;
        case 65:
            result = function_call_with_args_vector<65>(fnHandle, argObjects);
            break;
        case 66:
            result = function_call_with_args_vector<66>(fnHandle, argObjects);
            break;
        case 67:
            result = function_call_with_args_vector<67>(fnHandle, argObjects);
            break;
        case 68:
            result = function_call_with_args_vector<68>(fnHandle, argObjects);
            break;
        case 69:
            result = function_call_with_args_vector<69>(fnHandle, argObjects);
            break;
        case 70:
            result = function_call_with_args_vector<70>(fnHandle, argObjects);
            break;
        case 71:
            result = function_call_with_args_vector<71>(fnHandle, argObjects);
            break;
        case 72:
            result = function_call_with_args_vector<72>(fnHandle, argObjects);
            break;
        case 73:
            result = function_call_with_args_vector<73>(fnHandle, argObjects);
            break;
        case 74:
            result = function_call_with_args_vector<74>(fnHandle, argObjects);
            break;
        case 75:
            result = function_call_with_args_vector<75>(fnHandle, argObjects);
            break;
        case 76:
            result = function_call_with_args_vector<76>(fnHandle, argObjects);
            break;
        case 77:
            result = function_call_with_args_vector<77>(fnHandle, argObjects);
            break;
        case 78:
            result = function_call_with_args_vector<78>(fnHandle, argObjects);
            break;
        case 79:
            result = function_call_with_args_vector<79>(fnHandle, argObjects);
            break;
        case 80:
            result = function_call_with_args_vector<80>(fnHandle, argObjects);
            break;
        case 81:
            result = function_call_with_args_vector<81>(fnHandle, argObjects);
            break;
        case 82:
            result = function_call_with_args_vector<82>(fnHandle, argObjects);
            break;
        case 83:
            result = function_call_with_args_vector<83>(fnHandle, argObjects);
            break;
        case 84:
            result = function_call_with_args_vector<84>(fnHandle, argObjects);
            break;
        case 85:
            result = function_call_with_args_vector<85>(fnHandle, argObjects);
            break;
        case 86:
            result = function_call_with_args_vector<86>(fnHandle, argObjects);
            break;
        case 87:
            result = function_call_with_args_vector<87>(fnHandle, argObjects);
            break;
        case 88:
            result = function_call_with_args_vector<88>(fnHandle, argObjects);
            break;
        case 89:
            result = function_call_with_args_vector<89>(fnHandle, argObjects);
            break;
        case 90:
            result = function_call_with_args_vector<90>(fnHandle, argObjects);
            break;
        case 91:
            result = function_call_with_args_vector<91>(fnHandle, argObjects);
            break;
        case 92:
            result = function_call_with_args_vector<92>(fnHandle, argObjects);
            break;
        case 93:
            result = function_call_with_args_vector<93>(fnHandle, argObjects);
            break;
        case 94:
            result = function_call_with_args_vector<94>(fnHandle, argObjects);
            break;
        case 95:
            result = function_call_with_args_vector<95>(fnHandle, argObjects);
            break;
        case 96:
            result = function_call_with_args_vector<96>(fnHandle, argObjects);
            break;
        case 97:
            result = function_call_with_args_vector<97>(fnHandle, argObjects);
            break;
        case 98:
            result = function_call_with_args_vector<98>(fnHandle, argObjects);
            break;
        case 99:
            result = function_call_with_args_vector<99>(fnHandle, argObjects);
            break;
        case 100:
            result = function_call_with_args_vector<100>(fnHandle, argObjects);
            break;
        case 101:
            result = function_call_with_args_vector<101>(fnHandle, argObjects);
            break;
        case 102:
            result = function_call_with_args_vector<102>(fnHandle, argObjects);
            break;
        case 103:
            result = function_call_with_args_vector<103>(fnHandle, argObjects);
            break;
        case 104:
            result = function_call_with_args_vector<104>(fnHandle, argObjects);
            break;
        case 105:
            result = function_call_with_args_vector<105>(fnHandle, argObjects);
            break;
        case 106:
            result = function_call_with_args_vector<106>(fnHandle, argObjects);
            break;
        case 107:
            result = function_call_with_args_vector<107>(fnHandle, argObjects);
            break;
        case 108:
            result = function_call_with_args_vector<108>(fnHandle, argObjects);
            break;
        case 109:
            result = function_call_with_args_vector<109>(fnHandle, argObjects);
            break;
        case 110:
            result = function_call_with_args_vector<110>(fnHandle, argObjects);
            break;
        case 111:
            result = function_call_with_args_vector<111>(fnHandle, argObjects);
            break;
        case 112:
            result = function_call_with_args_vector<112>(fnHandle, argObjects);
            break;
        case 113:
            result = function_call_with_args_vector<113>(fnHandle, argObjects);
            break;
        case 114:
            result = function_call_with_args_vector<114>(fnHandle, argObjects);
            break;
        case 115:
            result = function_call_with_args_vector<115>(fnHandle, argObjects);
            break;
        case 116:
            result = function_call_with_args_vector<116>(fnHandle, argObjects);
            break;
        case 117:
            result = function_call_with_args_vector<117>(fnHandle, argObjects);
            break;
        case 118:
            result = function_call_with_args_vector<118>(fnHandle, argObjects);
            break;
        case 119:
            result = function_call_with_args_vector<119>(fnHandle, argObjects);
            break;
        case 120:
            result = function_call_with_args_vector<120>(fnHandle, argObjects);
            break;
        case 121:
            result = function_call_with_args_vector<121>(fnHandle, argObjects);
            break;
        case 122:
            result = function_call_with_args_vector<122>(fnHandle, argObjects);
            break;
        case 123:
            result = function_call_with_args_vector<123>(fnHandle, argObjects);
            break;
        case 124:
            result = function_call_with_args_vector<124>(fnHandle, argObjects);
            break;
        case 125:
            result = function_call_with_args_vector<125>(fnHandle, argObjects);
            break;
        case 126:
            result = function_call_with_args_vector<126>(fnHandle, argObjects);
            break;
        case 127:
            result = function_call_with_args_vector<127>(fnHandle, argObjects);
            break;
        default:
            throw std::out_of_range("Number of arguments cannot exceed 127. Consider creating a python function which can use *args");
        }
        *returnObjectPtr = session->keepObject(result);
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