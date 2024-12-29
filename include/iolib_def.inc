#importonce
.print("Siz IOLib definitions")
.namespace iolib {

.enum { UNKNOWN, C1540, C1541, C1541C, C1541II, C1551, C1570, C1571, C1581, RF501C, SD2IEC, FLEXSD, FD2000, C16STM, YAPEIEC, TCBM2SD }
.enum { UNSUPPORTED, SERIAL, TCBM, PARALLEL, TCBMSTANDARD, IEEE488, SERIALBURST, EXPANSIONPORT }

// IOLib definitions to be included in programs where you use it
#if skip_default_definitions
#else
	#if HAS_TED
	.label	io_base		= $fc00
	#else
	.label	io_base		= $cf00
	#endif
	.label	io_data		= io_base-1
#endif
// Compress data to be loaded with "exomizer mem -f" command
// parameters: X: file name first byte, Y: file name second byte
.label	decrunch		= io_base - $150

// parameters: X: file name first byte, Y: file name second byte
.label	load			= io_base
.label	io_initloader	= io_base + 3

#if !skip_default_definitions
	// bit buffer for serial shift operations
	.label  io_bitbuff	= $b7
	// load pointer
	.label  io_loadptr  = $9e
	.label	io_tcbmoffs	= io_data
#endif
}
