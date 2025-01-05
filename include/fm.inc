#importonce
.print("FM/OPL2 registers")
// Commodore Sound Expander/SideKick OPL2 FM (https://github.com/frntc/Sidekick64)
// Write register number to address register, wait 6us, read/write data register and wait 35us
.namespace fm {
	.label	address	= fm_base
	.label	data	= address + 1 * fm_reg_step
	.label	status	= address + 2 * fm_reg_step
}
