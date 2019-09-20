/**
 * Copyright 2019 Joshua Bakita
 */
#include "scu.h"
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void print_ctrl(SCU_CTRL_t ctrl);
void print_cfg(SCU_CFG_t cfg);
void print_pwr(PWR_STAT_t pwr);
void print_scu_acl(SCU_ACL_t acl);
void print_scu_nacl(SCU_NACL_t acl);

int main(int argc, char** argv) {
    void* A;
    int fd = open("/dev/mem", O_RDWR, 0);
    if (fd < 0) {
        fprintf(stderr, "Could not open /dev/mem\n");
        return 1;
    }
    A = (unsigned long*) mmap(NULL, sizeof(SCU_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, SCU_BASE_ADDR);
    if (A == MAP_FAILED) {
        perror("Unable to map SCU registers from /dev/mem");
        close(fd);
        return 2;
    }
    print_ctrl(((pSCU_t)A)->ctrl);
    print_cfg(((pSCU_t)A)->cfg);
    print_pwr(((pSCU_t)A)->pwr_stat);
    fprintf(stdout, "Filtering starts at address %#0x\n", ((pSCU_t)A)->filter_start_addr);
    fprintf(stdout, "Filtering ends at address %#0x\n", ((pSCU_t)A)->filter_end_addr);
    print_scu_acl(((pSCU_t)A)->scu_acl);
    print_scu_nacl(((pSCU_t)A)->scu_nacl);

    close(fd);
}

void print_ctrl(SCU_CTRL_t ctrl) {
    if (ctrl.scu_enable)
        fprintf(stdout, "Snoop Control Unit (SCU) Enabled\n");
    else
        fprintf(stdout, "Snoop Control Unit (SCU) Disabled\n");
    if (ctrl.address_filtering_enable)
        fprintf(stdout, "Address filtering is enabled\n");
    else
        fprintf(stdout, "Address filtering is disabled\n");
    if (ctrl.scu_ram_parity_enable)
        fprintf(stdout, "SCU RAM parity is enabled\n");
    else
        fprintf(stdout, "SCU RAM parity is disabled\n");
    if (ctrl.scu_speculative_linefill_enable)
        fprintf(stdout, "SCU speculative linefill is enabled\n");
    else
        fprintf(stdout, "SCU speculative linefill is disabled\n");
    if (ctrl.force_all_to_port0_enable)
        fprintf(stdout, "ACP requests are forced to AXI master 0\n");
    else
        fprintf(stdout, "ACP requests are not forced to AXI master 0\n");
    if (ctrl.scu_standby_enable)
        fprintf(stdout, "SCU auto standby in enabled\n");
    else
        fprintf(stdout, "SCU auto standby is disabled\n");
    if (ctrl.ic_standby_enable)
        fprintf(stdout, "Interrupt Controller (IC) auto standby in enabled\n");
    else
        fprintf(stdout, "Interrupt Controller (IC) auto standby in disabled\n");
}

void print_cfg(SCU_CFG_t cfg) {
    fprintf(stdout, "You have %d CPUs\n", cfg.cpu_count+1);
    unsigned int smp[4] = {cfg.cpu_0_smp, cfg.cpu_1_smp, cfg.cpu_2_smp, cfg.cpu_3_smp};
    unsigned int tag[4] = {cfg.cpu_0_tag, cfg.cpu_1_tag, cfg.cpu_2_tag, cfg.cpu_3_tag};
    for (int i = 0; i <= cfg.cpu_count; i++) {
        if (smp[i])
            fprintf(stdout, "CPU%d is in SMP mode\n", i);
        else
            fprintf(stdout, "CPU%d is in AMP mode\n", i);
        if (tag[i] == 0)
            fprintf(stdout, "CPU%d has a 16KB cache with 64 indexes per tag RAM\n", i);
        if (tag[i] == 1)
            fprintf(stdout, "CPU%d has a 32KB cache with 128 indexes per tag RAM\n", i);
        if (tag[i] == 2)
            fprintf(stdout, "CPU%d has a 64KB cache with 256 indexes per tag RAM\n", i);
    }
}

void print_pwr(PWR_STAT_t pwr) {
    unsigned int stats[4] = {pwr.cpu_0_pwr, pwr.cpu_1_pwr, pwr.cpu_2_pwr, pwr.cpu_3_pwr};
    for (int i = 0; i < 4; i++) {
        if (stats[i] == 0)
            fprintf(stdout, "CPU%d is in normal power mode\n", i);
        if (stats[i] == 2)
            fprintf(stdout, "CPU%d is dormant. No coherency requests shall be sent\n", i);
        if (stats[i] == 3)
            fprintf(stdout, "CPU%d is powered off or nonpresent\n", i);
    }
}

void print_scu_acl(SCU_ACL_t acl) {
    unsigned int acl_arr[4] = {acl.cpu0, acl.cpu1, acl.cpu2, acl.cpu3};
    for (int i = 0; i < 4; i++) {
        if (acl_arr[i] == 1)
            fprintf(stdout, "CPU%d is allowed to change SCU registers\n", i);
        else
            fprintf(stdout, "CPU%d is prohibited from changing SCU registers\n", i);
    }
}

void print_scu_nacl(SCU_NACL_t acl) {
    unsigned int reg_acl[4] = {acl.cpu0_reg, acl.cpu1_reg, acl.cpu2_reg, acl.cpu3_reg};
    unsigned int timer_acl[4] = {acl.cpu0_timer, acl.cpu1_timer, acl.cpu2_timer, acl.cpu3_timer};
    unsigned int gtimer_acl[4] = {acl.cpu0_gtimer, acl.cpu1_gtimer, acl.cpu2_gtimer, acl.cpu3_gtimer};
    for (int i = 0; i < 4; i++) {
        fprintf(stdout, "In non-secure mode, CPU%d", i);
        if (reg_acl[i] == 1)
            fprintf(stdout, " may change SCU registers,");
        else
            fprintf(stdout, " may not change SCU registers,");
        if (timer_acl[i] == 1)
            fprintf(stdout, " may change their private timer and watchdog,");
        else
            fprintf(stdout, " may not change their private timer and watchdog,");
        if (gtimer_acl[i] == 1)
            fprintf(stdout, " and may change the global timer\n");
        else
            fprintf(stdout, " and may not change the global timer\n");
    }
}

