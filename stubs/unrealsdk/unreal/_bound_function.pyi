from __future__ import annotations

from typing import Any

from ._uobject import UObject
from ._uobject_children import UFunction

class BoundFunction:
    func: UFunction
    object: UObject

    def __call__(self, *args: Any, **kwargs: Any) -> Any:
        """
        Calls the function.

        Args:
            The unreal function's args. Out params will be used to initialized the
            unreal value, but the python value is not modified in place. Kwargs are
            supported.
            In UE3, optional params should also be optional.
            Alternatively, may call with a single positional WrappedStruct which matches
            the type of the function, in order to reuse the args already stored in it.
        Returns:
            If the function has no out params, returns the actual return value, or
            Ellipsis for a void function.
            If there are out params, returns a tuple, where the first entry is the
            return value as described above, and the following entries are the final
            values of each of the out params, in positional order.
        """
    def __init__(self, func: UFunction, object: UObject) -> None:
        """
        Creates a new bound function.

        Args:
            func: The function to bind.
            object: The object the function is bound to.
        """
