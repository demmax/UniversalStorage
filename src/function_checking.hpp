//
// Created by maxon on 14.05.18.
//

#ifndef UNIVERSALSTORAGE_FUNCTION_CHECKING_HPP
#define UNIVERSALSTORAGE_FUNCTION_CHECKING_HPP

#include <vector>

namespace UniversalStorage {
    namespace TypeTraits {

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

//        template<typename T>
//        class has_serialize_free_function
//        {
//            template<class U, typename std::enable_if<std::is_same<std::vector<uint8_t>(*)(
//                    const U &), decltype(&UniversalStorage::TypeTraits::serialize)>::value>::type...>
//            static std::true_type check(int) {};
//
//            template<class>
//            static std::false_type check(...) {};
//        public:
//            static constexpr bool value = decltype(check<T>(0))::value;
//        };


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

    }
}

#endif //UNIVERSALSTORAGE_FUNCTION_CHECKING_HPP
