# Cortex-A9 MPCore Snoop Control Unit Configuration Tool
This tool allows you to read or write the configuration of the Snoop Control Unit (SCU) integrated into all ARM Cortex-A9 MPCore processors. For more information about the SCU, see section 2 of ARM DDI0407G: *ARM Cortex-A9 MPCore Technical Reference Manual, Issue G*.

The configuration offset in `scu.h` is presently set as appropriate for the i.MX6 Dual/Quad development board. Please change `SCU_BASE_ADDR` when using this application on other boards (typically this offset can be found in your board's reference manual).

## Building
To cross compile using arm-linux-gnueabihf-gcc:
```
make all
```
To build on your board using gcc:
```
make CC=gcc all
```

## Usage
```
root@bruford:~# /tmp/scu help
This program can read and configure the Snoop Control Unit (SCU) registers on Cortex-A9 MPCore processors.
Usage:
  /tmp/scu read
  /tmp/scu write <key> <value>
    Available key and value ranges:
	scu_enable [0,1] Enable or disable the SCU [DANGEROUS]
	address_filtering_enable [0,1] Enable address filtering to redirect requests to M1 if physical address in within set range (always zero for single master configurations)
	scu_ram_parity_enable [0,1] Enable parity checking on the SCU RAM (always zero if unavailable)
	scu_speculative_linefill_enable [0,1] Enable speculative linefills to the L2C-310 L2 cache controller
	force_acp_to_port0 [0,1] Force all ACP requests to master 0
	scu_standby_enable [0,1] Enable auto-standby of the SCU when idle
	ic_standby_enable [0,1] Enable auto-standby of the interrupt controller when idle
	cpu[0,1,2,3]_pwr {0,2,3} Set CPU 0, 1, 2, or 3 power mode to normal, dormant, or powered-off
	filter_start_addr 0x00000000-0xFFFFFFFF Set the address from which requests are filtered to master 1
	filter_end_addr 0x00000000-0xFFFFFFFF Set the address to which requests are filtered to master 1
	cpu[0,1,2,3]_inval_ways 0x0-0xF Invalidate all SCU tag RAM ways on CPU 0, 1, 2, or 3 matching the provided mask (4 possible ways)
	cpu[0,1,2,3]_scu_acl [0,1] Enable SCU configuration write access from CPU0, 1, 2, or 3 [DANGEROUS]
	cpu[0,1,2,3]_scu_nacl [0,1] Enable non-secure write access to SCU configuration from CPU0, 1, 2, or 3
	cpu[0,1,2,3]_scu_nacl_timer [0,1] Enable non-secure write access to SCU timers and watchdog from CPU0, 1, 2, or 3
	cpu[0,1,2,3]_scu_nacl_gtimer [0,1] Enable non-secure write access to SCU global timers from CPU0, 1, 2, or 3
```

## Example Output
This is from running the `read` command on an iMX6 Quad running a variant of Linux 4.1.3:
```
root@bruford~# /tmp/scu read
Snoop Control Unit (SCU) Enabled
Address filtering is enabled
SCU RAM parity is enabled
SCU speculative linefill is disabled
ACP requests are not forced to AXI master 0
SCU auto standby is enabled
Interrupt Controller (IC) auto standby is disabled
You have 4 CPUs
CPU0 is in SMP mode
CPU0 has a 32KB cache with 128 indexes per tag RAM
CPU1 is in SMP mode
CPU1 has a 32KB cache with 128 indexes per tag RAM
CPU2 is in SMP mode
CPU2 has a 32KB cache with 128 indexes per tag RAM
CPU3 is in SMP mode
CPU3 has a 32KB cache with 128 indexes per tag RAM
CPU0 is in normal power mode
CPU1 is in normal power mode
CPU2 is in normal power mode
CPU3 is in normal power mode
Filtering starts at address 0
Filtering ends at address 0
CPU0 is allowed to change SCU registers
CPU1 is allowed to change SCU registers
CPU2 is allowed to change SCU registers
CPU3 is allowed to change SCU registers
In non-secure mode, CPU0 may change SCU registers, may not change their private timer and watchdog, and may not change the global timer
In non-secure mode, CPU1 may change SCU registers, may not change their private timer and watchdog, and may not change the global timer
In non-secure mode, CPU2 may change SCU registers, may not change their private timer and watchdog, and may not change the global timer
In non-secure mode, CPU3 may change SCU registers, may not change their private timer and watchdog, and may not change the global timer
```
