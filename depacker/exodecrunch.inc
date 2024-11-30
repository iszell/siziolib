//  Source modified for KickAss syntax by Siz.
	.namespace iolib {
	.namespace iolibdecrunch {
	
//
// Copyright (c) 2002 - 2020 Magnus Lind.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//   1. The origin of this software must not be misrepresented// you must not
//   claim that you wrote the original software. If you use this software in a
//   product, an acknowledgment in the product documentation would be
//   appreciated but is not required.
//
//   2. Altered source versions must be plainly marked as such, and must not
//   be misrepresented as being the original software.
//
//   3. This notice may not be removed or altered from any distribution.
//
//   4. The names of this software and/or it's copyright holders may not be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// -------------------------------------------------------------------
// Known quirks:
//  Can't handle a sequence reference that ends at $ffff. It is left in
//  since it is a corner case and fixing it impacts negatively on
//  performance or backwards compatibility.
//  A simple way to work around this is to not decrunch to address $ffff.
// -------------------------------------------------------------------
// Controls if the shared get_bits routines should be inlined or not.
//INLINE_GET_BITS=1
//Siz .IFNDEF INLINE_GET_BITS
//Siz INLINE_GET_BITS = 0
//Siz .ENDIF
// -------------------------------------------------------------------
// if literal sequences is not used (the data was crunched with the -c
// flag) then the following line can be uncommented for shorter and.
// slightly faster code.
//LITERAL_SEQUENCES_NOT_USED = 1
//Siz .IFNDEF LITERAL_SEQUENCES_NOT_USED
//Siz LITERAL_SEQUENCES_NOT_USED = 0
//Siz .ENDIF
// -------------------------------------------------------------------
// if the sequence length is limited to 256 (the data was crunched with
// the -M256 flag) then the following line can be uncommented for
// shorter and slightly faster code.
//MAX_SEQUENCE_LENGTH_256 = 1
//Siz .IFNDEF MAX_SEQUENCE_LENGTH_256
//Siz MAX_SEQUENCE_LENGTH_256 = 0
//Siz .ENDIF
// -------------------------------------------------------------------
// if the sequence length 3 has its own offset table (the data was
// crunched with the -P+16 flag) then the following
// line must be uncommented.
//EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE = 1
//Siz .IFNDEF EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE
//Siz EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE = 0
//Siz .ENDIF
// -------------------------------------------------------------------
// if sequence offsets are not reused (the data was crunched with the
// -P-32 flag) then the following line must be uncommented. Uncommenting the
// line will also result in shorter and slightly faster code.
//DONT_REUSE_OFFSET = 1
//Siz .IFNDEF DONT_REUSE_OFFSET
//Siz DONT_REUSE_OFFSET = 0
//Siz .ENDIF
// -------------------------------------------------------------------
// if decrunching forwards then the following line must be uncommented.
//DECRUNCH_FORWARDS = 1
#define DECRUNCH_FORWARDS //Siz
//Siz .IFNDEF DECRUNCH_FORWARDS
//Siz DECRUNCH_FORWARDS = 0
//Siz .ENDIF
// -------------------------------------------------------------------
// if split encoding is used (the data is crunched with the -E flag)
// then the following line must be uncommented.
//ENABLE_SPLIT_ENCODING = 1
//Siz .IFNDEF ENABLE_SPLIT_ENCODING
//Siz ENABLE_SPLIT_ENCODING = 0
//Siz .ENDIF

// -------------------------------------------------------------------
// The decruncher jsr:s to the get_crunched_byte address when it wants to
// read a crunched byte into A. This subroutine has to preserve X and Y
// register and must not modify the state of the carry nor the overflow flag.
// -------------------------------------------------------------------
//Siz .import get_crunched_byte
// -------------------------------------------------------------------
// This function is the heart of the decruncher. (for non split crunched files)
// It initializes the decruncher zeropage locations and precalculates the
// decrunch tables and decrunches the data
// This function will not change the interrupt status bit and it will not
// modify the memory configuration.
// -------------------------------------------------------------------
//Siz .export decrunch

// -------------------------------------------------------------------
// zero page addresses used
// -------------------------------------------------------------------
.label	zp_len_lo = $a7
.label	zp_len_hi = $a8

.label	zp_src_lo  = $ae
.label	zp_src_hi  = zp_src_lo + 1

.label	zp_bits_hi = $fc
#if !DONT_REUSE_OFFSET
.label	zp_ro_state = $a9
#endif

.label	zp_bitbuf  = $fd
.label	zp_dest_lo = zp_bitbuf + 1      // dest addr lo
.label	zp_dest_hi = zp_bitbuf + 2      // dest addr hi

#if EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE
.const	encoded_entries = 68
#else
.const	encoded_entries = 52
#endif

.label	decrunch_table = $ff40

.label	tabl_bi = decrunch_table
.label	tabl_lo = decrunch_table + encoded_entries
.label	tabl_hi = decrunch_table + encoded_entries * 2

	.macro exodecrunch(get_crunched_byte) {

#if ENABLE_SPLIT_ENCODING
// -------------------------------------------------------------------
// To decrunch files crunched with the split feature (-E) you can't use the
// decrunch function. Instead you call the split_decrunch function. But you
// can only do this if the decrunch table contains the encoding used by the
// file you are decrunching. To generate the correct content for the decrunch
// table call set the get_crunched_byte function to point to the encoding data
// and then call the split_gentable function.
// -------------------------------------------------------------------
.export split_gentable
.export split_decrunch
#endif

// -------------------------------------------------------------------
// no code below this comment has to be modified in order to generate
// a working decruncher of this source file.
// However, you may want to relocate the tables last in the file to a
// more suitable address.
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// jsr this label to decrunch, it will in turn init the tables and
// call the decruncher
// no constraints on register content, however the
// decimal flag has to be cleared (it almost always is, otherwise do a cld)
decrunch:
#if ENABLE_SPLIT_ENCODING
        ldx #3
        jsr internal_gentable
        jmp normal_decrunch
split_gentable:
        ldx #1
internal_gentable:
        jsr split_init_zp
#else
        ldx #3
        mac_init_zp(get_crunched_byte)
#endif
// -------------------------------------------------------------------
// calculate tables (64 bytes) + get_bits macro
// x must be #0 when entering
//
        ldy #0
        clc
table_gen:
        tax
        tya
        and #$0f
        sta tabl_lo,y
        beq shortcut            // start a new sequence
// -------------------------------------------------------------------
        txa
        adc tabl_lo - 1,y
        sta tabl_lo,y
        lda zp_len_hi
        adc tabl_hi - 1,y
shortcut:
        sta tabl_hi,y
// -------------------------------------------------------------------
        lda #$01
        sta <zp_len_hi
        lda #$78                // %01111000
        mac_get_bits(get_bits)
// -------------------------------------------------------------------
        lsr
        tax
        beq rolled
        php
rolle:
        asl zp_len_hi
        sec
        ror
        dex
        bne rolle
        plp
rolled:
        ror
        sta tabl_bi,y
        bmi no_fixup_lohi
        lda zp_len_hi
        stx zp_len_hi
        .byte $24
no_fixup_lohi:
        txa
// -------------------------------------------------------------------
        iny
        cpy #encoded_entries
        bne table_gen
// -------------------------------------------------------------------
#if ENABLE_SPLIT_ENCODING
        rts
split_decrunch:
        ldx #3
        jsr split_init_zp
// X reg must be 0 here
        sec
normal_decrunch:
#endif
// -------------------------------------------------------------------
// prepare for main decruncher
#if !DONT_REUSE_OFFSET
        ror zp_ro_state
        sec
#endif
        ldy zp_dest_lo
        stx zp_dest_lo
        stx zp_bits_hi
// -------------------------------------------------------------------
// copy one literal byte to destination (11 bytes)
//
literal_start1:
#if !DECRUNCH_FORWARDS
        tya
        bne no_hi_decr
        dec zp_dest_hi
#if !DONT_REUSE_OFFSET
        dec zp_src_hi
#endif
no_hi_decr:
        dey
#endif
        jsr get_crunched_byte
        sta (zp_dest_lo),y
#if DECRUNCH_FORWARDS
        iny
        bne skip_hi_incr
        inc zp_dest_hi
#if !DONT_REUSE_OFFSET
        inc zp_src_hi
#endif
skip_hi_incr:
#endif
// -------------------------------------------------------------------
// fetch sequence length index (15 bytes)
// x must be #0 when entering and contains the length index + 1
// when exiting or 0 for literal byte
next_round:
#if !DONT_REUSE_OFFSET
        ror zp_ro_state
#endif
        dex
        lda zp_bitbuf
no_literal1:
        asl
        bne nofetch8
        jsr get_crunched_byte
        rol
nofetch8:
        inx
        bcc no_literal1
        sta zp_bitbuf
// -------------------------------------------------------------------
// check for literal byte (2 bytes)
//
        beq literal_start1
// -------------------------------------------------------------------
// check for decrunch done and literal sequences (4 bytes)
//
        cpx #$11
#if INLINE_GET_BITS
        bcc skip_jmp
        jmp exit_or_lit_seq
skip_jmp:
#else
        bcs exit_or_lit_seq
#endif
// -------------------------------------------------------------------
// calulate length of sequence (zp_len) (18(11) bytes) + get_bits macro
//
        lda tabl_bi - 1,x
        mac_get_bits(get_bits)
        adc tabl_lo - 1,x       // we have now calculated zp_len_lo
        sta zp_len_lo
#if !MAX_SEQUENCE_LENGTH_256
        lda zp_bits_hi
        adc tabl_hi - 1,x       // c = 0 after this.
        sta zp_len_hi
// -------------------------------------------------------------------
// here we decide what offset table to use (27(26) bytes) + get_bits_nc macro
// z-flag reflects zp_len_hi here
//
        ldx zp_len_lo
#else
        tax
#endif
#if !MAX_SEQUENCE_LENGTH_256
        lda #0
#endif
#if !DONT_REUSE_OFFSET
// -------------------------------------------------------------------
// here we decide to reuse latest offset or not (13(15) bytes)
//
        bit <zp_ro_state
        bmi test_reuse
no_reuse:
#endif
// -------------------------------------------------------------------
// here we decide what offset table to use (17(15) bytes)
//
#if !MAX_SEQUENCE_LENGTH_256
        sta <zp_bits_hi
#endif
        lda #$e1
#if EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE
        cpx #$04
#else
        cpx #$03
#endif
        bcs gbnc2_next
        lda tabl_bit - 1,x
gbnc2_next:
        asl zp_bitbuf
        bne gbnc2_ok
        tax
        jsr get_crunched_byte
        rol
        sta zp_bitbuf
        txa
gbnc2_ok:
        rol
        bcs gbnc2_next
        tax
// -------------------------------------------------------------------
// calulate absolute offset (zp_src) (17 bytes) + get_bits macro
//
        lda tabl_bi,x
        mac_get_bits(get_bits)
#if !DECRUNCH_FORWARDS
        adc tabl_lo,x
        sta zp_src_lo
        lda zp_bits_hi
        adc tabl_hi,x
        adc zp_dest_hi
        sta zp_src_hi
#else
        clc
        adc tabl_lo,x
        eor #$ff
        sta zp_src_lo
        lda zp_bits_hi
        adc tabl_hi,x
        eor #$ff
        adc zp_dest_hi
        sta zp_src_hi
        clc
#endif
// -------------------------------------------------------------------
// prepare for copy loop (2 bytes)
//
        ldx zp_len_lo
// -------------------------------------------------------------------
// main copy loop (30 bytes)
//
copy_next:
#if !DECRUNCH_FORWARDS
        tya
        bne copy_skip_hi
        dec zp_dest_hi
        dec zp_src_hi
copy_skip_hi:
        dey
#endif
#if !LITERAL_SEQUENCES_NOT_USED
        bcs get_literal_byte
#endif
        lda (zp_src_lo),y
literal_byte_gotten:
        sta (zp_dest_lo),y
#if DECRUNCH_FORWARDS
        iny
        bne copy_skip_hi
        inc zp_dest_hi
        inc zp_src_hi
copy_skip_hi:
#endif
        dex
        bne copy_next
#if !MAX_SEQUENCE_LENGTH_256
        lda zp_len_hi
#if INLINE_GET_BITS
        bne copy_next_hi
#endif
#endif
        stx zp_bits_hi
#if !INLINE_GET_BITS
        beq next_round
#else
        jmp next_round
#endif
#if !MAX_SEQUENCE_LENGTH_256
copy_next_hi:
        dec zp_len_hi
        jmp copy_next
#endif
#if !DONT_REUSE_OFFSET
// -------------------------------------------------------------------
// test for offset reuse (11 bytes)
//
test_reuse:
        bvs no_reuse
#if MAX_SEQUENCE_LENGTH_256
        lda #$00                // fetch one bit
#endif
        asl zp_bitbuf
        bne gbnc1_ok
        pha
        jsr get_crunched_byte
        rol
        sta zp_bitbuf
        pla
gbnc1_ok:
        rol
        beq no_reuse            // bit == 0 => C=0, no reuse
        bne copy_next           // bit != 0 => C=0, reuse previous offset
#endif
// -------------------------------------------------------------------
// exit or literal sequence handling (16(12) bytes)
//
exit_or_lit_seq:
#if !LITERAL_SEQUENCES_NOT_USED
        beq decr_exit
        jsr get_crunched_byte
#if !MAX_SEQUENCE_LENGTH_256
        sta zp_len_hi
#endif
        jsr get_crunched_byte
        tax
        bcs copy_next
decr_exit:
#endif
        rts
#if !LITERAL_SEQUENCES_NOT_USED
get_literal_byte:
        jsr get_crunched_byte
        bcs literal_byte_gotten
#endif
#if EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE
// -------------------------------------------------------------------
// the static stable used for bits+offset for lengths 1, 2 and 3 (3 bytes)
// bits 2, 4, 4 and offsets 64, 48, 32 corresponding to
// %10010000, %11100011, %11100010
tabl_bit:
        .byte $90, $e3, $e2
#else
// -------------------------------------------------------------------
// the static stable used for bits+offset for lengths 1 and 2 (2 bytes)
// bits 2, 4 and offsets 48, 32 corresponding to %10001100, %11100010
tabl_bit:
        .byte $8c, $e2
#endif

#if ENABLE_SPLIT_ENCODING
split_init_zp:
        mac_init_zp
        rts
#endif

#if !INLINE_GET_BITS
get_bits:
        adc #$80                // needs c=0, affects v
        asl
        bpl gb_skip
gb_next:
        asl zp_bitbuf
        bne gb_ok
        mac_refill_bits(get_crunched_byte, zp_bitbuf)
gb_ok:
        rol
        bmi gb_next
gb_skip:
        bvs gb_get_hi
        rts
gb_get_hi:
        sec
        sta zp_bits_hi
        jmp get_crunched_byte
#endif
// -------------------------------------------------------------------
// end of decruncher
// -------------------------------------------------------------------
}

        //// refill bits is always inlined
.macro mac_refill_bits(get_crunched_byte, zp_bitbuf) {
        pha
        jsr get_crunched_byte
        rol
        sta zp_bitbuf
        pla
}

.macro mac_get_bits(get_bits) {
#if INLINE_GET_BITS
{
        adc #$80                // needs c=0, affects v
        asl
        bpl gb_skip
gb_next:
        asl zp_bitbuf
        bne gb_ok
        mac_refill_bits
gb_ok:
        rol
        bmi gb_next
gb_skip:
        bvc skip
gb_get_hi:
        sec
        sta zp_bits_hi
        jsr get_crunched_byte
skip:
}
#else
        jsr get_bits
#endif
}

.macro mac_init_zp(get_crunched_byte) {
// -------------------------------------------------------------------
// init zeropage and x reg. (8 bytes)
//
init_zp:
        jsr get_crunched_byte
        sta zp_bitbuf - 1,x
        dex
        bne init_zp
}

// -------------------------------------------------------------------
// this 156 (204) byte table area may be relocated. It may also be
// clobbered by other data between decrunches.
// -------------------------------------------------------------------
//Siz decrunch_table:
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz #if EXTRA_TABLE_ENTRY_FOR_LENGTH_THREE
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz #endif
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//Siz         .byte 0,0,0,0,0,0,0,0,0,0,0,0
// -------------------------------------------------------------------
// end of decruncher
// -------------------------------------------------------------------
}
}
