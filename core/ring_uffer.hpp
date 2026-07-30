#pragma once

#include <cstddef>

namespace core
{

// Fixed-capacity FIFO container.
//
// RingBuffer stores elements in a circular buffer whose capacity
// is known at compile time. Memory is allocated statically and
// no dynamic allocation is performed.
//
// The container is intended for embedded applications where
// deterministic execution time and fixed memory usage are required.
//
// All operations execute in constant time.
//
// This implementation is not thread-safe. Concurrent access from
// interrupts and the main context must be synchronized externally.
template<
    class T,
    std::size_t Capacity>
class RingBuffer
{
    static_assert(
        Capacity > 0,
        "RingBuffer capacity must be greater than zero.");

public:

    RingBuffer() noexcept = default;

    // Inserts an element at the end of the buffer.
    //
    // Returns false if the buffer is full.
    bool push(const T& value) noexcept
    {
        if (size_ == Capacity)
        {
            return false;
        }

        buffer_[write_index_] = value;

        write_index_ = next_index(write_index_);

        ++size_;

        return true;
    }

    // Removes the oldest element from the buffer.
    //
    // Returns false if the buffer is empty.
    bool pop(T& value) noexcept
    {
        if (size_ == 0)
        {
            return false;
        }

        value = buffer_[read_index_];

        read_index_ = next_index(read_index_);

        --size_;

        return true;
    }

    // Resets the logical state of the buffer.
    //
    // Existing values remain stored internally and will be
    // overwritten by future push() operations.
    void clear() noexcept
    {
        read_index_ = 0;
        write_index_ = 0;
        size_ = 0;
    }

    // Returns the current number of stored elements.
    std::size_t size() const noexcept
    {
        return size_;
    }

    // Returns the maximum number of elements the buffer can hold.
    static constexpr std::size_t capacity() noexcept
    {
        return Capacity;
    }

private:

    // Advances an index and wraps around at the end of the buffer.
    static constexpr std::size_t next_index(
        std::size_t index) noexcept
    {
        return (index + 1) % Capacity;
    }

private:

    T buffer_[Capacity];

    // Index where the next element will be written.
    std::size_t write_index_ = 0;

    // Index of the next element to be read.
    std::size_t read_index_ = 0;

    // Current number of stored elements.
    std::size_t size_ = 0;
};

} // namespace core