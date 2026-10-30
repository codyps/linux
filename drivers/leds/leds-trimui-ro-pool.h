/* SPDX-License-Identifier: GPL-2.0-only */
/* TG3040 #860 read-only layout, reconstructed from independently reached data.
 * The grouped names reproduce byte positions, not original C declarations.
 * Keep the effect-name pointers as a separate object: an embedded pointer
 * member changes the compiler's address materialization in help_show_effect.
 */
#ifndef TRIMUI_LED_RO_POOL
#define TRIMUI_LED_RO_POOL
/* Four recovered periods; only the first three are selected by stock. */
static const int trimui_blink_pattern[40] __attribute__((no_reorder)) = {
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
};

static const struct trimui_led_ro_early {
	char sunxi_set_reset_ns[19];
	char sunxi_set_t1h_ns[17];
	char sunxi_set_t1l_ns[17];
	char sunxi_set_t0h_ns[17];
	char sunxi_set_t0l_ns[17];
	char sunxi_set_wait_time0_ns[24];
	char sunxi_set_wait_time1_ns[24];
	char sunxi_set_wait_data_time_ns[28];
	char sunxi_ledc_complete[20];
	char led_select_gpio_state[22];
} trimui_ro_early __attribute__((no_reorder)) = {
	.sunxi_set_reset_ns = "sunxi_set_reset_ns",
	.sunxi_set_t1h_ns = "sunxi_set_t1h_ns",
	.sunxi_set_t1l_ns = "sunxi_set_t1l_ns",
	.sunxi_set_t0h_ns = "sunxi_set_t0h_ns",
	.sunxi_set_t0l_ns = "sunxi_set_t0l_ns",
	.sunxi_set_wait_time0_ns = "sunxi_set_wait_time0_ns",
	.sunxi_set_wait_time1_ns = "sunxi_set_wait_time1_ns",
	.sunxi_set_wait_data_time_ns = "sunxi_set_wait_data_time_ns",
	.sunxi_ledc_complete = "sunxi_ledc_complete",
	.led_select_gpio_state = "led_select_gpio_state",
};
#include "leds-trimui-effect-name-text.h"
static const char * const trimui_effect_names[8] __attribute__((no_reorder)) = {
	"disable", trimui_effect_name_text.linear, trimui_effect_name_text.breath,
	trimui_effect_name_text.sniff, trimui_effect_name_text.static_effect,
	trimui_effect_name_text.blink1, trimui_effect_name_text.blink2,
	trimui_effect_name_text.blink3,
};
static const struct trimui_led_ro_writes {
	char wait_data_time_ns_write[24];
	char wait_time0_ns_write[20];
	char t0l_ns_write[13];
	char t0h_ns_write[13];
	char t1l_ns_write[13];
	char t1h_ns_write[13];
	char reset_ns_write[15];
	char wait_time1_ns_write[20];
	char sunxi_led_shutdown[19];
	char sunxi_ledc_dma_callback[24];
	char output_mode_write[18];
	char sunxi_led_probe[16];
	char sunxi_get_u32_of_property[26];
	char sunxi_get_str_of_property[26];
	char sunxi_get_para_of_property[27];
	char led_regulator_request[22];
	char sunxi_register_led_classdev[28];
	char sunxi_clk_get[14];
	char sunxi_ledc_irq_init[20];
	char sunxi_ledc_pinctrl_init[24];
	char sunxi_led_create_debugfs[25];
} trimui_ro_writes __attribute__((no_reorder)) = {
	.wait_data_time_ns_write = "wait_data_time_ns_write",
	.wait_time0_ns_write = "wait_time0_ns_write",
	.t0l_ns_write = "t0l_ns_write",
	.t0h_ns_write = "t0h_ns_write",
	.t1l_ns_write = "t1l_ns_write",
	.t1h_ns_write = "t1h_ns_write",
	.reset_ns_write = "reset_ns_write",
	.wait_time1_ns_write = "wait_time1_ns_write",
	.sunxi_led_shutdown = "sunxi_led_shutdown",
	.sunxi_ledc_dma_callback = "sunxi_ledc_dma_callback",
	.output_mode_write = "output_mode_write",
	.sunxi_led_probe = "sunxi_led_probe",
	.sunxi_get_u32_of_property = "sunxi_get_u32_of_property",
	.sunxi_get_str_of_property = "sunxi_get_str_of_property",
	.sunxi_get_para_of_property = "sunxi_get_para_of_property",
	.led_regulator_request = "led_regulator_request",
	.sunxi_register_led_classdev = "sunxi_register_led_classdev",
	.sunxi_clk_get = "sunxi_clk_get",
	.sunxi_ledc_irq_init = "sunxi_ledc_irq_init",
	.sunxi_ledc_pinctrl_init = "sunxi_ledc_pinctrl_init",
	.sunxi_led_create_debugfs = "sunxi_led_create_debugfs",
};
#define SUNXI_LED_NAMED_ERR(name, fmt, arg...) pr_err("%s()%d - "fmt, name, __LINE__, ##arg)
#define SUNXI_LED_NAMED_LOG(name, level_mask, fmt, arg...) do { if (unlikely(debug_mask & level_mask)) pr_warn("%s()%d - "fmt, name, __LINE__, ##arg); } while (0)

#define SUNXI_LED_NAMED_STOCK_ERR(name, line, fmt, arg...) pr_err("%s()%d - "fmt, name, line, ##arg)
#define SUNXI_LED_NAMED_STOCK_LOG(name, line, level_mask, fmt, arg...) do { if (unlikely(debug_mask & level_mask)) pr_warn("%s()%d - "fmt, name, line, ##arg); } while (0)
#endif
