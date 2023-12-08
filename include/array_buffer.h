// Copyright (c) 2023 Ture Fronczek-Munter
//
// This file is part of the BufferLibCpp library
// Please visit: https://github.com/ture-f-munter/BufferLibCpp
//
// MIT License
#pragma once

#include "bufferlib_common.h"

#include <array>
#include <iomanip>
#include <ios>
#include <string>
#include <stdint.h>
#include <type_traits>

namespace BufferLib
{
    template<typename TStorageDataType = std::uint8_t, int TCapacity = 512>
    class array_buffer
    {
        typedef TStorageDataType Storage_DataType;
        typedef std::array<TStorageDataType, TCapacity> Storage_Container;
        //typedef typename Storage_Container::size_type size_type_t;
        typedef int size_type_t;

    public:
        array_buffer();

        [[nodiscard]] constexpr size_type_t capacity() const noexcept;    // Returns total capacity of buffer in bytes.
        [[nodiscard]] size_type_t size() const noexcept;                  // Returns number of elements ready to be read.
        [[nodiscard]] size_type_t free_size() const noexcept;             // Returns size of write buffer.

        size_type_t set_min_free(size_type_t n_elem) noexcept;   // Set the number of elements that must be free in buffer. Returns -1 if failed, else returns n_elem.
        size_type_t consume(size_type_t n_elem) noexcept;        // Mark n_elem as consumed. Returns -1 if failed, else returns n_elem.
        size_type_t commit(size_type_t n_elem) noexcept;         // Commit n_elem to buffer. Returns -1 if failed, else returns n_elem.

        [[nodiscard]] const Storage_DataType* read_ptr() const noexcept;
        [[nodiscard]] Storage_DataType* write_ptr() noexcept;
        [[nodiscard]] Storage_DataType* raw_ptr() noexcept;
        [[nodiscard]] const Storage_DataType* const_raw_ptr() const noexcept;

        void to_stream(std::ostream& o) const noexcept;

    private:
        void relocate() noexcept;

        Storage_Container m_buf;                    // Buffer
        const size_type_t m_capacity = TCapacity;   // Total capacity of buffer
        size_type_t m_read_index;                   // Index for reading.
        size_type_t m_write_index;                  // Index for writing.
        size_type_t m_min_free;                     // Minimum free bytes in buffer before a relocation of remaining data.
    };

    template<typename TStorageDataType, int TCapacity>
    array_buffer<TStorageDataType, TCapacity>::array_buffer()
        : m_read_index(0)
        , m_write_index(0)
        , m_min_free(TCapacity / 4)
    {
        static_assert(std::is_arithmetic_v<TStorageDataType>, "TStorageDataType is not an arithmetic type");

        memset(&m_buf[0], 0, TCapacity * sizeof(TStorageDataType));
    }

    template<typename TStorageDataType, int TCapacity>
    constexpr typename array_buffer<TStorageDataType, TCapacity>::size_type_t array_buffer<TStorageDataType, TCapacity>::capacity() const noexcept
    {
        return m_buf.size();
    }

    template<typename TStorageDataType, int TCapacity>
    typename array_buffer<TStorageDataType, TCapacity>::size_type_t array_buffer<TStorageDataType, TCapacity>::size() const noexcept
    {
        return m_write_index - m_read_index;
    }

    template<typename TStorageDataType, int TCapacity>
    typename array_buffer<TStorageDataType, TCapacity>::size_type_t array_buffer<TStorageDataType, TCapacity>::free_size() const noexcept
    {
        return capacity() - m_write_index;
    }

    template<typename TStorageDataType, int TCapacity>
    typename array_buffer<TStorageDataType, TCapacity>::size_type_t array_buffer<TStorageDataType, TCapacity>::set_min_free(size_type_t n_elem) noexcept
    {
        if (n_elem > (TCapacity / 2))
        {
            // Setting min_free to half the buffer capacity
            m_min_free = TCapacity / 2;
            return m_min_free;
        }
        
        m_min_free = n_elem;
        return m_min_free;
    }

