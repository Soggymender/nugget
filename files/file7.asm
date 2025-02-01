			.ORIG x3000

MAIN        
			LD	R2, TERM
			LD	R3, ASCII

AGAIN       
			TRAP  x23
			ADD   R1, R2, R0
			BRz   EXIT
			ADD   R0, R0, R3
			TRAP  x21
			BRnzp AGAIN

TERM		.FILL xFFC9
ASCII       .FILL x0020
EXIT		
			TRAP  x25

			.END			