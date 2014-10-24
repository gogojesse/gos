.global __gos_vectors
__gos_vectors:
  b _gos_reset_handler /* Reset */
  b _gos_undfd_handler /* Undefined */
  b .                  /* SWI */
  b .                  /* Prefetch Abort */
  b .                  /* Data Abort */
  b .                  /* reserved */
  b .                  /* IRQ */
  b .                  /* FIQ */

