.section ".vectors", "ax", %progbits
	b _start		//RESET vector
	b .
	b .
	b .
	b .
	b .
	b IRQ_Handler 	//IRQ Vector
	b .

.section  ".init", "ax", %progbits

.func       _lowlevel_init
.global     _lowlevel_init
_lowlevel_init:
            /* Enable instruction cache */
            mrc     p15,0,r0,c1,c0,0
            orr     r0,r0,#(1<<12)
            mcr     p15,0,r0,c1,c0,0

            /* Setup CPU clocks */

            ldr     r0,=0x0021b000

            mov     r1, #0x15
            str     r1, [r0, #0x20]

            /* MPLL => 192 MHz */
            ldr     r1,=0x07b32da5
            str     r1, [r0, #0x04]

            mov     r1,#0
            str     r1, [r0, #0x08]

            /* Reset MPLL */
            ldr     r1,=0x0f20ac03
            str     r1,[r0]

            /* SPLL => 96 MHz */
            ldr     r1,=0x0fb32da5
            str     r1,[r0,#0x0c]

            mov     r1,#0
            str     r1,[r0,#0x10]

            ldr     r1,=0x0f40ac03
            str     r1,[r0]

            /* Wait for SPLL to stabilize */
1:          ldr     r1,[r0,#0x10]
            tst     r1,#(1<<15)
            beq     1b

            ldr     r1,=0x27000003
            str     r1,[r0]

            ldr     r1,=0x3ab
            str     r1,[r0,#0x80c]

            /* Enable PLL clock */
            mrc     p15,0,r0,c1,c0,0
            orr     r0,r0,#0xc0000000
            mcr     p15,0,r0,c1,c0,0

            /* Setup Flash: remap CS0 to 0x10000000 */
            mov     r0,#0x00220000
            ldr     r1,=0x00330e04
            str     r1,[r0]

            ldr     r1,=0xd01
            str     r1,[r0,#4]

            /* Setup SDRAM timings */

            /* Useful definitions */
.equ        PRE_ALL_CMD,        0x10000000
.equ        AUTO_REF_CMD,       0x20000000
.equ        SET_MODE_REG_CMD,   0x30000000
.equ        SDCTL_BASE_VAL,     0x810a9267
.equ        MODE_REG_VAL,       0x00223000
.equ        SBANK_BASE,         0x08000000
.equ        SDCTL,              0x00221000
.equ        SDRAM_SIZE,         16*1024*1024    @ 16M

            /* Skip SDRAM initialisation if we're running in SDRAM */
            sub     r0,pc,#SBANK_BASE
            cmp     r0,#SDRAM_SIZE
            blo     skip_sdram_setup

sdram_setup:
            ldr     r0,=SDCTL
            mov     r1,#SBANK_BASE
            ldr     r2,=(SDCTL_BASE_VAL | PRE_ALL_CMD)
            str     r2,[r0]
            ldr     r2,[r1]

            ldr     r2,=(SDCTL_BASE_VAL | AUTO_REF_CMD)
            str     r2,[r0]
            mov     r3,#8
1:          ldr     r2,[r1]
            subs    r3,r3,#1
            bne     1b

            ldr     r2,=(SDCTL_BASE_VAL | SET_MODE_REG_CMD)
            str     r2,[r0]
            ldr     r2,=MODE_REG_VAL
            orr     r1,r1,r2
            ldr     r2,[r1]
            ldr     r2,=SDCTL_BASE_VAL
            str     r2,[r0]

skip_sdram_setup:
            bx      lr
.endfunc


.global _start
_start:
	bl _lowlevel_init

	ldr r0,=START_BSS
	ldr r1,=END_BSS
	mov r2,#0
boucle:
	cmp r0,r1
	beq suite
	str r2,[r0],#4
	b boucle

suite:

	ldr r0,=START_DATA
	ldr r1,=END_DATA
	ldr r2,=START_FLASH_DATA
boucle2:
	cmp r0,r1
	beq suite2
	ldr r3,[r2],#4
	str r3,[r0],#4
	b boucle2

suite2:

	mrs r0, CPSR      @ Copie CPSR dans r0
	bic r0, r0, #0x1f @ Met à 0 les 5 bits M
	orr r0, r0, #0x12 @ et change vers le mode irq
	msr CPSR_c, r0    @ Recharge les bits de contrôle
	nop               @ de CPSR

	ldr sp ,=0x8EFFFF0

	mrs r0, CPSR      @ Copie CPSR dans r0
	bic r0, r0, #0x1f @ Met à 0 les 5 bits M
	orr r0, r0, #0x13 @ et change vers le mode superviseur
	msr CPSR_c, r0    @ Recharge les bits de contrôle
	nop               @ de CPSR

	ldr sp, =0x8FFFFF0

	bl main

	b .
