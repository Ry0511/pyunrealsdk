# ruff: noqa: D205

from __future__ import annotations

from ._bound_function import BoundFunction
from ._uenum import UEnum
from ._uobject import UObject
from ._uobject_children import (
    UArrayProperty,
    UBlueprintGeneratedClass,
    UBoolProperty,
    UByteProperty,
    UClass,
    UClassProperty,
    UConst,
    UDoubleProperty,
    UEnumProperty,
    UField,
    UFloatProperty,
    UFunction,
    UInt8Property,
    UInt16Property,
    UInt64Property,
    UInterfaceProperty,
    UIntProperty,
    ULazyObjectProperty,
    UNameProperty,
    UObjectProperty,
    UProperty,
    UScriptStruct,
    USoftClassProperty,
    USoftObjectProperty,
    UStrProperty,
    UStruct,
    UStructProperty,
    UTextProperty,
    UUInt16Property,
    UUInt32Property,
    UUInt64Property,
)
from ._weak_pointer import WeakPointer
from ._wrapped_array import WrappedArray
from ._wrapped_multicast_delegate import WrappedMulticastDelegate
from ._wrapped_struct import WrappedStruct

__all__: tuple[str, ...] = (
    "BoundFunction",
    "UArrayProperty",
    "UBlueprintGeneratedClass",
    "UBoolProperty",
    "UByteProperty",
    "UClass",
    "UClassProperty",
    "UConst",
    "UDoubleProperty",
    "UEnum",
    "UEnumProperty",
    "UField",
    "UFloatProperty",
    "UFunction",
    "UInt8Property",
    "UInt16Property",
    "UInt64Property",
    "UInterfaceProperty",
    "UIntProperty",
    "ULazyObjectProperty",
    "UNameProperty",
    "UObject",
    "UObjectProperty",
    "UProperty",
    "UScriptStruct",
    "USoftClassProperty",
    "USoftObjectProperty",
    "UStrProperty",
    "UStruct",
    "UStructProperty",
    "UTextProperty",
    "UUInt16Property",
    "UUInt32Property",
    "UUInt64Property",
    "WeakPointer",
    "WrappedArray",
    "WrappedMulticastDelegate",
    "WrappedStruct",
    "dir_includes_unreal",
)

def dir_includes_unreal(should_include: bool) -> None:
    """
    Sets if `__dir__` should include dynamic unreal properties, specific to the
    object. Defaults to true.

    Args:
        should_include: True if to include dynamic properties, false to not.
    """
