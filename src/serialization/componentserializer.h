#pragma once
#ifndef H_COMPONENT_SERIALIZER
#define H_COMPONENT_SERIALIZER

#include <string>
#include <sstream>
#include <filesystem>

//#include "../math/vec.h"
#include "../ecs/ecs.h"
#include "../ecs/entitymanager.h"
#include "../ecs/component.h"
#include "../ecs/reflection/reflection.h"
#include "serializable.h"

#define H_SERIALIZE_IMPL(class_name) \
void class_name::serialize(rapidjson::Document& root, rapidjson::Document::AllocatorType& alloc) { \
    hserializer::HJsonSerializer::Serialize(*this, root); \
}

#define H_DESERIALIZE_IMPL(class_name) \
void class_name::deserialize(const rapidjson::Value& root) { \
    hserializer::HJsonDeserializer::Deserialize(*this, root); \
}

namespace hserialization::hcomponentserializer {
void serializeComponent(ecs::Component* comp, rapidjson::Document& root, rapidjson::Document::AllocatorType& alloc);
void deserializeComponent(ecs::Entity* entity, const char* compName, const rapidjson::Value& root);
}

namespace hserializer {
    
/*
 * SERIALIZER
 */
    
struct HSerializer {
    template<typename classtype, typename memtype>
    static void Resolve(const hreflection::SProperty<classtype, memtype>& prop,
            const classtype& obj, rapidjson::Document& root) {
        if (root.IsNull()) root.SetObject();
        HSerializerType<memtype>::Resolve(prop.name, obj.*(prop.member), root);
    }
    
    template<typename T, typename Enable = void>
    struct HSerializerType {
        static void Resolve(const char* name, const T& value, rapidjson::Document& root) {}
        static void SerialToJsonArr(rapidjson::Value& root, const T& value, rapidjson::Document::AllocatorType& alloc) {}
    };
};

template <typename T>
struct HSerializer::HSerializerType<T, typename std::enable_if<
    std::is_integral<T>::value || std::is_floating_point<T>::value>::type> {
    static void Resolve(const char* name, const T& value, rapidjson::Document& root) {
        root.AddMember(rapidjson::GenericStringRef<char>(name), value, root.GetAllocator());
    }
    static void SerialToJsonArr(rapidjson::Value& root, const T& value, rapidjson::Document::AllocatorType& alloc) {
        root.PushBack(value, alloc);
    }
};

template <typename T>
struct HSerializer::HSerializerType<T, typename std::enable_if<std::is_enum<T>::value>::type> {
    static void Resolve(const char* name, const T& value, rapidjson::Document& root) {
        root.AddMember(rapidjson::GenericStringRef<char>(name), static_cast<std::int32_t>(value), root.GetAllocator());
    }
    static void SerialToJsonArr(rapidjson::Value& root, const T& value, rapidjson::Document::AllocatorType& alloc) {
        root.PushBack(static_cast<std::int32_t>(value), alloc);
    }
};

template <>
struct HSerializer::HSerializerType<std::string> {
    static void Resolve(const char* name, const std::string& value, rapidjson::Document& root) {
        root.AddMember(rapidjson::GenericStringRef<char>(name), rapidjson::GenericStringRef<char>(value.c_str()), root.GetAllocator());
    }
    static void SerialToJsonArr(rapidjson::Value& root, const std::string& value, rapidjson::Document::AllocatorType& alloc) {
        root.PushBack(rapidjson::GenericStringRef<char>(value.c_str()), alloc); 
    }
};

template <>
struct HSerializer::HSerializerType<std::filesystem::path> {
    static void Resolve(const char* name, const std::filesystem::path& value, rapidjson::Document& root) {
        root.AddMember(rapidjson::GenericStringRef<char>(name), rapidjson::GenericStringRef<char>(value.string().c_str()), root.GetAllocator());
    }
    static void SerialToJsonArr(rapidjson::Value& root, const std::filesystem::path& value, rapidjson::Document::AllocatorType& alloc) {
        root.PushBack(rapidjson::GenericStringRef<char>(value.string().c_str()), alloc); 
    }
};

// these types are not included in this repo
// you may add your own types here to define how to serialize/deserialize them
/*
template <>
struct HSerializer::HSerializerType<vec2i> {
    static void Resolve(const char* name, const vec2i& value, rapidjson::Document& root) {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(value.x, root.GetAllocator());
        arr.PushBack(value.y, root.GetAllocator());
        root.AddMember(rapidjson::GenericStringRef<char>(name), arr, root.GetAllocator());
    }
    
    static void SerialToJsonArr(rapidjson::Value& root, const vec2i& value, rapidjson::Document::AllocatorType& alloc) {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(value.x, alloc);
        arr.PushBack(value.y, alloc);
        root.PushBack(arr, alloc);
    }
};

template <>
struct HSerializer::HSerializerType<vec3f> {
    static void Resolve(const char* name, const vec3f& value, rapidjson::Document& root) {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(value.x, root.GetAllocator());
        arr.PushBack(value.y, root.GetAllocator());
        arr.PushBack(value.z, root.GetAllocator());
        root.AddMember(rapidjson::GenericStringRef<char>(name), arr, root.GetAllocator());
    }
    
    static void SerialToJsonArr(rapidjson::Value& root, const vec3f& value, rapidjson::Document::AllocatorType& alloc) {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(value.x, alloc);
        arr.PushBack(value.y, alloc);
        arr.PushBack(value.z, alloc);
        root.PushBack(arr, alloc);
    }
};

template <>
struct HSerializer::HSerializerType<vec4f> {
    static void Resolve(const char* name, const vec4f& value, rapidjson::Document& root) {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(value.x, root.GetAllocator());
        arr.PushBack(value.y, root.GetAllocator());
        arr.PushBack(value.z, root.GetAllocator());
        arr.PushBack(value.w, root.GetAllocator());
        root.AddMember(rapidjson::GenericStringRef<char>(name), arr, root.GetAllocator());
    }
    
    static void SerialToJsonArr(rapidjson::Value& root, const vec4f& value, rapidjson::Document::AllocatorType& alloc) {
        rapidjson::Value arr(rapidjson::kArrayType);
        arr.PushBack(value.x, alloc);
        arr.PushBack(value.y, alloc);
        arr.PushBack(value.z, alloc);
        arr.PushBack(value.w, alloc);
        root.PushBack(arr, alloc);
    }
};
*/

template <typename T>
struct HSerializer::HSerializerType<T, typename std::enable_if<std::is_base_of<ISerializable, T>::value>::type> {
    static void Resolve(const char* name, const T& value, rapidjson::Document& root) {
        rapidjson::Document doc;
        H_APPLY_RESOLVER_TO_CONST(value, HSerializer, doc)
        rapidjson::Value newval(doc, root.GetAllocator());
        root.AddMember(rapidjson::GenericStringRef<char>(name), newval, root.GetAllocator());
    }
    
