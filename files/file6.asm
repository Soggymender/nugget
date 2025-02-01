	    .ORIG x3000

        LD  R0, VALUE
        STI R0, F001

		LD  R0, ONE
		STI R0, F000

VALUE   .FILL x00B3       ; Value to be loaded (0xB3)
F001	.FILL xF001       ; Address 0xF000 (for reference, could be omitted)

ONE     .FILL x0001
F000	.FILL xF000

        .END 