#include "pyunrealsdk/pch.h"
#include "pyunrealsdk/base_bindings.h"
#include "pyunrealsdk/unreal_bindings/uenum.h"
#include "pyunrealsdk/unreal_bindings/wrapped_struct.h"
#include "unrealsdk/unreal/classes/uclass.h"
#include "unrealsdk/unreal/classes/uenum.h"
#include "unrealsdk/unreal/classes/uobject.h"
#include "unrealsdk/unreal/classes/uscriptstruct.h"
#include "unrealsdk/unreal/find_class.h"
#include "unrealsdk/unreal/namedobjectcache.h"
#include "unrealsdk/unreal/structs/fname.h"
#include "unrealsdk/unreal/wrappers/gobjects.h"
#include "unrealsdk/unreal/wrappers/wrapped_struct.h"
#include "unrealsdk/unrealsdk.h"
#include "unrealsdk/utils.h"

namespace pyunrealsdk {

using namespace unrealsdk::unreal;

namespace {

NamedObjectCache<UScriptStruct> scriptstruct_cache{};
NamedObjectCache<UEnum> enum_cache{};

/**
 * @brief Finds an object from a name cache, where the name might be fully qualified.
 *
 * @tparam CachedType The type of the cached object.
 * @tparam StrGetterFunc The type of the string getter.
 * @tparam NameGetterFunc The type of the string getter.
 * @param name The name to lookup.
 * @param fully_qualified If the name is fully qualified, or nullopt to autodetect.
 * @param str_getter The getter function to use on fully qualified names, called with a wstring.
 * @param name_getter The getter function to use with partial names, called with an fname.
 * @return The cached value, or it's default if unable to find.
 */
template <typename CachedType, typename StrGetterFunc, typename NameGetterFunc>
CachedType find_cached_potentially_qualified(const std::wstring& name,
                                             std::optional<bool> fully_qualified,
                                             StrGetterFunc str_getter,
                                             NameGetterFunc fname_getter) {
    if (!fully_qualified.has_value()) {
        fully_qualified = name.find_first_of(L".:") != std::string::npos;
    }

    if (fully_qualified.value()) {
        return str_getter(name);
    }
    return fname_getter(FName{name});
}

/**
 * @brief Given an argument which accepts either a class or it's name, attempt to find the class.
 * @note Always auto-detects if fully qualified.
 * @note Throws if unable to find a valid object.
 *
 * @param cls_arg The class argument.
 * @return The class object.
 */
UClass* evaluate_class_arg(const std::variant<UClass*, std::wstring>& cls_arg) {
    if (std::holds_alternative<UClass*>(cls_arg)) {
        auto cls_ptr = std::get<UClass*>(cls_arg);
        if (cls_ptr == nullptr) {
            throw std::invalid_argument("Passed class was null!");
        }
        return cls_ptr;
    }

    auto cls_name = std::get<std::wstring>(cls_arg);
    auto cls_ptr = find_cached_potentially_qualified<UClass*>(
        cls_name, std::nullopt,
        [](const std::wstring& name) { return unrealsdk::unreal::find_class(name); },
        [](const FName& name) { return unrealsdk::unreal::find_class(name); });

    if (cls_ptr == nullptr) {
        throw std::invalid_argument(
            unrealsdk::fmt::format("Couldn't find class {}", unrealsdk::utils::narrow(cls_name)));
    }

    return cls_ptr;
}

}  // namespace

void register_base_bindings(py::module_& mod) {
    mod.def(
        "find_class",
        [](const std::wstring& name, std::optional<bool> fully_qualified) {
            return find_cached_potentially_qualified<UClass*>(
                name, fully_qualified,
                [](const std::wstring& name) { return unrealsdk::unreal::find_class(name); },
                [](const FName& name) { return unrealsdk::unreal::find_class(name); });
        },
        "Finds a class by name.\n"
        "\n"
        "Args:\n"
        "    name: The class name.\n"
        "    fully_qualified: If the class name is fully qualified, or None (the default)\n"
        "                     to autodetect.\n"
        "Returns:\n"
        "    The class, or None if not found.",
        "name"_a, "fully_qualified"_a = std::nullopt);

    mod.def(
        "find_enum",
        [](const std::wstring& name, std::optional<bool> fully_qualified) {
            auto enum_obj = find_cached_potentially_qualified<UEnum*>(
                name, fully_qualified,
                [](const std::wstring& name) { return enum_cache.find(name); },
                [](const FName& name) { return enum_cache.find(name); });

            return unreal::enum_as_py_enum(enum_obj);
        },
        "Finds an enum by name.\n"
        "\n"
        "Args:\n"
        "    name: The enum name.\n"
        "    fully_qualified: If the enum name is fully qualified, or None (the default)\n"
        "                     to autodetect.\n"
        "Returns:\n"
        "    The enum, or None if not found.",
        "name"_a, "fully_qualified"_a = std::nullopt);

    mod.def(
        "make_struct",
        [](const std::wstring& name, std::optional<bool> fully_qualified,
           const py::kwargs& kwargs) {
            auto type = find_cached_potentially_qualified<UScriptStruct*>(
                name, fully_qualified,
                [](const std::wstring& name) { return scriptstruct_cache.find(name); },
                [](const FName& name) { return scriptstruct_cache.find(name); });

            if (type == nullptr) {
                throw py::value_error(unrealsdk::fmt::format("Couldn't find script struct {}",
                                                             unrealsdk::utils::narrow(name)));
            }

            const py::args empty_args{};
            return unreal::make_struct(type, empty_args, kwargs);
        },
        "Finds and constructs a WrappedStruct by name.\n"
        "\n"
        "Args:\n"
        "    name: The struct name.\n"
        "    fully_qualified: If the struct name is fully qualified, or None (the\n"
        "                     default) to autodetect.\n"
        "    **kwargs: Fields on the struct to initialize.\n"
        "Returns:\n"
        "    The newly constructed struct.",
        "name"_a, "fully_qualified"_a = std::nullopt, py::pos_only{});

    mod.def(
        "find_object",
        [](const std::variant<UClass*, std::wstring>& cls_arg, const std::wstring& name) {
            return unrealsdk::find_object(evaluate_class_arg(cls_arg), name);
        },
        "Finds an object by name.\n"
        "\n"
        "Args:\n"
        "    cls: The object's class, or class name. If given as the name, always\n"
        "         autodetects if fully qualified - call find_class() directly if you need\n"
        "         to specify.\n"
        "    name: The object's name.\n"
        "Returns:\n"
        "    The object, or None if not found.",
        "cls"_a, "name"_a);

    mod.def(
        "find_all",
        [](const std::variant<UClass*, std::wstring>& cls_arg, bool exact) {
            auto cls_ptr = evaluate_class_arg(cls_arg);
            auto gobjects = unrealsdk::gobjects();

            std::vector<UObject*> results{};
            if (exact) {
                std::copy_if(gobjects.begin(), gobjects.end(), std::back_inserter(results),
                             [cls_ptr](UObject* obj) { return obj->Class == cls_ptr; });
            } else {
                std::copy_if(gobjects.begin(), gobjects.end(), std::back_inserter(results),
                             [cls_ptr](UObject* obj) { return obj->is_instance(cls_ptr); });
            }

            return results;
        },
        "Finds all instances of a class.\n"
        "\n"
        "Args:\n"
        "    cls: The object's class, or class name. If given as the name, always\n"
        "         autodetects if fully qualified - call find_class() directly if you need\n"
        "         to specify.\n"
        "    exact: If true (the default), only finds exact class matches. If false, also\n"
        "           matches subclasses.\n"
        "Returns:\n"
        "    A list of all instances of the class.",
        "cls"_a, "exact"_a = true);

    mod.def(
        "construct_object",
        [](const std::variant<UClass*, std::wstring>& cls_arg, UObject* outer, const FName& name,
           decltype(UObject::ObjectFlags) flags, UObject* template_obj) {
            return unrealsdk::construct_object(evaluate_class_arg(cls_arg), outer, name, flags,
                                               template_obj);
        },
        "Constructs a new object\n"
        "\n"
        "Args:\n"
        "    cls: The class to construct, or it's name. Required. If given as the name,\n"
        "         always autodetects if fully qualified - call find_class() directly if\n"
        "         you need to specify.\n"
        "    outer: The outer object to construct the new object under. Required.\n"
        "    name: The new object's name.\n"
        "    flags: Object flags to set.\n"
        "    template_obj: The template object to use.\n"
        "Returns:\n"
        "    The constructed object.\n",
        "cls"_a, "outer"_a, "name"_a = FName{0, 0}, "flags"_a = 0, "template_obj"_a = nullptr);
}

}  // namespace pyunrealsdk