    static void SerialToJsonArr(rapidjson::Value& root, const T& value, rapidjson::Document::AllocatorType& alloc) {
        rapidjson::Document doc;
        H_APPLY_RESOLVER_TO_CONST(value, HSerializer, doc)
        rapidjson::Value newval(doc, alloc);
        root.PushBack(newval, alloc);
    }
};

template <template<typename, typename, typename...> typename TT, typename T1, typename T2, typename... Args>
struct HSerializer::HSerializerType<TT<T1, T2, Args...>> {
    static void Resolve(const char* name, const TT<T1, T2, Args...>& value, rapidjson::Document& root) {
        rapidjson::Value arr(rapidjson::kArrayType);
        for (const auto& [key, val] : value) {
            rapidjson::Value elem(rapidjson::kArrayType);
            HSerializer::HSerializerType<T1>::SerialToJsonArr(elem, key, root.GetAllocator());
            HSerializer::HSerializerType<T2>::SerialToJsonArr(elem, val, root.GetAllocator());
            arr.PushBack(elem, root.GetAllocator()); 
        }
        root.AddMember(rapidjson::GenericStringRef<char>(name), arr, root.GetAllocator());
    }
    
    static void SerialToJsonArr(rapidjson::Value& root, const TT<T1, T2, Args...>& value, rapidjson::Document::AllocatorType& alloc) {
        rapidjson::Value arr(rapidjson::kArrayType);
        for (const auto& [key, val] : value) {
            rapidjson::Value elem(rapidjson::kArrayType);
            HSerializer::HSerializerType<T1>::SerialToJsonArr(elem, key, alloc);
            HSerializer::HSerializerType<T2>::SerialToJsonArr(elem, val, alloc);
            arr.PushBack(elem, alloc); 
        }
        root.PushBack(arr, alloc);
    }
};

struct HJsonSerializer {
    template<class T>
    static void Serialize(T& obj, rapidjson::Document& root) {
        rapidjson::Document doc(&root.GetAllocator());
        doc.SetObject();
        H_APPLY_RESOLVER(obj, HSerializer, doc)
        const auto n = SReflectionProperties<T>::s_className;
        root.AddMember(rapidjson::GenericStringRef<char>(n), doc, root.GetAllocator());
    }
    
    template<class T>
    static void Serialize(const T& obj, rapidjson::Document& root) {
        rapidjson::Document doc(&root.GetAllocator());
        doc.SetObject();
        H_APPLY_RESOLVER_TO_CONST(obj, HSerializer, root)
        const auto n = SReflectionProperties<T>::s_className;
        root.AddMember(rapidjson::GenericStringRef<char>(n), doc, root.GetAllocator());
    }
};

/*
 * DESERIALIZER
 */

struct HDeserializer {
    template<typename classtype, typename memtype>
    static void Resolve(const hreflection::SProperty<classtype, memtype>& prop,
            classtype& obj, const rapidjson::Value& root) {
        if (root.HasMember(prop.name)) {
        HDeserializerType<memtype>::Resolve(obj.*(prop.member), root[prop.name]);
        }
    }
    
