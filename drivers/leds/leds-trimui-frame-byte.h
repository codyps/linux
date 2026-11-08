/* SPDX-License-Identifier: GPL-2.0-only */
/* Keep the OEM scale/blue/red/green byte-observation order without volatile
 * QI operands that GCC7 combine refuses to fold into zero-extending loads.
 * These empty compiler barriers emit no target instructions or CPU fences.
 * The u32 input captures the widened value before the next memory boundary.
 * No atomic or fault-atomicity contract is added. The count remains unchecked.
 */
#define TRIMUI_FRAME_BYTE(address) ({ \
 u32 __trimui_byte; \
 asm volatile("" : : : "memory"); \
 __trimui_byte = *(const u8 *)(address); \
 asm volatile("" : : "r"(__trimui_byte) : "memory"); \
 __trimui_byte; \
})
static void commit_static_frame(void) __asm__("commit_static_frame.constprop.16"); /* OEM clone name, not original C. */
/* Stock joins the masked middle channel with arithmetic blue before the
 * shifted high channel. This input observation retains that partial value;
 * it emits no instruction, memory access or CPU fence. Not original-source
 * provenance. Preserve unchecked counts and signed blue as shipped.
 */
#define TRIMUI_PACK_CAPTURE(value) ({ u32 __trimui_pack=(value); asm volatile("" : : "r"(__trimui_pack)); __trimui_pack; })
