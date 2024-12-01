	#import	"kernal_table.inc"

	.encoding	"petscii_mixed"

.label	ptr	= 4

	BasicUpstart2(start)
start:	jsr	primm
	.text	"Hardware detect using IOLibV3 by Siz"
	.byte	14, 13, 0

	jsr	iolib.detect
	rts

primm:	pla
	sta	ptr
	pla
	sta	ptr+1
	ldy	#0
!:	inc	ptr
	bne	!+
	inc	ptr+1
!:	lda	(ptr),y
	beq	end
	jsr	chrout
	jmp	!--
end:	lda	ptr+1
	pha
	lda	ptr
	pha
	rts

.namespace fm {
	.label	address	= $df40
	.label	data	= address + $10
	.label	status	= address + $20
}

.namespace iolib {
	#define	prtstatus
	#define need_video_detect
	#define need_memory_detect
	#define need_sound_detect
}
#import "core/iolib.inc"
