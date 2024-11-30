#importonce
.print "Plus/4 BASIC header"
	* = $1001 "BASIC header"

{
	.word basend
	.word 2024
	.byte $9e	// BASIC SYS token
	.text "4109"
	.byte 0
basend:	.word 0
}
	* = $100d "Code"