    template<typename TStorageDataType, int TCapacity>
    typename array_buffer<TStorageDataType, TCapacity>::size_type_t  array_buffer<TStorageDataType, TCapacity>::consume(size_type_t n_elem) noexcept
    {
        if (m_read_index + n_elem > capacity())
        {
	        // Too many elements consumed
            return -1;
        }

        if (m_read_index + n_elem > m_write_index)
        {
            // Trying to consume past the write pointer
            return -1;
        }

        m_read_index += n_elem;
        if (free_size() < m_min_free)
        {
            // Trigger a relocate
            relocate();
        }

        return n_elem;
    }

    template<typename TStorageDataType, int TCapacity>
    typename array_buffer<TStorageDataType, TCapacity>::size_type_t  array_buffer<TStorageDataType, TCapacity>::commit(size_type_t n_elem) noexcept
    {
        if (n_elem > free_size())
        {
            // Commiting more elements than there is free
            return -1;
        }

        m_write_index += n_elem;
        if (free_size() < m_min_free)
        {
            // Trigger a relocate
            relocate();
        }

        return n_elem;
    }

    template<typename TStorageDataType, int TCapacity>
    const TStorageDataType* array_buffer<TStorageDataType, TCapacity>::read_ptr() const noexcept
    {
        return &m_buf[m_read_index];
    }

    template<typename TStorageDataType, int TCapacity>
    TStorageDataType* array_buffer<TStorageDataType, TCapacity>::write_ptr() noexcept
    {
        return &m_buf[m_write_index];
    }

    template<typename TStorageDataType, int TCapacity>
    TStorageDataType* array_buffer<TStorageDataType, TCapacity>::raw_ptr() noexcept
    {
        return &m_buf[0];
    }

    template<typename TStorageDataType, int TCapacity>
    const TStorageDataType* array_buffer<TStorageDataType, TCapacity>::const_raw_ptr() const noexcept
    {
        return &m_buf[0];
    }

    template<typename TStorageDataType, int TCapacity>
    void array_buffer<TStorageDataType, TCapacity>::relocate() noexcept
    {
        if (free_size() > capacity() / 2)
        {
            // More than half the buffer still free, do nothing
            return;
        }

        memcpy(&m_buf[0], &m_buf[m_read_index], size());
        m_write_index = m_write_index - m_read_index;
        m_read_index = 0;
        memset(&m_buf[m_write_index], 0, free_size());   // Clear rest of buffer
    }

    template<typename TStorageDataType, int TCapacity>
    void array_buffer<TStorageDataType, TCapacity>::to_stream(std::ostream& o) const noexcept
    {
        const auto write_index = TCapacity - free_size();
        const auto read_index = write_index - size();

        for (size_type_t i = 0; i < TCapacity; ++i)
        {
            if(std::is_integral_v<TStorageDataType>)
            {
                // Type is an integer type, write as hex
                o << std::setw(2 * sizeof(Storage_DataType)) << std::setfill('0') << std::hex << static_cast<std::uint64_t>(*(const_raw_ptr() + i));    
            }
            else
            {
                // Type is not an integer type, call std::to_string()
                o << std::setw(2 * sizeof(Storage_DataType)) << std::setfill('0') << std::hex << std::to_string(*(const_raw_ptr() + i));
            }
            

            if (i == read_index && i == write_index)
            {
                o << std::string("<-rw ");
            }
            else if (i == read_index)
            {
                o << std::string("<-r  ");
            }
            else if (i == write_index)
            {
                o << std::string("<-w  ");
            }
            else
            {
                o << std::string("     ");
            }

            if ((i + 1) % 8 == 0 && i > 0)
            {
                o << std::string(BUFFERLIB_NEWLINE);
            }
        }
    }

    template<typename TStorageDataType, int TCapacity>
    std::ostream& operator<< (std::ostream& o, const array_buffer<TStorageDataType, TCapacity>& buf)
    {
        buf.to_stream(o);
        return o;
    }
}
