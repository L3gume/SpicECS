#pragma once
#ifndef REFLECTION_H
#define REFLECTION_H

#include <tuple>
#include <type_traits>
#include <utility>
#include <sstream>

#include "../../3rdparty/rapidjson/rapidjson.h"
#include "../../3rdparty/rapidjson/document.h"
#include "../../3rdparty/rapidjson/allocators.h"

/*
 * Implementation of a reflection system for easier serialization
 * and manipulation of properties for ecs components and entities
 * 
 * This is very heavily inspired by DonerReflection, a project on github:
 * https://github.com/Donerkebap13/DonerReflection
 *
 * It is almost identical as I am learning about the concepts that
 * are used in this file and I will do my best to explain how it works.
 * 
 * It is a very minimal system and will be kept as minimal as possible.
 * 
 * - Justin
 */
 
// ----------------------- MACROS -----------------------------------------

// Main macro, initializes reflection data with a variadic list of properties
// This one takes a name as an arg
#define H_DEFINE_NAMED_REFLECTION_DATA(class_name, base_class, ...)         \
    template <> struct SReflectionProperties<base_class> {                  \
        using T = base_class;                                               \
        constexpr static auto s_className = class_name;                     \
        constexpr static auto s_properties = std::make_tuple(__VA_ARGS__);  \
        constexpr static std::size_t s_propertiesCount =                    \
            std::tuple_size<decltype(s_properties)>::value;                 \
    };

// Defines a named property, nameless properties are not supported in Habanero
#define H_ADD_NAMED_PROPERTY(prop, name)                                    \
    hreflection::SProperty<T, decltype(T::prop)> { &T::prop, name }

#define H_DECLARE_AS_REFLECTABLE(base_class)                                \
    friend struct SReflectionProperties<base_class>;

// ----------------------- RESOLVERS  -------------------------------------

#define H_APPLY_RESOLVER(object_ref, resolver, ...)                         \
    hreflection::ApplyFunc<                                                 \
        SReflectionProperties<typename std::decay<decltype(                 \
                object_ref)>::type>::s_propertiesCount - 1,                 \
           resolver>(object_ref, __VA_ARGS__); 

#define H_APPLY_RESOLVER_TO_CONST(object_ref, resolver, ...)                \
    hreflection::ApplyFuncToConst<                                          \
        SReflectionProperties<typename std::decay<decltype(                 \
                object_ref)>::type>::s_propertiesCount - 1,                 \
           resolver>(object_ref, __VA_ARGS__); 


// ----------------------- REFLECTION -------------------------------------

template <typename T> struct SReflectionProperties {
    // The members are static, which mean they weill only be initialized once per type T
    // since templates are initialized at compile-time
    constexpr static auto s_className = "Undefined";
    constexpr static auto s_properties = std::tuple<>();
    constexpr static std::size_t s_propertiesCount = std::tuple_size<decltype(s_properties)>::value;
};

// this is going to have its own namespace
namespace hreflection
{
    // Property type: this is the backbone of this whole system.
    // Components are registered witht their set of properties,
    // which can be accessed at runtime to gain knowledge of a
    // member's type
    template <typename Class, typename T>
    struct SProperty {
        // Properties are generated at compile-time using a macro, hence the constexpr
        constexpr SProperty(T Class::*member, const char* name) : member(member), name(name) {}

        using Type = T;
        
        T Class::*member; // pointer to the class's member, this allows for some funky stuff, see below
        const char* name; // name of the property
    };
    
    // Non const objs with args
    
    template <std::size_t iter, typename Resolver, typename T, typename... Args>
    void DoApplyFunc(T& obj, Args&&... args) {
        constexpr auto prop = std::get<iter>(std::decay<SReflectionProperties<T>>::type::s_properties);
        Resolver::Resolve(prop, obj, std::forward<Args>(args)...);
    }
    
    template <std::size_t iter, typename Resolver, typename T, typename... Args>
    typename std::enable_if<(iter == 0)>::type ApplyFunc(T& obj, Args&&... args) {
        DoApplyFunc<iter, Resolver>(obj, std::forward<Args>(args)...);
    }
    
    template <std::size_t iter, typename Resolver, typename T, typename... Args>
    typename std::enable_if<(iter > 0)>::type ApplyFunc(T& obj, Args&&... args) {
        DoApplyFunc<iter, Resolver>(obj, std::forward<Args>(args)...);
        ApplyFunc<iter - 1, Resolver>(obj, std::forward<Args>(args)...);
    }
    
    // Non const without args
    
    template <std::size_t iter, typename Resolver, typename T>
    void DoApplyFunc(T& obj) {
        constexpr auto prop = std::get<iter>(std::decay<SReflectionProperties<T>>::type::s_properties);
        Resolver::Resolve(prop, obj);
    }
    
    template <std::size_t iter, typename Resolver, typename T>
    typename std::enable_if<(iter == 0)>::type ApplyFunc(T& obj) {
        DoApplyFunc<iter, Resolver>(obj);
    }
    
    template <std::size_t iter, typename Resolver, typename T>
    typename std::enable_if<(iter > 0)>::type ApplyFunc(T& obj) {
        DoApplyFunc<iter, Resolver>(obj);
        ApplyFunc<iter - 1, Resolver>(obj);
    }
    
    // const objs with args
    
    template <std::size_t iter, typename Resolver, typename T, typename... Args>
    void DoApplyFuncToConst(const T& obj, Args&&... args) {
        constexpr auto prop = std::get<iter>(std::decay<SReflectionProperties<T>>::type::s_properties);
        Resolver::Resolve(prop, obj, std::forward<Args>(args)...);
    }
    
    template <std::size_t iter, typename Resolver, typename T, typename... Args>
    typename std::enable_if<(iter == 0)>::type ApplyFuncToConst(const T& obj, Args&&... args) {
        DoApplyFuncToConst<iter, Resolver>(obj, std::forward<Args>(args)...);
    }
    
    template <std::size_t iter, typename Resolver, typename T, typename... Args>
    typename std::enable_if<(iter > 0)>::type ApplyFuncToConst(const T& obj, Args&&... args) {
        DoApplyFuncToConst<iter, Resolver>(obj, std::forward<Args>(args)...);
        ApplyFuncToConst<iter - 1, Resolver>(obj, std::forward<Args>(args)...);
    }
    
    // const without args
    
    template <std::size_t iter, typename Resolver, typename T>
    void DoApplyFuncToConst(const T& obj) {
        constexpr auto prop = std::get<iter>(std::decay<SReflectionProperties<T>>::type::s_properties);
        Resolver::Resolve(prop, obj);
    }
    
    template <std::size_t iter, typename Resolver, typename T>
    typename std::enable_if<(iter == 0)>::type ApplyFuncToConst(const T& obj) {
        DoApplyFuncToConst<iter, Resolver>(obj);
    }
    
    template <std::size_t iter, typename Resolver, typename T>
    typename std::enable_if<(iter > 0)>::type ApplyFuncToConst(const T& obj) {
        DoApplyFuncToConst<iter, Resolver>(obj);
        ApplyFuncToConst<iter - 1, Resolver>(obj);
    }
    
    /*
     * HOW TO USE PROPERTIES:
     *
     * SReflectionProperties<Foo>::s_properties; // gives access to the properties
     * 
     * each elem in the s_properties tuple is of type SProperty<ClassType, MemberType>
     * 
     * hreflection::SProperty<Foo, int32> prop;
     * prop.m_name;
     * prop.m_member; // pointer to class member
     * 
     * To use the member:
     *
     * Foo obj;
     * obj.*(prop.m_member) = 420;
     *
     */
}

#endif
