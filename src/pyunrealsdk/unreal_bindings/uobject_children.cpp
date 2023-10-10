#include "pyunrealsdk/pch.h"
#include "pyunrealsdk/unreal_bindings/uobject_children.h"
#include "pyunrealsdk/unreal_bindings/bindings.h"
#include "pyunrealsdk/unreal_bindings/wrapped_struct.h"
#include "unrealsdk/unreal/classes/properties/copyable_property.h"
#include "unrealsdk/unreal/classes/properties/uarrayproperty.h"
#include "unrealsdk/unreal/classes/properties/uboolproperty.h"
#include "unrealsdk/unreal/classes/properties/uclassproperty.h"
#include "unrealsdk/unreal/classes/properties/uenumproperty.h"
#include "unrealsdk/unreal/classes/properties/uinterfaceproperty.h"
#include "unrealsdk/unreal/classes/properties/uobjectproperty.h"
#include "unrealsdk/unreal/classes/properties/ustrproperty.h"
#include "unrealsdk/unreal/classes/properties/ustructproperty.h"
#include "unrealsdk/unreal/classes/properties/utextproperty.h"
#include "unrealsdk/unreal/classes/properties/uweakobjectproperty.h"
#include "unrealsdk/unreal/classes/ublueprintgeneratedclass.h"
#include "unrealsdk/unreal/classes/uclass.h"
#include "unrealsdk/unreal/classes/uconst.h"
#include "unrealsdk/unreal/classes/uenum.h"
#include "unrealsdk/unreal/classes/ufield.h"
#include "unrealsdk/unreal/classes/ufunction.h"
#include "unrealsdk/unreal/classes/uproperty.h"
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/classes/ustruct.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"

#ifdef PYUNREALSDK_INTERNAL

using namespace unrealsdk::unreal;

