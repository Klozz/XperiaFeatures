/*
 *  Author: nobodyAtall <nobodyAtall at xda-developers>
 *  Adapted from doixanh's <doixanh at xda-developers> X8Undervolt module
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>	

// defines
#define DX_MODULE_NAME 			"x10uv"
#define DX_PROCFS_NAME 			"x10uv"
#define DXDBG(x)					
#define X10

// patch offsets
#ifdef X10
#define DEVICE_NAME				"X10"
#define OFS_KALLSYMS_LOOKUP_NAME	0xC0094100			// kallsyms_lookup_name
#endif

// struct definition
struct clkctl_acpu_speed_dx {
	unsigned int     use_for_scaling;
	unsigned int     acpuclk_khz;
	int              pll;
	unsigned int     acpuclk_src_sel;
	unsigned int     acpuclk_src_div;
	unsigned int     ahbclk_khz;
	unsigned int     ahbclk_div;
	unsigned int     axiclk_khz;
	unsigned int     sc_core_src_sel_mask;
	unsigned int     sc_l_value;
	int              vdd;
	unsigned long    lpj; /* loops_per_jiffy */
};

// variables
static struct clkctl_acpu_speed_dx *standard_clocks;

// external variables / functions
typedef unsigned long (*kallsyms_lookup_name_type)(const char *name);
static kallsyms_lookup_name_type kallsyms_lookup_name_dx;

// init module
static int __init x10uv_init(void)
{
	printk(KERN_INFO DX_MODULE_NAME ": Module loaded. Built for target device: " DEVICE_NAME "\n");
	
	// our 'GetProcAddress' :D
	kallsyms_lookup_name_dx = (void*) OFS_KALLSYMS_LOOKUP_NAME;

	// look for other offsets
	standard_clocks = (void*) kallsyms_lookup_name_dx("acpu_freq_tbl_998");
	
	// do some undervoltage
	standard_clocks[2].vdd = 950;		// a little bit under voltage for 245760 from 1000
	standard_clocks[3].vdd = 950;		// a little bit under voltage for 384000 from 1000
	standard_clocks[8].vdd = 1000;		// a little bit under voltage for 576000 from 1050
	standard_clocks[13].vdd = 1100;		// a little bit under voltage for 768000 from 1150
	standard_clocks[19].vdd = 1300;		// a little bit under voltage for 998400 (max) from 1300
	
	/*standard_clocks[2].vdd = 900;		// a little bit under voltage for 245760
	standard_clocks[3].vdd = 900;		// a little bit under voltage for 384000
	standard_clocks[8].vdd = 950;		// a little bit under voltage for 576000
	standard_clocks[13].vdd = 1050;		// a little bit under voltage for 768000
	standard_clocks[19].vdd = 1300;		// same voltage for 998400 (max)
	*/
	printk(KERN_INFO DX_MODULE_NAME ": patching done. enjoy better battery life.\n");
	return 0;
}


// exit module - will most likely not be called
static void __exit x10uv_exit(void)
{
	printk(KERN_INFO DX_MODULE_NAME ": module unloaded\n");
}

module_init(x10uv_init);
module_exit(x10uv_exit);

MODULE_DESCRIPTION("Undervolting module for Sony Ericsson X10");
MODULE_LICENSE("GPL");
