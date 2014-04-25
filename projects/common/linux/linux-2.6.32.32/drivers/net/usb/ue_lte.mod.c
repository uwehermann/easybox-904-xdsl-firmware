#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v216Fp0040d*dc*dsc*dp*ic02isc06ipFF*");
MODULE_ALIAS("usb:v216Fp0040d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v216Fp0041d*dc*dsc*dp*ic02iscFFipFF*");
MODULE_ALIAS("usb:v216Fp0041d*dc*dsc*dp*ic02iscFFip00*");
