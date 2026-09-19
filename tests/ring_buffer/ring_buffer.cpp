#include "ring_buffer.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>

int main()
{
    {
        core::RingBuffer<int, 3> buffer;

        assert(buffer.size() == 0U);
        assert(buffer.capacity() == 3U);

        assert(buffer.push(1));
        assert(buffer.push(2));
        assert(buffer.push(3));

        assert(buffer.size() == 3U);
        assert(!buffer.push(4));

        int value = 0;

        assert(buffer.pop(value));
        assert(value == 1);

        assert(buffer.pop(value));
        assert(value == 2);

        assert(buffer.pop(value));
        assert(value == 3);

        assert(buffer.size() == 0U);
        assert(!buffer.pop(value));
    }

    {
        // Verify wraparound.
        core::RingBuffer<int, 3> buffer;

        int value = 0;

        assert(buffer.push(1));
        assert(buffer.push(2));
        assert(buffer.pop(value));
        assert(value == 1);

        assert(buffer.push(3));
        assert(buffer.push(4));

        assert(buffer.pop(value));
        assert(value == 2);

        assert(buffer.pop(value));
        assert(value == 3);

        assert(buffer.pop(value));
        assert(value == 4);

        assert(!buffer.pop(value));
    }

    {
        // Verify overwrite and newest ordering.
        core::RingBuffer<int, 3> buffer;

        buffer.push_overwrite(1);
        buffer.push_overwrite(2);
        buffer.push_overwrite(3);

        assert(buffer.size() == 3U);
        assert(buffer.from_newest(0U) == 3);
        assert(buffer.from_newest(1U) == 2);
        assert(buffer.from_newest(2U) == 1);

        buffer.push_overwrite(4);

        assert(buffer.size() == 3U);
        assert(buffer.from_newest(0U) == 4);
        assert(buffer.from_newest(1U) == 3);
        assert(buffer.from_newest(2U) == 2);
    }

    {
        // Verify clear.
        core::RingBuffer<int, 3> buffer;

        buffer.push(1);
        buffer.push(2);
        buffer.clear();

        assert(buffer.size() == 0U);

        int value = 0;
        assert(!buffer.pop(value));

        assert(buffer.push(3));
        assert(buffer.pop(value));
        assert(value == 3);
    }

    std::printf("RingBuffer tests: PASS\n");
    return 0;
}