.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global SysTick_Handler
.global _stack_top

.section .vectors, "a"

/* Core exceptions */
.word _stack_top              /* 0: Initial Stack Pointer */
.word Reset_Handler + 1       /* 1: Reset */
.word 0                       /* 2: NMI */
.word 0                       /* 3: HardFault */
.word 0                       /* 4: MemManage */
.word 0                       /* 5: BusFault */
.word 0                       /* 6: UsageFault */
.word 0                       /* 7 */
.word 0                       /* 8 */
.word 0                       /* 9 */
.word 0                       /* 10 */
.word 0                       /* 11: SVCall */
.word 0                       /* 12: Debug */
.word 0                       /* 13 */
.word 0                       /* 14: PendSV */
.word SysTick_Handler + 1     /* 15: SysTick */

.section .text
.extern main

Reset_Handler:
    bl main
1:
    b 1b
