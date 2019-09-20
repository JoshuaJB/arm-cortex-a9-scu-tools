/**
 * Copyright 2019 Joshua Bakita
 * This program allows the configuration of the Cortex-A9 MPCore SCU
 */
#include <stdint.h>

#define SCU_BASE_ADDR 0x00A00000

typedef struct {
    unsigned int scu_enable:1;
    unsigned int address_filtering_enable:1;
    unsigned int scu_ram_parity_enable:1;
    unsigned int scu_speculative_linefill_enable:1;
    unsigned int force_acp_to_port0:1;
    unsigned int scu_standby_enable:1;
    unsigned int ic_standby_enable:1;
    unsigned int padding_1:25;
} SCU_CTRL_t;

typedef struct {
    unsigned int cpu_count:2;
    unsigned int padding_1:2;
    unsigned int cpu0_smp:1;
    unsigned int cpu1_smp:1;
    unsigned int cpu2_smp:1;
    unsigned int cpu3_smp:1;
    unsigned int cpu0_tag:2;
    unsigned int cpu1_tag:2;
    unsigned int cpu2_tag:2;
    unsigned int cpu3_tag:2;
    unsigned int padding_2:16;
} SCU_CFG_t;

typedef struct {
    unsigned int cpu0_pwr:2;
    unsigned int padding_1:6;
    unsigned int cpu1_pwr:2;
    unsigned int padding_2:6;
    unsigned int cpu2_pwr:2;
    unsigned int padding_3:6;
    unsigned int cpu3_pwr:2;
    unsigned int padding_4:6;
} PWR_STAT_t;

typedef struct {
    unsigned int cpu0_ways:4;
    unsigned int cpu1_ways:4;
    unsigned int cpu2_ways:4;
    unsigned int cpu3_ways:4;
    unsigned int padding_1:16;
} TAG_INVAL_t;

typedef struct {
    unsigned int cpu0:1;
    unsigned int cpu1:1;
    unsigned int cpu2:1;
    unsigned int cpu3:1;
    unsigned int padding_1:28;
} SCU_ACL_t;

typedef struct {
    unsigned int cpu0_reg:1;
    unsigned int cpu1_reg:1;
    unsigned int cpu2_reg:1;
    unsigned int cpu3_reg:1;
    unsigned int cpu0_timer:1;
    unsigned int cpu1_timer:1;
    unsigned int cpu2_timer:1;
    unsigned int cpu3_timer:1;
    unsigned int cpu0_gtimer:1;
    unsigned int cpu1_gtimer:1;
    unsigned int cpu2_gtimer:1;
    unsigned int cpu3_gtimer:1;
    unsigned int padding_1:20;
} SCU_NACL_t;

typedef struct {
    SCU_CTRL_t ctrl;
    SCU_CFG_t cfg;
    PWR_STAT_t pwr_stat;
    TAG_INVAL_t tag_inval; // Write-only, always reads as 0
    uint32_t padding_1[12];
    uint32_t filter_start_addr;
    uint32_t filter_end_addr;
    uint32_t padding_2[2];
    SCU_ACL_t scu_acl;
    SCU_NACL_t scu_nacl;
} SCU_t;

typedef SCU_t *pSCU_t;

