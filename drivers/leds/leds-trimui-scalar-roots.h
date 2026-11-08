/* SPDX-License-Identifier: GPL-2.0-only */
/* Address-only GNU C bindings to real frame members and the device slot. No storage
 * is allocated. Numeric bases reach neighboring effect fields only under
 * the verified stock BSS layout; no before-object C pointer is formed.
 * Original OEM declaration spelling is unknown.
 */
extern u32 trimui_middle_member_root __asm__("trimui_effect_l+1400");
extern u32 trimui_left_member_root __asm__("trimui_effect_r+920");
extern u32 trimui_right_member_root __asm__("trimui_effect_f1+440");
_Static_assert(__builtin_offsetof(struct trimui_led_effect, frames[342]) == 1400, "middle root");
_Static_assert(__builtin_offsetof(struct trimui_led_effect, frames[222]) == 920, "left root");
_Static_assert(__builtin_offsetof(struct trimui_led_effect, frames[102]) == 440, "right root");
_Static_assert(920 + sizeof(trimui_left_member_root) <= sizeof(trimui_effect_r), "left root extent");
_Static_assert(440 + sizeof(trimui_right_member_root) <= sizeof(trimui_effect_f1), "right root extent");
_Static_assert(920 + 2952 == sizeof(struct trimui_led_effect), "left numeric field delta");
_Static_assert(440 + 3432 == sizeof(struct trimui_led_effect), "right numeric field delta");
#define TRIMUI_EFFECT_MEMBER(root, delta, member) \
 (*(__typeof__(trimui_effect_m.member) *)((root) + (delta) + __builtin_offsetof(struct trimui_led_effect, member)))

/* The root is the existing pointer slot itself, never its pointed-to device. */
extern __typeof__(sunxi_led) trimui_front_member_root __asm__("sunxi_led");
_Static_assert(sizeof(trimui_front_member_root) == 8, "front root pointer slot");
#define FRONT_FIELD(root, member) TRIMUI_EFFECT_MEMBER(root, 40, member)
