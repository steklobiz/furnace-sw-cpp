#pragma once

#include <cstddef>

namespace core
{

// Fixed-capacity FIFO container.
//
// RingBuffer stores elements in a statically allocated circular buffer.
// No dynamic allocation is performed.
//
// push() and pop() support a single-producer/single-consumer usage,
// such as an interrupt producer and a main-context consumer.
//
// The implementation reserves one internal slot to distinguish
// full and empty states, so the public capacity remains exactly Capacity.
//
// push_overwrite(), clear(), from_newest(), and size() are intended
// for single-context use and are not part of the ISR/main-context
// producer/consumer contract.
//
// No general thread-safety or multi-producer/multi-consumer support
// is provided.
template<
    class T,
    std::size_t Capacity>
class RingBuffer
{
    static_assert(Capacity > 0, "RingBuffer capacity must be greater than zero.");

    static constexpr std::size_t StorageSize = Capacity + 1U;

public:
    RingBuffer() noexcept = default;

    // Adds an element.
    //
    // SPSC-safe when called only by the producer.
    // Returns false when the buffer is full.
    bool push(const T& value) noexcept
    {
        const std::size_t next_write = next_index(write_index_);

        if (next_write == read_index_)
        {
            return false;
        }

        buffer_[write_index_] = value;
        write_index_ = next_write;

        return true;
    }

    // Adds an element, discarding the oldest element when full.
    //
    // Single-context operation.
    bool push_overwrite(const T& value) noexcept
    {
        const std::size_t next_write = next_index(write_index_);

        if (next_write == read_index_)
        {
            read_index_ = next_index(read_index_);
        }

        buffer_[write_index_] = value;
        write_index_ = next_write;

        return true;
    }

    // Removes the oldest element.
    //
    // SPSC-safe when called only by the consumer.
    // Returns false when the buffer is empty.
    bool pop(T& value) noexcept
    {
        if (read_index_ == write_index_)
        {
            return false;
        }

        value = buffer_[read_index_];
        read_index_ = next_index(read_index_);

        return true;
    }

    // Clears the buffer.
    //
    // Single-context operation.
    void clear() noexcept
    {
        read_index_ = 0U;
        write_index_ = 0U;
    }

    // Returns an element relative to the newest element.
    //
    // index == 0 refers to the newest element.
    // Caller must provide index < size().
    //
    // Single-context operation.
    const T& from_newest(std::size_t index) const noexcept
    {
        const std::size_t idx =
            (write_index_ + StorageSize - 1U - index) % StorageSize;

        return buffer_[idx];
    }

    // Returns the current number of stored elements.
    //
    // This is a snapshot and is intended for single-context/history use.
    std::size_t size() const noexcept
    {
        if (write_index_ >= read_index_)
        {
            return write_index_ - read_index_;
        }

        return StorageSize - read_index_ + write_index_;
    }

    static constexpr std::size_t capacity() noexcept
    {
        return Capacity;
    }

private:
    static constexpr std::size_t next_index(std::size_t index) noexcept
    {
        return (index + 1U) % StorageSize;
    }

    T buffer_[StorageSize];

    // These indices may be accessed from main context and an ISR.
    volatile std::size_t write_index_ = 0U;
    volatile std::size_t read_index_ = 0U;
};

} // namespace core