    template<typename T, typename Enable = void>
    struct HDeserializerType {
        static void Resolve(T& value, const rapidjson::Value& root) {}
    };
};

template <>
struct HDeserializer::HDeserializerType<std::int32_t> {
    static void Resolve(std::int32_t& value, const rapidjson::Value& root) {
        if (root.IsInt()) {
            value = root.GetInt();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<std::uint32_t> {
    static void Resolve(std::uint32_t& value, const rapidjson::Value& root) {
        if (root.IsUint()) {
            value = root.GetUint();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<std::int64_t> {
    static void Resolve(std::int64_t& value, const rapidjson::Value& root) {
        if (root.IsInt64()) {
            value = root.GetInt64();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<std::uint64_t> {
    static void Resolve(std::uint64_t& value, const rapidjson::Value& root) {
        if (root.IsUint64()) {
            value = root.GetUint64();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<float> {
    static void Resolve(float& value, const rapidjson::Value& root) {
        if (root.IsFloat()) {
            value = root.GetFloat();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<double> {
    static void Resolve(double& value, const rapidjson::Value& root) {
        if (root.IsDouble()) {
            value = root.GetDouble();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<bool> {
    static void Resolve(bool& value, const rapidjson::Value& root) {
        if (root.IsBool()) {
            value = root.GetBool();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<std::string> {
    static void Resolve(std::string& value, const rapidjson::Value& root) {
        if (root.IsString()) {
            value = root.GetString();
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<std::filesystem::path> {
    static void Resolve(std::filesystem::path& value, const rapidjson::Value& root) {
        if (root.IsString()) {
            value = std::filesystem::path(root.GetString());
        }
    }
};

// these types are not included in this repo
// you may add your own types here to define how to serialize/deserialize them
/*
template <>
struct HDeserializer::HDeserializerType<vec2i> {
    static void Resolve(vec2i& value, const rapidjson::Value& root) {
        if (root.IsArray()) {
            const auto arr = root.GetArray();
            HDeserializer::HDeserializerType<decltype(value.x)>::Resolve(value.x, arr[0]);
            HDeserializer::HDeserializerType<decltype(value.y)>::Resolve(value.y, arr[1]);
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<vec3f> {
    static void Resolve(vec3f& value, const rapidjson::Value& root) {
        if (root.IsArray()) {
            const auto arr = root.GetArray();
            HDeserializer::HDeserializerType<decltype(value.x)>::Resolve(value.x, arr[0]);
            HDeserializer::HDeserializerType<decltype(value.y)>::Resolve(value.y, arr[1]);
            HDeserializer::HDeserializerType<decltype(value.z)>::Resolve(value.z, arr[2]);
        }
    }
};

template <>
struct HDeserializer::HDeserializerType<vec4f> {
    static void Resolve(vec4f& value, const rapidjson::Value& root) {
        if (root.IsArray()) {
            const auto arr = root.GetArray();
            HDeserializer::HDeserializerType<decltype(value.x)>::Resolve(value.x, arr[0]);
            HDeserializer::HDeserializerType<decltype(value.y)>::Resolve(value.y, arr[1]);
            HDeserializer::HDeserializerType<decltype(value.z)>::Resolve(value.z, arr[2]);
            HDeserializer::HDeserializerType<decltype(value.z)>::Resolve(value.w, arr[3]);
        }
    }
};
*/

template <typename T>
struct HDeserializer::HDeserializerType<T, typename std::enable_if<std::is_base_of<ISerializable, T>::value>::type> {
    static void Resolve(T& value, const rapidjson::Value& root) {
        H_APPLY_RESOLVER(value, HDeserializer, root)
    }
};

template <template<typename, typename, typename...> typename TT, typename T1, typename T2, typename... Args>
struct HDeserializer::HDeserializerType<TT<T1, T2, Args...>> {
    static void Resolve(TT<T1, T2>& value, const rapidjson::Value& root) {
        if (root.IsArray()) {
            for (auto& arr : root.GetArray()) {
                T1 key;
                HDeserializer::HDeserializerType<T1>::Resolve(key, arr[0]);
                T2 val;
                HDeserializer::HDeserializerType<T2>::Resolve(val, arr[1]);
                value[key] = val;
            }
        }
    }
};

struct HJsonDeserializer {
    template<class T>
    static void Deserialize(T& obj, const rapidjson::Value& root) {
        H_APPLY_RESOLVER(obj, HDeserializer, root)
    }
    
    template<class T>
    static void Deserialize(const T& obj, const rapidjson::Value& root) {
        H_APPLY_RESOLVER_TO_CONST(obj, HDeserializer, root)
    }
};
}

#endif
