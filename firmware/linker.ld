ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
  .vectors :
  {
    KEEP(*(.vectors))
  } > FLASH

  .text :
  {
    *(.text*)
  } > FLASH

  . = ORIGIN(RAM) + LENGTH(RAM);
  _stack_top = .;
}
