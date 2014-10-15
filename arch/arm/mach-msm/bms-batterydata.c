/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/mfd/pm8xxx/pm8921-bms.h>

#if defined (CONFIG_MACH_MSM8960_FX1)
static struct single_row_lut LGC_BL54SH_2500_fcc_temp = {
	.x		= {-20, 0, 25, 40, 60},
	.y		= {2509, 2559, 2558, 2550, 2533},
	.cols	= 5
};

static struct single_row_lut LGC_BL54SH_2500_fcc_sf = {
	.x		= {0},
	.y		= {100},
	.cols	= 1
};

static struct sf_lut LGC_BL54SH_2500_rbatt_sf = { 
        .rows           = 28, 
        .cols           = 5,
        .row_entries            = {-20, 0, 25, 40, 60},
        .percent        = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
        .sf                     = {
					{1150,271,100,84,80},
					{1230,279,103,86,81},
					{1350,287,108,89,83},
					{1285,292,113,92,85},
					{1217,298,119,96,88},
					{1223,287,127,101,91},
					{1218,286,137,109,96},
					{1229,285,122,113,100},
					{1289,285,107,88,83},
					{1373,291,107,89,84},
					{1470,293,112,93,88},
					{1574,298,115,97,92},
					{1687,319,120,97,87},
					{1836,353,121,94,85},
					{2265,389,119,93,85},
					{3509,450,117,91,85},
					{5909,548,137,104,93},
					{10832,696,177,127,113},
					{6570,618,171,130,119},
					{8456,662,180,134,122},
					{10987,721,190,140,125},
					{14033,804,207,149,131},
					{17472,926,233,162,139},
					{21896,1104,269,177,148},
					{28485,1304,316,196,159},
					{38431,2053,386,221,175},
					{53388,4136,522,268,206},
					{69919,7084,769,371,295},

        }
};
static struct sf_lut LGC_BL54SH_2500_pc_sf = {
	.rows		= 1,
	.cols		= 1,
	/* row_entries are chargecycles */
	.row_entries	= {0},
	.percent	= {100},
	.sf		= {
			{100}
	},
};

static struct pc_temp_ocv_lut LGC_BL54SH_2500_pc_temp_ocv = {
	.rows		= 29,
	.cols		= 5,
	.temp		= {-20, 0, 25, 40, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
				{4334, 4332, 4327, 4321, 4312},
				{4228, 4248, 4250, 4248, 4242},
				{4156, 4186, 4189, 4188, 4182},
				{4089, 4128, 4132, 4131, 4126},
				{4033, 4072, 4078, 4078, 4073},
				{3953, 4014, 4028, 4028, 4024},
				{3909, 3961, 3983, 3984, 3980},
				{3858, 3917, 3940, 3943, 3939},
				{3819, 3877, 3890, 3897, 3895},
				{3801, 3843, 3850, 3850, 3849},
				{3787, 3815, 3823, 3824, 3822},
				{3773, 3791, 3802, 3803, 3801},
				{3760, 3774, 3784, 3784, 3782},
				{3745, 3760, 3769, 3765, 3755},
				{3727, 3741, 3750, 3741, 3727},
				{3703, 3715, 3716, 3708, 3695},
				{3665, 3693, 3684, 3676, 3663},
				{3597, 3657, 3656, 3650, 3639},
				{3462, 3569, 3575, 3566, 3550},
				{3425, 3538, 3546, 3538, 3522},
				{3389, 3505, 3513, 3506, 3493},
				{3354, 3468, 3478, 3473, 3462},
				{3320, 3430, 3444, 3441, 3432},
				{3286, 3392, 3410, 3409, 3400},
				{3250, 3350, 3372, 3371, 3363},
				{3215, 3302, 3323, 3322, 3315},
				{3174, 3241, 3258, 3259, 3250},
				{3106, 3156, 3164, 3164, 3153},
				{3000, 3000, 3000, 3000, 3000}
	}
};



struct pm8921_bms_battery_data LGC_BL54SH_2500_data = {
		.fcc				= 2500,
		.fcc_temp_lut		= &LGC_BL54SH_2500_fcc_temp,
		.fcc_sf_lut 		= &LGC_BL54SH_2500_fcc_sf,
		.pc_temp_ocv_lut	= &LGC_BL54SH_2500_pc_temp_ocv,
		.pc_sf_lut			= &LGC_BL54SH_2500_pc_sf,
		.rbatt_sf_lut		= &LGC_BL54SH_2500_rbatt_sf,
		.default_rbatt_mohm	= 125,
		.delta_rbatt_mohm	= 60,
};