namespace pyunrealsdk::unreal {

void register_uobject_children(py::module_& mod) {
    // ======== First Layer Subclasses ========

    PyUEClass<UField, UObject>(mod, "UField").def_readwrite("Next", &UField::Next);

    // ======== Second Layer Subclasses ========

    PyUEClass<UConst, UField>(mod, "UConst")
        .def_property(
            "Value", [](const UConst* self) { return (std::string)self->Value; },
            [](UConst* self, const std::string& new_value) { self->Value = new_value; });

    PyUEClass<UProperty, UField>(mod, "UProperty")
        .def_readwrite("ArrayDim", &UProperty::ArrayDim)
        .def_readwrite("ElementSize", &UProperty::ElementSize)
        .def_readwrite("PropertyFlags", &UProperty::PropertyFlags)
        .def_readwrite("Offset_Internal", &UProperty::Offset_Internal)
        .def_readwrite("PropertyLinkNext", &UProperty::PropertyLinkNext);

    PyUEClass<UStruct, UField>(mod, "UStruct")
        .def(
            "_fields",
            [](UStruct* self) {
                auto fields = self->fields();
                // I don't like how calling this with the `fields` directly takes an lvalue
                // reference - would prefer to move an rvalue.
                // Testing, everything still works fine, there's no memory leak, but prefer to
                // manually call this with the iterators anyway
                return py::make_iterator(fields.begin(), fields.end());
            },
            "Iterates over all fields in the struct.\n"
            "\n"
            "Returns:\n"
            "    An iterator over all fields in the struct.")
        .def(
            "_properties",
            [](UStruct* self) {
                auto props = self->properties();
                return py::make_iterator(props.begin(), props.end());
            },
            "Iterates over all properties in the struct.\n"
            "\n"
            "Returns:\n"
            "    An iterator over all properties in the struct.")
        .def("_inherits", &UStruct::inherits,
             "Checks if this structs inherits from another.\n"
             "\n"
             "Also returns true if this struct *is* the given struct.\n"
             "\n"
             "Args:\n"
             "    base_struct: The base struct to check if this inherits from.\n"
             "Returns:\n"
             "    True if this struct is the given struct, or inherits from it.",
             "base_struct"_a)
        .def("_get_struct_size", &UStruct::get_struct_size,
             "Gets the actual size of the described structure, including alignment.\n"
             "\n"
             "Returns:\n"
             "    The size which must be allocated.")
        .def("_find", &UStruct::find,
             "Finds a child field by name.\n"
             "\n"
             "Throws an exception if the child is not found.\n"
             "\n"
             "Args:\n"
             "    name: The name of the child field.\n"
             "Returns:\n"
             "    The found child field.",
             "name"_a)
        .def("_find_prop", &UStruct::find_prop,
             "Finds a child property by name.\n"
             "\n"
             "When known to be a property, this is more efficient than _find.\n"
             "\n"
             "Throws an exception if the child is not found.\n"
             "\n"
             "Args:\n"
             "    name: The name of the child property.\n"
             "Returns:\n"
             "    The found child property.",
             "name"_a)
        .def_readwrite("SuperField", &UStruct::SuperField)
        .def_readwrite("Children", &UStruct::Children)
        .def_readwrite("PropertyLink", &UStruct::PropertyLink);

    // ======== Third Layer Subclasses ========

    PyUEClass<UArrayProperty, UProperty>(mod, "UArrayProperty")
        .def_property_readonly("Inner", &UArrayProperty::get_inner);

    PyUEClass<UBoolProperty, UProperty>(mod, "UBoolProperty")
        .def_property_readonly("FieldMask", &UBoolProperty::get_field_mask);

    PyUEClass<UByteProperty, UProperty>(mod, "UByteProperty");

    PyUEClass<UClass, UStruct>(mod, "UClass")
        .def(
            "_implements",
            [](UClass* self, UClass* interface) { return self->implements(interface, nullptr); },
            "Checks if this class implements a given interface.\n"
            "\n"
            "Args:\n"
            "    interface: The interface to check.\n"
            "Returns:\n"
            "    True if this class implements the interface, false otherwise.",
            "interface"_a)
        .def_readwrite("ClassDefaultObject", &UClass::ClassDefaultObject)
        .def_property_readonly("Interfaces", [](const UClass* self) {
            std::vector<UClass*> interfaces{};
            for (const auto& iface : self->Interfaces) {
                interfaces.push_back(iface.Class);
            }
            return interfaces;
        });

    PyUEClass<UDoubleProperty, UProperty>(mod, "UDoubleProperty");

    PyUEClass<UEnumProperty, UProperty>(mod, "UEnumProperty")
        .def_property_readonly("UnderlyingProp", &UEnumProperty::get_underlying_prop)
        .def_property_readonly("Enum", &UEnumProperty::get_enum);

    PyUEClass<UFloatProperty, UProperty>(mod, "UFloatProperty");

    PyUEClass<UFunction, UStruct>(mod, "UFunction")
        .def("_find_return_param", &UFunction::find_return_param,
             "Finds the return param for this function (if it exists).\n"
             "\n"
             "Returns:\n"
             "    The return param, or None if it doesn't exist.")
        .def_readwrite("FunctionFlags", &UFunction::FunctionFlags)
        .def_readwrite("NumParams", &UFunction::NumParams)
        .def_readwrite("ParamsSize", &UFunction::ParamsSize)
        .def_readwrite("ReturnValueOffset", &UFunction::ReturnValueOffset);

    PyUEClass<UInt8Property, UProperty>(mod, "UInt8Property");

    PyUEClass<UInt16Property, UProperty>(mod, "UInt16Property");

    PyUEClass<UInt64Property, UProperty>(mod, "UInt64Property");

    PyUEClass<UInterfaceProperty, UProperty>(mod, "UInterfaceProperty")
        .def_property_readonly("InterfaceClass", &UInterfaceProperty::get_interface_class);

    PyUEClass<UIntProperty, UProperty>(mod, "UIntProperty");

    PyUEClass<UNameProperty, UProperty>(mod, "UNameProperty");

    PyUEClass<UObjectProperty, UProperty>(mod, "UObjectProperty")
        .def_property_readonly("PropertyClass", &UObjectProperty::get_property_class);

    PyUEClass<UScriptStruct, UStruct>(mod, "UScriptStruct")
        .def_readwrite("StructFlags", &UScriptStruct::StructFlags);

    PyUEClass<UStrProperty, UProperty>(mod, "UStrProperty");

    PyUEClass<UStructProperty, UProperty>(mod, "UStructProperty")
        .def_property_readonly("Struct", &UStructProperty::get_inner_struct);

    PyUEClass<UTextProperty, UProperty>(mod, "UTextProperty");

    PyUEClass<UUInt16Property, UProperty>(mod, "UUInt16Property");

    PyUEClass<UUInt32Property, UProperty>(mod, "UUInt32Property");

    PyUEClass<UUInt64Property, UProperty>(mod, "UUInt64Property");

    // ======== Fourth Layer Subclasses ========

    PyUEClass<UBlueprintGeneratedClass, UClass>(mod, "UBlueprintGeneratedClass");

    PyUEClass<UClassProperty, UObjectProperty>(mod, "UClassProperty")
        .def_property_readonly("MetaClass", &UClassProperty::get_meta_class);

    PyUEClass<UWeakObjectProperty, UObjectProperty>(mod, "UWeakObjectProperty");
}

}  // namespace pyunrealsdk::unreal

#endif
