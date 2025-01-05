#importonce
.print("AY-3-8910 registers")
// AY-3-8910 (DIGIMUZ) registers (http://plus4world.powweb.com/hardware/DIGIMUZ)
// write register number to address then read from read register or write to write register
.namespace ay {
	.label	read	= ay_base
	.label	write	= read + 1
	.label	address	= write + 1
}