/*
 * add Sanyo cell profiling data
 * it can be distinguished by part number
 * e.g. P/N: #### AAC
 */
static struct single_row_lut SANYO_BL54SH_2500_fcc_temp = {
	.x		= {-20, 0, 25, 40, 60},
	.y		= {2408, 2426, 2423, 2410, 2390},
	.cols	= 5
};

static struct single_row_lut SANYO_BL54SH_2500_fcc_sf = {
	.x		= {0},
	.y		= {100},
	.cols	= 1
};

static struct sf_lut SANYO_BL54SH_2500_rbatt_sf = {
	.rows           = 28, 
	.cols           = 5,
	/* row_entries are temperature */
	.row_entries    = {-20, 0, 25, 40, 60},
	.percent        = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
	.sf             = {
					{1001,318,100,77,71},
					{988,326,103,79,72},
					{971,330,106,81,74},
					{959,329,111,83,75},
					{953,319,118,87,77},
					{955,314,128,93,81},
					{962,311,134,103,87},
					{967,312,106,85,76},
					{988,314,103,80,74},
					{1035,324,105,82,75},
					{1103,344,109,86,79},
					{1190,367,114,89,82},
					{1283,388,122,86,76},
					{1390,397,127,84,76},
					{1517,375,133,89,79},
					{2023,384,136,95,81},
					{3315,502,175,121,101},
					{7330,750,180,142,133},
					{7686,629,179,145,136},
					{8900,679,191,151,142},
					{10115,729,204,156,148},
					{11908,820,223,166,156},
					{13911,911,241,177,173},
					{16388,1204,280,193,194},
					{19397,1497,318,230,246},
					{23085,2127,437,309,310},
					{27284,2756,556,439,416},
					{33233,4820,1793,790,537},
	}
};

static struct sf_lut SANYO_BL54SH_2500_pc_sf = {
    .rows       = 1,
    .cols       = 1,
    /* row_entries are chargecycles */
    .row_entries    = {0},
    .percent    = {100},
    .sf     = {
			{100}
    },
};

static struct pc_temp_ocv_lut SANYO_BL54SH_2500_pc_temp_ocv = {
	.rows		= 29,
	.cols		= 5,
	.temp		= {-20, 0, 25, 40, 60},
	.percent	= {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
	.ocv		= {
				{4327, 4328, 4324, 4319, 4310},
				{4178, 4238, 4251, 4250, 4245},
				{4082, 4173, 4192, 4192, 4188},
				{4003, 4116, 4136, 4137, 4133},
				{3935, 4060, 4083, 4084, 4081},
				{3877, 4003, 4034, 4035, 4032},
				{3826, 3950, 3989, 3990, 3988},
				{3782, 3903, 3945, 3948, 3946},
				{3740, 3863, 3892, 3898, 3898},
				{3699, 3827, 3849, 3854, 3855},
				{3663, 3798, 3820, 3824, 3825},
				{3633, 3779, 3797, 3802, 3803},
				{3607, 3763, 3780, 3784, 3784},
				{3584, 3747, 3767, 3768, 3764},
				{3561, 3724, 3753, 3750, 3738},
				{3536, 3687, 3727, 3725, 3714},
				{3505, 3630, 3689, 3692, 3679},
				{3461, 3569, 3649, 3662, 3659},
				{3393, 3488, 3574, 3600, 3593},
				{3367, 3455, 3535, 3560, 3560},
				{3341, 3423, 3496, 3520, 3517},
				{3314, 3390, 3457, 3479, 3474},
				{3288, 3356, 3418, 3439, 3431},
				{3257, 3312, 3371, 3395, 3388},
				{3211, 3263, 3320, 3343, 3340},
				{3156, 3212, 3267, 3285, 3278},
				{3100, 3155, 3202, 3214, 3201},
				{3050, 3087, 3115, 3118, 3099},
				{3000, 3000, 3000, 3000, 3000}
	}
};

struct pm8921_bms_battery_data SANYO_BL54SH_2500_data = {
        .fcc                = 2540,
        .fcc_temp_lut       = &SANYO_BL54SH_2500_fcc_temp,
        .fcc_sf_lut         = &SANYO_BL54SH_2500_fcc_sf,
        .pc_temp_ocv_lut    = &SANYO_BL54SH_2500_pc_temp_ocv,
        .pc_sf_lut          = &SANYO_BL54SH_2500_pc_sf,
        .rbatt_sf_lut       = &SANYO_BL54SH_2500_rbatt_sf,
        .default_rbatt_mohm = 131,
        .delta_rbatt_mohm   = 60,

};
/*                                               
 */
#endif
