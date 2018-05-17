//
// Created by maxon on 14.05.18.
//

#ifndef UNIVERSALSTORAGE_FUNCTION_CHECKING_HPP
#define UNIVERSALSTORAGE_FUNCTION_CHECKING_HPP

#include <vector>
#include <boost/hana.hpp>

namespace UniversalStorage {
    namespace TypeTraits {

        template <typename T>
        std::vector<uint8_t> serialize(const T &) = delete;

        template <typename T>
        T deserialize(const std::vector<uint8_t> &) = delete;


        // Check if T has method serialize with given signature
        template<typename T>
        class has_serialize_method
        {
            template<class U, typename std::enable_if<std::is_same<std::vector<uint8_t>(U::*)() const, decltype(&U::serialize)>::value>::type...>
            static std::true_type check(int) {};

            template<class>
            static std::false_type check(...) {};
        public:
            static constexpr bool value = decltype(check<T>(0))::value;
        };

        // Check if T has method deserialize with given signature
        template<typename T>
        class has_deserialize_method
        {
            template<class U, typename std::enable_if<std::is_same<T(*)(
                    const std::vector<uint8_t> &), decltype(&U::deserialize)>::value>::type...>
            static std::true_type check(int) {};

            template<class>
            static std::false_type check(...) {};
        public:
            static constexpr bool value = decltype(check<T>(0))::value;
        };


        // Check if where is specialization of method serialize for given T
        template <class T>
        using has_serilize_t = decltype(serialize(std::declval<T>()));

        template <typename T, typename = void>
        struct has_serialize_spec { static constexpr bool value = false; };

        template <typename T>
        struct has_serialize_spec<T, std::void_t<has_serilize_t<T>>> { static constexpr bool value = true; };

        // Check if where is specialization of method deserialize for given T
        template <class T>
        using has_deserilize_t = decltype(deserialize<T>(std::declval<const std::vector<uint8_t>&>()));

        template <typename T, typename = void>
        struct has_deserialize_spec { static constexpr bool value = false; };

        template <typename T>
        struct has_deserialize_spec<T, std::void_t<has_deserilize_t<T>>> { static constexpr bool value = true; };


        // Some simplifications
        template <typename T>
        struct can_serialize
        {
            static constexpr bool value = has_serialize_method<T>::value || has_serialize_spec<T>::value;
        };

        template <typename T>
        struct can_deserialize
        {
            static constexpr bool value = has_deserialize_method<T>::value || has_deserialize_spec<T>::value;
        };
    }
}

#endif //UNIVERSALSTORAGE_FUNCTION_CHECKING_HPP
