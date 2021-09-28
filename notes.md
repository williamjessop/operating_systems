Important reference https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms

The frame pointer is the x29 register
The link register is the x30 register (this is used as a return address register)

These 3 lines are important!!!
	sub	sp, sp, #32                     ; =32
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16                    ; =16

This will store the frame pointer and the return address on the stack for safe keeping. It will also leave the stack pointer where it was when the function was loaded.

These 2 lines will reload the frame pointer and the link register. After doing so it will also move the stack pointer to where it needs to be.
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #32                     ; =32

The frame pointer and link register must be restored before calling ret

