#importonce
.print("C64/C128 I/O map")
// Standard C64 I/O ports 
// ---------------------------------

.label	cpu_dir		= $0
.label	cpu_port	= $1

.label	vic_base	= $d000
.label	sid_base	= $d400
.label	colorram	= $d800
.label	cia1_base	= $dc00
.label	cia2_base	= $dd00
.label	io1			= $de00
.label	io2			= $df00

// Standard C128 I/O ports 
// ---------------------------------
.label	mmu_base	= $d500
.label	vdc_base	= $d600

// Expansions
// ---------------------------------
#define HAS_FM
.label	fm_base		= io2+$40
.const	fm_reg_step	= $10
