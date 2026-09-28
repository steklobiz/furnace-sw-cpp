// Startup code: the very first code that runs after CPU reset.
//
// When the Cortex-M0+ powers on or resets:
//   1. Hardware loads the initial stack pointer from address 0x08000000
//      (the first entry in the vector table → _estack)
//   2. Hardware loads the reset vector from address 0x08000004
//      (the second entry → Reset_Handler)
//   3. CPU starts executing Reset_Handler
//
// Reset_Handler must:
//   a) Copy .data from flash to RAM  — initialized globals/statics live
//      in flash initially but must be in RAM at runtime (writable).
//   b) Zero .bss                      — uninitialized globals/statics
//      must start at 0 per the C/C++ standard.
//   c) Run static constructors        — C++ globals (like Register objects)
//      need their constructors called before main().
//   d) Call main()                    — the user application.
//   e) Halt if main returns.

#include <cstdint>
#include "systick.hpp"
// #include "board.hpp"

// These symbols are defined in the linker script (stm32g030f6p6.ld).
// They mark the boundaries of memory sections in the linked binary.
// We declare them as extern uint32_t and take their address.

extern uint32_t _estack;          // End of RAM = initial SP (top of stack)
extern uint32_t _sidata;          // Start of .data init values in flash
extern uint32_t _sdata;           // Start of .data section in RAM
extern uint32_t _edata;           // End of .data section in RAM
extern uint32_t _sbss;            // Start of .bss section in RAM
extern uint32_t _ebss;            // End of .bss section in RAM
extern void (*__init_array_start)(void);  // First constructor pointer
extern void (*__init_array_end)(void);    // After last constructor pointer

extern int main();

extern "C" {
// All functions here use C linkage so the symbol names match the
// vector table entries. C++ name-mangling would produce different
// names (e.g. _Z7myfuncv) that the linker can't resolve.

// Called directly by the CPU after reset.
// This is the true entry point of the firmware.

__attribute__((used))
void Reset_Handler() {
    // Copy .data from flash (LMA) to RAM (VMA).
    for (uint32_t* src = &_sidata, *dst = &_sdata; dst < &_edata; *dst++ = *src++);

    // Zero-fill .bss (uninitialized globals/statics).
    for (uint32_t* dst = &_sbss; dst < &_ebss; *dst++ = 0);

    // Call C++ static constructors.
    for (void (**init)(void) = &__init_array_start; init < &__init_array_end; (*init++)());

    // enable global interrupts
    __asm("cpsie i");
	    

    // Jump to the application.
    main();

    // If main() returns, loop forever.
    for (;;);
}

// SysTick interrupt — increments the system tick counter every 1 ms.

// extern volatile uint32_t system_tick;

void SysTick_Handler(void)
{
//	drivers::systick::irq_handler();
}

void USART2_IRQHandler(void)
{
//    board::lcd_uart_buf.irq_handler();
}

// Trap for unhandled interrupts. Just halts the CPU.
__attribute__((used))
void Default_Handler() {
    for (;;);
}

// Weak aliases for NMI and HardFault.
// If the user defines their own NMI_Handler() anywhere, it overrides these.
// If not, the weak alias redirects to Default_Handler (which halts).

void NMI_Handler()       __attribute__((nothrow, weak, alias("Default_Handler")));
void HardFault_Handler() __attribute__((nothrow, weak, alias("Default_Handler")));



// Vector table: placed at flash base (0x08000000) by the linker script.
// Cortex-M0+ expects this exact layout:
//   [0]  Initial SP value  → _estack
//   [1]  Reset_Handler
//   [2]  NMI_Handler
//   [3]  HardFault_Handler
//   [4-10] Reserved (must be 0 on M0+)
//   [11] SVCall
//   [12-13] Reserved
//   [14] PendSV
//   [15] SysTick
//   [16+] Peripheral IRQs
//
// The (void*) casts are required because C++ forbids implicit conversion
// from function pointers to void pointers.

#define DH Default_Handler

__attribute__((used, section(".isr_vector")))
const void* const vector_table[] = {
    // System exceptions (positions 0-15)
    (void*)&_estack,           //  0: Initial SP
    (void*)Reset_Handler,      //  1: Reset
    (void*)NMI_Handler,        //  2: NMI
    (void*)HardFault_Handler,  //  3: HardFault
    (void*)0, (void*)0, (void*)0, (void*)0, (void*)0, (void*)0, (void*)0, // 4-10: Reserved
    (void*)DH,                 // 11: SVCall
    (void*)0, (void*)0,        // 12-13: Reserved
    (void*)DH,                 // 14: PendSV
    (void*)SysTick_Handler,    // 15: SysTick

    // Peripheral interrupts (positions 0-31 from Table 45)
    (void*)DH,  // 16: Position 0 - WWDG
    (void*)0,   // 17: Position 1 - Reserved
    (void*)DH,  // 18: Position 2 - RTC/TAMP
    (void*)DH,  // 19: Position 3 - FLASH
    (void*)DH,  // 20: Position 4 - RCC
    (void*)DH,  // 21: Position 5 - EXTI0_1
    (void*)DH,  // 22: Position 6 - EXTI2_3
    (void*)DH,  // 23: Position 7 - EXTI4_15
    (void*)0,   // 24: Position 8 - Reserved
    (void*)DH,  // 25: Position 9 - DMA1_Channel1
    (void*)DH,  // 26: Position 10 - DMA1_Channel2_3
    (void*)DH,  // 27: Position 11 - DMA1_Channel4_5_6_7/DMAMUX/DMA2
    (void*)DH,  // 28: Position 12 - ADC
    (void*)DH,  // 29: Position 13 - TIM1_BRK_UP_TRG_COM
    (void*)DH,  // 30: Position 14 - TIM1_CC
    (void*)0,   // 31: Position 15 - Reserved
    (void*)DH,  // 32: Position 16 - TIM3+TIM4
    (void*)DH,  // 33: Position 17 - TIM6
    (void*)DH,  // 34: Position 18 - TIM7
    (void*)DH,  // 35: Position 19 - TIM14
    (void*)DH,  // 36: Position 20 - TIM15
    (void*)DH,  // 37: Position 21 - TIM16
    (void*)DH,  // 38: Position 22 - TIM17
    (void*)DH,  // 39: Position 23 - I2C1
    (void*)DH,  // 40: Position 24 - I2C2/I2C3
    (void*)DH,  // 41: Position 25 - SPI1
    (void*)DH,  // 42: Position 26 - SPI2/SPI3
    (void*)DH, // 43: Position 27 - USART1_IRQHandler
    (void*)USART2_IRQHandler,  // 44: Position 28 - USART2_IRQHandler
    (void*)DH,  // 45: Position 29 - USART3/4/5/6
    (void*)0,   // 46: Position 30 - Reserved
    (void*)0,   // 47: Position 31 - Reserved    
};


} // extern "C"
