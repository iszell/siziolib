#importonce
.print("Plus/4 I/O map")
// Standard CBM 264 series I/O ports 
// ---------------------------------

.label	cpu_dir		= $0
//  Bit Dir Expl.
//  7   In  Serial Data In (and PLUS/4: Cas Sense) (0=Low, 1=High)
//  6   In  Serial Clock In                        (0=Low, 1=High)
//  5   -   Not used (no pin-out on 7501/8501)     (N/A)
//  4   In  Cassette Read                          (0=Low, 1=High)
//  3   Out Cassette Motor                         (0=9VDC, 1=Off) (Inverted)
//  2   Out Serial ATN Out (and PLUS/4: User Port) (0=High, 1=Low) (Inverted)
//  1   Out Serial Clock Out, Cassette Write       (0=High, 1=Low) (Inverted)
//  0   Out Serial Data Out                        (0=High, 1=Low) (Inverted)
.label	cpu_port	= $1

.label	acia_base	= $fd00
.namespace	acia	{
	.label	data	= acia_base + 0
	// Read: status, Write: programmed reset (for any data)

	//  Bit7    IRq (1: was)
	//  Bit6    DSR
	//  Bit5    DCD
	//  Bit4    Transmit data register empty
	//  Bit3    Receive data register full
	//  Bit2    Overrun error
	//  Bit1    Framing error
	//  Bit0    Parity error
	.label	status	= acia_base + 1
	//  Bit7-5  Parity mode
	//  Bit4    Echo on/off
	//  Bit3-2  Transmitter controls
	//  Bit1    Interrupt disable (1: disabled)
	//  Bit0    Negated DTR
	.label	command	= acia_base + 2
	//  Bit7    Number of stop bits (0: 1, 1: depends on data length)
	//  Bit6-5  Data length (0: 8bit, 1: 7bit, 2: 6bit, 3: 5bit)
	//  Bit4    Receive clock source (0: external, 1: internal)
	//  Bit3-0  Baud rate
	.label	control	= acia_base + 3
}

.label	pio_parport 	= $fd10
.label	pio_keyboard 	= $fd30
.label	rom_bank_latch 	= $fdd0
	
.label	ted_base	= $ff00

// Expansions
// ----------

.label	tcbm_8		= $fef0
.label	tcbm_9		= $fec0
.namespace tcbm {
	.label	data	= tcbm_9 + 0
	//  Bit1    status 1
	//  Bit0    status 0
	.label	status	= tcbm_9 + 1
	//  Bit7    DAV
	//  Bit6    ACK
	.label	handshake	= tcbm_9 + 2
	// 1=out// 0=in
	.label	data_dir	= tcbm_9 + 3
	.label	status_dir	= tcbm_9 + 4
	.label	handshake_dir	= tcbm_9 + 5
}
	
// Custom expansions
// -----------------

// Csory RAM expansion control 
// Bit 7-6: main memory window select (00: $0000, 01: $4000, 10: $8000, 11: $c000)
//     5-4: unused (avaliable for 1M RAM expansion) 
//     3-0: 16k memory bank select to display in main memory window (16x16k = 256k)
// See also Hannes control register bit 6 
// Default value: $ff
.label	ramexpctrl_csory	= $fd15
// Hannes RAM expansion control
// Bit   7: expansion memory switch bound: 0:$1000, 1:$4000
//       6: expansion disable for TED: 0: access only default bank (11), 
//                                     1: access like CPU
//          affects Csory expansion too 
//     5-4: activate Csory expansion with 01, other values switch it off
//          address bits 21-20 for 4M Hannes RAM expansion
//     3-2: unused
//          address bits 19-18 for 1M Hannes RAM expansion 
//     1-0: 64k memory bank select 
// Default value: $ff
.label	ramexpctrl_hannes	= $fd16
// Hannes/Solder ROM expansion control
// Bit   7: 1: disable external cartridge ROMs, use internal ROM/SRAM expansion
//       6: 1: write protect SRAM bank $f, 0: SRAM bank $f write enabled between $4000 and $bfff
//       5: 1: write protect SRAM bank $a, 0: SRAM bank $a write enabled between $4000 and $bfff
//       4: 1: write protect SRAM bank $5, 0: SRAM bank $5 write enabled between $4000 and $bfff
//     3-2: unused
//     1-0: ROM/SRAM bank select (A15 and A14 for ROM banks)
// Default value: $ff
.label	romexpctrl	= $fd17

.label	sid_solder	= $fd40
.label	nae_control	= $fd80
.label	sid_csory	= $fe80

// AY-3-8910 (DIGIMUZ) registers (http://plus4world.powweb.com/hardware/DIGIMUZ)
// write register number to address then read from read register or write to write register
.namespace ay {
	.label	read	= $fd21
	.label	write	= read + 1
	.label	address	= write + 1
}

// SideKick NeoRAM (https://github.com/frntc/Sidekick64)
// block: select 16k block of memory (max. 4M)
// window: select 256 byte window within selected block
// page: bit 7 of ram address (select 128B block within window)
.namespace neoram {
	.label	ram	= $fe00
	.label	window	= $fde8
	.label	block	= window + 1
	.label	page	= block + 1
}

// SideKick OPL2 FM (https://github.com/frntc/Sidekick64)
// Write register number to address register, wait 6us, read/write data register and wait 35us
.namespace fm {
	.label	address	= $fde2
	.label	data	= address + 1
	.label	status	= address + 2
}

// C16STM registers (http://joco.homeserver.hu/c16stm/)
.namespace c16stm {
	.label	data	= $fdf0
	.label	status	= data + 1
}
