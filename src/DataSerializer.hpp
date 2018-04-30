//
// Created by maxon on 30.04.18.
//

#ifndef UNIVERSALSTORAGE_DATASERIALIZER_HPP
#define UNIVERSALSTORAGE_DATASERIALIZER_HPP

#include <boost/endian/conversion.hpp>


namespace DataSerializer
{
    template <typename T>
    void serialize(const T &object, uint8_t** bytes)
    {
        auto endianed_val = boost::endian::native_to_little(object);
        *bytes = reinterpret_cast<uint8_t*>(&endianed_val);
    }

    template <typename T>
    void deserialize(T &object, const uint8_t* bytes)
    {
        auto endianed_val = reinterpret_cast<const T*>(bytes);
        return boost::endian::little_to_native(*endianed_val);
    }
}


#endif //UNIVERSALSTORAGE_DATASERIALIZER_HPP
