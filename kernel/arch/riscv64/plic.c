#include "plic.h"
#include <stdint.h>

void plic_init(void)
{
  *(uint32_t*)(PLIC + UART0_IRQ*4) = 1;
}

void plic_init_hart(int hart_id)
{
  *(uint32_t*)PLIC_SENABLE(hart_id) = (1 << UART0_IRQ);
  *(uint32_t*)PLIC_SPRIORITY(hart_id) = 0;
}

int plic_claim(int hart_id)
{
  int irq = *(uint32_t*)PLIC_SCLAIM(hart_id);
  return irq;
}

void plic_complete(int irq,int hart_id)
{
  *(uint32_t*)PLIC_SCLAIM(hart_id) = irq;
}
