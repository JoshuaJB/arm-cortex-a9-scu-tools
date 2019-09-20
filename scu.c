/**
 * Copyright 2019 Joshua Bakita
 */
#include "scu.h"
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define DANGEROUS "\033[1mDANGEROUS\033[0m"

unsigned int parse_range(unsigned int min, unsigned int max, unsigned int base, char* text);
void print_help(char* prog_name);
void print_write_status(unsigned int goal, unsigned int result, char* key);
void print_acl_warning();
void print_ctrl(SCU_CTRL_t ctrl);
void print_cfg(SCU_CFG_t cfg);
void print_pwr(PWR_STAT_t pwr);
void print_scu_acl(SCU_ACL_t acl);
void print_scu_nacl(SCU_NACL_t acl);

int main(int argc, char** argv) {
    int fd = open("/dev/mem", O_RDWR, 0);
    if (fd < 0) {
        fprintf(stderr, "Could not open /dev/mem. Make sure you are root.\n");
        return 1;
    }
    const pSCU_t scu = mmap(NULL, sizeof(SCU_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, SCU_BASE_ADDR);
    if (scu == MAP_FAILED) {
        perror("Unable to map SCU registers from /dev/mem. Make sure you are on an i.MX6 Dual/Quad.");
        close(fd);
        return 2;
    }
    close(fd); // Memory still stays mapped after the file is closed

    if (argc == 2 && strcmp(argv[1], "read") == 0) {
        print_ctrl(scu->ctrl);
        print_cfg(scu->cfg);
        print_pwr(scu->pwr_stat);
        fprintf(stdout, "Filtering starts at address %#0x\n", scu->filter_start_addr);
        fprintf(stdout, "Filtering ends at address %#0x\n", scu->filter_end_addr);
        print_scu_acl(scu->scu_acl);
        print_scu_nacl(scu->scu_nacl);
    } else if (argc == 4 && strcmp(argv[1], "write") == 0) {
        if (strcmp(argv[2], "scu_enable") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            if (val == 0) {
                fprintf(stdout, "Turning off the Snoop Control Unit (SCU) is "DANGEROUS"!\n"
                        "L1 cache coherency will not be maintained while the SCU is off.\n");
                fprintf(stdout, "Are you CERTAIN you want to proceed? (y/N): ");
                if (fgetc(stdin) != 'y') {
                    fprintf(stdout, "Aborting...\n");
                    exit(5);
                }
            }
            scu->ctrl.scu_enable = val;
            print_write_status(val, scu->ctrl.scu_enable, argv[2]);
        } else if (strcmp(argv[2], "address_filtering_enable") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            scu->ctrl.address_filtering_enable = val;
            print_write_status(val, scu->ctrl.address_filtering_enable, argv[2]);
        } else if (strcmp(argv[2], "scu_ram_parity_enable") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            scu->ctrl.scu_ram_parity_enable = val;
            print_write_status(val, scu->ctrl.scu_ram_parity_enable, argv[2]);
        } else if (strcmp(argv[2], "scu_speculative_linefill_enable") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            scu->ctrl.scu_speculative_linefill_enable = val;
            print_write_status(val, scu->ctrl.scu_speculative_linefill_enable, argv[2]);
        } else if (strcmp(argv[2], "force_acp_to_port0") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            scu->ctrl.force_acp_to_port0 = val;
            print_write_status(val, scu->ctrl.force_acp_to_port0, argv[2]);
        } else if (strcmp(argv[2], "scu_standby_enable") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            scu->ctrl.scu_standby_enable = val;
            print_write_status(val, scu->ctrl.scu_standby_enable, argv[2]);
        } else if (strcmp(argv[2], "ic_standby_enable") == 0) {
            unsigned int val = parse_range(0, 1, 10, argv[3]);
            scu->ctrl.ic_standby_enable = val;
            print_write_status(val, scu->ctrl.ic_standby_enable, argv[2]);
        } else if (strcmp(argv[2], "cpu0_pwr") == 0) {
            unsigned int val = parse_range(0, 3, 10, argv[3]);
            scu->pwr_stat.cpu0_pwr = val;
            print_write_status(val, scu->pwr_stat.cpu0_pwr, argv[2]);
        } else if (strcmp(argv[2], "cpu1_pwr") == 0) {
            unsigned int val = parse_range(0, 3, 10, argv[3]);
            scu->pwr_stat.cpu1_pwr = val;
            print_write_status(val, scu->pwr_stat.cpu1_pwr, argv[2]);
        } else if (strcmp(argv[2], "cpu2_pwr") == 0) {
            unsigned int val = parse_range(0, 3, 10, argv[3]);
            scu->pwr_stat.cpu2_pwr = val;
            print_write_status(val, scu->pwr_stat.cpu2_pwr, argv[2]);
        } else if (strcmp(argv[2], "cpu3_pwr") == 0) {
            unsigned int val = parse_range(0, 3, 10, argv[3]);
            scu->pwr_stat.cpu3_pwr = val;
            print_write_status(val, scu->pwr_stat.cpu3_pwr, argv[2]);
        } else if (strcmp(argv[2], "filter_start_addr") == 0) {
            unsigned int val = parse_range(0, -1, 16, argv[3]);
            scu->filter_start_addr = val;
            print_write_status(val, scu->filter_start_addr, argv[2]);
        } else if (strcmp(argv[2], "filter_end_addr") == 0) {
            unsigned int val = parse_range(0, -1, 16, argv[3]);
            scu->filter_end_addr = val;
            print_write_status(val, scu->filter_end_addr, argv[2]);
        } else if (strcmp(argv[2], "cpu0_inval_ways") == 0) {
            unsigned int val = parse_range(0, 0xF, 16, argv[3]);
            scu->tag_inval.cpu0_ways = val;
            fprintf(stdout, "Ways invalidated (note this will only take effect if you are in secure mode)\n");
        } else if (strcmp(argv[2], "cpu1_inval_ways") == 0) {
            unsigned int val = parse_range(0, 0xF, 16, argv[3]);
            scu->tag_inval.cpu1_ways = val;
            fprintf(stdout, "Ways invalidated (note this will only take effect if you are in secure mode)\n");
        } else if (strcmp(argv[2], "cpu2_inval_ways") == 0) {
            unsigned int val = parse_range(0, 0xF, 16, argv[3]);
            scu->tag_inval.cpu2_ways = val;
            fprintf(stdout, "Ways invalidated (note this will only take effect if you are in secure mode)\n");
        } else if (strcmp(argv[2], "cpu3_inval_ways") == 0) {
            unsigned int val = parse_range(0, 0xF, 16, argv[3]);
            scu->tag_inval.cpu3_ways = val;
            fprintf(stdout, "Ways invalidated (note this will only take effect if you are in secure mode)\n");
        } else if (strcmp(argv[2], "cpu0_scu_acl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_acl.cpu0 = val;
            print_write_status(val, scu->scu_acl.cpu0, argv[2]);
        } else if (strcmp(argv[2], "cpu1_scu_acl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_acl.cpu1 = val;
            print_write_status(val, scu->scu_acl.cpu1, argv[2]);
        } else if (strcmp(argv[2], "cpu2_scu_acl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_acl.cpu2 = val;
            print_write_status(val, scu->scu_acl.cpu2, argv[2]);
        } else if (strcmp(argv[2], "cpu3_scu_acl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_acl.cpu3 = val;
            print_write_status(val, scu->scu_acl.cpu3, argv[2]);
        } else if (strcmp(argv[2], "cpu0_scu_nacl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu0_reg = val;
            print_write_status(val, scu->scu_nacl.cpu0_reg, argv[2]);
        } else if (strcmp(argv[2], "cpu1_scu_nacl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu1_reg = val;
            print_write_status(val, scu->scu_nacl.cpu1_reg, argv[2]);
        } else if (strcmp(argv[2], "cpu2_scu_nacl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu2_reg = val;
            print_write_status(val, scu->scu_nacl.cpu2_reg, argv[2]);
        } else if (strcmp(argv[2], "cpu3_scu_nacl") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu3_reg = val;
            print_write_status(val, scu->scu_nacl.cpu3_reg, argv[2]);
        } else if (strcmp(argv[2], "cpu0_scu_timer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu0_timer = val;
            print_write_status(val, scu->scu_nacl.cpu0_timer, argv[2]);
        } else if (strcmp(argv[2], "cpu1_scu_timer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu1_timer = val;
            print_write_status(val, scu->scu_nacl.cpu1_timer, argv[2]);
        } else if (strcmp(argv[2], "cpu2_scu_timer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu2_timer = val;
            print_write_status(val, scu->scu_nacl.cpu2_timer, argv[2]);
        } else if (strcmp(argv[2], "cpu3_scu_timer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu3_timer = val;
            print_write_status(val, scu->scu_nacl.cpu3_timer, argv[2]);
        } else if (strcmp(argv[2], "cpu0_scu_gtimer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu0_gtimer = val;
            print_write_status(val, scu->scu_nacl.cpu0_gtimer, argv[2]);
        } else if (strcmp(argv[2], "cpu1_scu_gtimer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu1_gtimer = val;
            print_write_status(val, scu->scu_nacl.cpu1_gtimer, argv[2]);
        } else if (strcmp(argv[2], "cpu2_scu_gtimer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu2_gtimer = val;
            print_write_status(val, scu->scu_nacl.cpu2_gtimer, argv[2]);
        } else if (strcmp(argv[2], "cpu3_scu_gtimer") == 0) {
            unsigned int val = parse_range(0, 1, 16, argv[3]);
            if (val == 0)
                print_acl_warning();
            scu->scu_nacl.cpu3_gtimer = val;
            print_write_status(val, scu->scu_nacl.cpu3_gtimer, argv[2]);
        } else {
            print_help(argv[0]);
            return 4;
        }
    } else {
        print_help(argv[0]);
        return 3;
    }
}

unsigned int parse_range(unsigned int min, unsigned int max, unsigned int base, char* text) {
    char* end = "\0";
    unsigned int num = strtol(text, &end, base);
    if (*end != '\0') {
        fprintf(stderr, "Unable to parse input '%s'\n", text);
        exit(4);
    }
    if (num > max || num < min) {
        fprintf(stderr, "Input out of range '%s'\n", text);
        exit(4);
    }
    return num;
}

void print_help(char* prog_name) {
    fprintf(stdout, "This program can read and configure the Snoop Control"
            "Unit (SCU) registers on Cortex-A9 MPCore processors.\nUsage:\n"
            "  %s read\n  %s write <key> <value>\n", prog_name, prog_name);
    fprintf(stdout, "    Available key and value ranges:\n"
            "\tscu_enable [0,1] Enable or disable the SCU ["DANGEROUS"]\n"
            "\taddress_filtering_enable [0,1] Enable address filtering to redirect requests to M1 if physical address in within set range (always zero for single master configurations)\n"
            "\tscu_ram_parity_enable [0,1] Enable parity checking on the SCU RAM (always zero if unavailable)\n"
            "\tscu_speculative_linefill_enable [0,1] Enable speculative linefills to the L2C-310 L2 cache controller\n"
            "\tforce_acp_to_port0 [0,1] Force all ACP requests to master 0\n"
            "\tscu_standby_enable [0,1] Enable auto-standby of the SCU when idle\n"
            "\tic_standby_enable [0,1] Enable auto-standby of the interrupt controller when idle\n"
            "\tcpu[0,1,2,3]_pwr {0,2,3} Set CPU 0, 1, 2, or 3 power mode to normal, dormant, or powered-off\n"
            "\tfilter_start_addr 0x00000000-0xFFFFFFFF Set the address from which requests are filtered to master 1\n"
            "\tfilter_end_addr 0x00000000-0xFFFFFFFF Set the address to which requests are filtered to master 1\n"
            "\tcpu[0,1,2,3]_inval_ways 0x0-0xF Invalidate all SCU tag RAM ways on CPU 0, 1, 2, or 3 matching the provided mask (4 possible ways)\n"
            "\tcpu[0,1,2,3]_scu_acl [0,1] Enable SCU configuration write access from CPU0, 1, 2, or 3 ["DANGEROUS"]\n"
            "\tcpu[0,1,2,3]_scu_nacl [0,1] Enable non-secure write access to SCU configuration from CPU0, 1, 2, or 3\n"
            "\tcpu[0,1,2,3]_scu_nacl_timer [0,1] Enable non-secure write access to SCU timers and watchdog from CPU0, 1, 2, or 3\n"
            "\tcpu[0,1,2,3]_scu_nacl_gtimer [0,1] Enable non-secure write access to SCU global timers from CPU0, 1, 2, or 3\n");
}

void print_write_status(unsigned int goal, unsigned int result, char* key) {
    if (goal == result)
        fprintf(stdout, "Write successful to %s!\n", key);
    else
        fprintf(stdout, "Write to %s failed.\n", key);
}

void print_acl_warning() {
    fprintf(stdout, "Disabling access to the SCU is "DANGEROUS"!\n"
            "The only way to reverse this is with a machine reset or with a command from a yet unrestricted CPU.\n");
    fprintf(stdout, "Are you CERTAIN you want to proceed? (y/N): ");
    if (fgetc(stdin) != 'y') {
        fprintf(stdout, "Aborting...\n");
        exit(5);
    }
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
    if (ctrl.force_acp_to_port0)
        fprintf(stdout, "ACP requests are forced to AXI master 0\n");
    else
        fprintf(stdout, "ACP requests are not forced to AXI master 0\n");
    if (ctrl.scu_standby_enable)
        fprintf(stdout, "SCU auto standby is enabled\n");
    else
        fprintf(stdout, "SCU auto standby is disabled\n");
    if (ctrl.ic_standby_enable)
        fprintf(stdout, "Interrupt Controller (IC) auto standby is enabled\n");
    else
        fprintf(stdout, "Interrupt Controller (IC) auto standby is disabled\n");
}

void print_cfg(SCU_CFG_t cfg) {
    fprintf(stdout, "You have %d CPUs\n", cfg.cpu_count+1);
    unsigned int smp[4] = {cfg.cpu0_smp, cfg.cpu1_smp, cfg.cpu2_smp, cfg.cpu3_smp};
    unsigned int tag[4] = {cfg.cpu0_tag, cfg.cpu1_tag, cfg.cpu2_tag, cfg.cpu3_tag};
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
    unsigned int stats[4] = {pwr.cpu0_pwr, pwr.cpu1_pwr, pwr.cpu2_pwr, pwr.cpu3_pwr};
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

