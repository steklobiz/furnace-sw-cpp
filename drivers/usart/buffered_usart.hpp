// drivers/usart/buffered_usart.hpp

#pragma once

#include <cstdint>
#include "usart.hpp"
#include "ring_buf.hpp"

namespace drivers {

class BufferedUsart {
public:
    BufferedUsart(Usart& usart, core::RingBuf& rx_buffer)
        : usart_(usart), 
        rx_buffer_(rx_buffer)
    {
    }

    void irq_handler()
    {
        if (usart_.readable())
        {
            rx_buffer_.push(usart_.read());
        }
    }
    
    bool read(uint8_t& byte)
    {
        return rx_buffer_.pop(byte);
    }

private:
    Usart& usart_;
    core::RingBuf& rx_buffer_;
};

} // namespace drivers