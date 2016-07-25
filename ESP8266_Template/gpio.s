	.file	"gpio.c"
	.text
.Ltext0:
	.literal_position
	.literal .LC0, 1610613508
	.literal .LC1, 1610613512
	.literal .LC2, 1610613520
	.literal .LC3, 1610613524
	.align	4
	.global	gpio_output_conf
	.type	gpio_output_conf, @function
gpio_output_conf:
.LFB1:
	.file 1 "libraries/gpio.c"
	.loc 1 98 0
.LVL0:
	.loc 1 99 0
	l32r	a6, .LC0
	s32i.n	a2, a6, 0
	.loc 1 100 0
	l32r	a2, .LC1
.LVL1:
	s32i.n	a3, a2, 0
.LVL2:
	.loc 1 101 0
	l32r	a2, .LC2
	s32i.n	a4, a2, 0
	.loc 1 102 0
	l32r	a2, .LC3
	s32i.n	a5, a2, 0
	ret.n
.LFE1:
	.size	gpio_output_conf, .-gpio_output_conf
	.literal_position
	.literal .LC4, 1610613528
	.align	4
	.global	gpio_input_get
	.type	gpio_input_get, @function
gpio_input_get:
.LFB2:
	.loc 1 109 0
	.loc 1 110 0
	l32r	a2, .LC4
	l32i.n	a2, a2, 0
	.loc 1 111 0
	ret.n
.LFE2:
	.size	gpio_input_get, .-gpio_input_get
	.literal_position
	.align	4
	.global	gpio_intr_handler_register
	.type	gpio_intr_handler_register, @function
gpio_intr_handler_register:
.LFB3:
	.loc 1 125 0
.LVL3:
	addi	sp, sp, -16
.LCFI0:
	.loc 1 126 0
	mov.n	a4, a3
	mov.n	a3, a2
.LVL4:
	movi.n	a2, 4
.LVL5:
	.loc 1 125 0
	s32i.n	a0, sp, 12
.LCFI1:
	.loc 1 126 0
	call0	_xt_isr_attach
.LVL6:
	.loc 1 127 0
	l32i.n	a0, sp, 12
	addi	sp, sp, 16
	ret.n
.LFE3:
	.size	gpio_intr_handler_register, .-gpio_intr_handler_register
	.literal_position
	.literal .LC5, 402653386
	.align	4
	.global	gpio_pin_wakeup_enable
	.type	gpio_pin_wakeup_enable, @function
gpio_pin_wakeup_enable:
.LFB4:
	.loc 1 133 0
.LVL7:
	addi	sp, sp, -16
.LCFI2:
	s32i.n	a12, sp, 8
	s32i.n	a13, sp, 4
	s32i.n	a0, sp, 12
.LCFI3:
	.loc 1 133 0
	mov.n	a13, a2
	.loc 1 136 0
	addi	a2, a3, -4
.LVL8:
	.loc 1 133 0
	mov.n	a12, a3
	.loc 1 136 0
	bgeui	a2, 2, .L4
	.loc 1 137 0
	call0	vPortEnterCritical
.LVL9:
	.loc 1 139 0
	l32r	a4, .LC5
	.loc 1 142 0
	slli	a12, a12, 7
.LVL10:
	.loc 1 139 0
	add.n	a2, a13, a4
	slli	a2, a2, 2
	l32i.n	a3, a2, 0
.LVL11:
	.loc 1 140 0
	movi	a4, -0x381
	and	a4, a3, a4
.LVL12:
	.loc 1 142 0
	movi	a3, 0x400
	or	a12, a12, a3
	or	a3, a12, a4
.LVL13:
	.loc 1 143 0
	s32i.n	a3, a2, 0
	.loc 1 145 0
	call0	vPortExitCritical
.LVL14:
.L4:
	.loc 1 147 0
	l32i.n	a0, sp, 12
	l32i.n	a12, sp, 8
	l32i.n	a13, sp, 4
.LVL15:
	addi	sp, sp, 16
	ret.n
.LFE4:
	.size	gpio_pin_wakeup_enable, .-gpio_pin_wakeup_enable
	.literal_position
	.literal .LC6, 1610613544
	.literal .LC7, 1610613608
	.align	4
	.global	gpio_pin_wakeup_disable
	.type	gpio_pin_wakeup_disable, @function
gpio_pin_wakeup_disable:
.LFB5:
	.loc 1 150 0
.LVL16:
	.loc 1 150 0
	l32r	a2, .LC6
	.loc 1 154 0
	l32r	a4, .LC7
	.loc 1 157 0
	movi	a5, 0x400
	.loc 1 160 0
	movi	a6, -0x781
.LVL17:
.L8:
	.loc 1 155 0
	l32i.n	a3, a2, 0
.LVL18:
	.loc 1 157 0
	bnone	a3, a5, .L7
.LVL19:
	.loc 1 160 0
	and	a3, a3, a6
.LVL20:
	.loc 1 161 0
	s32i.n	a3, a2, 0
.L7:
	addi.n	a2, a2, 4
	.loc 1 154 0 discriminator 2
	bne	a2, a4, .L8
	.loc 1 164 0
	ret.n
.LFE5:
	.size	gpio_pin_wakeup_disable, .-gpio_pin_wakeup_disable
	.literal_position
	.literal .LC8, 402653386
	.align	4
	.global	gpio_pin_intr_state_set
	.type	gpio_pin_intr_state_set, @function
gpio_pin_intr_state_set:
.LFB6:
	.loc 1 167 0
.LVL21:
	addi	sp, sp, -16
.LCFI4:
	s32i.n	a0, sp, 12
	s32i.n	a12, sp, 8
	s32i.n	a13, sp, 4
.LCFI5:
	.loc 1 167 0
	mov.n	a12, a3
	mov.n	a13, a2
	.loc 1 170 0
	call0	vPortEnterCritical
.LVL22:
	.loc 1 172 0
	l32r	a4, .LC8
	add.n	a2, a13, a4
	slli	a2, a2, 2
	l32i.n	a3, a2, 0
.LVL23:
	.loc 1 173 0
	movi	a4, -0x381
	and	a4, a3, a4
.LVL24:
	.loc 1 174 0
	slli	a3, a12, 7
	or	a3, a3, a4
.LVL25:
	.loc 1 175 0
	s32i.n	a3, a2, 0
	.loc 1 177 0
	call0	vPortExitCritical
.LVL26:
	.loc 1 178 0
	l32i.n	a0, sp, 12
	l32i.n	a12, sp, 8
.LVL27:
	l32i.n	a13, sp, 4
.LVL28:
	addi	sp, sp, 16
	ret.n
.LFE6:
	.size	gpio_pin_intr_state_set, .-gpio_pin_intr_state_set
	.literal_position
	.literal .LC9, 1610613544
	.literal .LC10, 1610614808
	.literal .LC11, 1610614836
	.literal .LC12, 1610614840
	.literal .LC13, 1610614804
	.literal .LC14, 1610614844
	.literal .LC15, 1610614848
	.literal .LC16, 1610614812
	.literal .LC17, 1610614816
	.literal .LC18, 1610614820
	.literal .LC19, 1610614824
	.literal .LC20, 1610614828
	.literal .LC21, 1610614832
	.literal .LC22, 1610614788
	.literal .LC23, 1610614792
	.literal .LC24, 1610614800
	.literal .LC25, 1610614796
	.literal .LC26, 1610613508
	.literal .LC27, 1610613512
	.literal .LC28, 1610613520
	.literal .LC29, 1610613524
	.literal .LC30, 1610613608
	.literal .LC31, 65536
	.align	4
	.global	gpio_config
	.type	gpio_config, @function
gpio_config:
.LFB0:
	.loc 1 31 0
.LVL29:
	addi	sp, sp, -32
.LCFI6:
	s32i.n	a13, sp, 20
	s32i.n	a14, sp, 16
.LCFI7:
	.loc 1 32 0
	l16ui	a13, a2, 0
.LVL30:
	.loc 1 31 0
	mov.n	a14, a2
	.loc 1 37 0
	l32i.n	a2, a2, 4
.LVL31:
	.loc 1 31 0
	s32i.n	a0, sp, 28
	s32i.n	a12, sp, 24
	s32i.n	a15, sp, 12
.LCFI8:
	.loc 1 37 0
	bnez.n	a2, .L15
.LVL32:
.LBB6:
.LBB7:
	.loc 1 99 0
	l32r	a3, .LC26
	s32i.n	a2, a3, 0
	.loc 1 100 0
	l32r	a3, .LC27
	s32i.n	a2, a3, 0
	.loc 1 101 0
	l32r	a3, .LC28
	s32i.n	a2, a3, 0
	.loc 1 102 0
	l32r	a2, .LC29
	s32i.n	a13, a2, 0
	j	.L16
.LVL33:
.L15:
.LBE7:
.LBE6:
	.loc 1 39 0
	bnei	a2, 2, .L16
.LVL34:
.LBB8:
.LBB9:
	.loc 1 99 0
	l32r	a3, .LC26
	movi.n	a2, 0
	s32i.n	a2, a3, 0
	.loc 1 100 0
	l32r	a3, .LC27
	s32i.n	a2, a3, 0
	.loc 1 101 0
	l32r	a3, .LC28
	s32i.n	a13, a3, 0
	.loc 1 102 0
	l32r	a3, .LC29
	s32i.n	a2, a3, 0
.LVL35:
.L16:
.LBE9:
.LBE8:
	.loc 1 31 0
	l32r	a12, .LC9
	movi.n	a15, 0
.LVL36:
.L25:
	.loc 1 44 0
	bbc	a13, a15, .L17
	extui	a3, a15, 0, 8
	.loc 1 45 0
	l32r	a2, .LC11
	beqz.n	a15, .L18
	.loc 1 45 0 is_stmt 0 discriminator 1
	l32r	a2, .LC10
	beqi	a3, 1, .L18
	.loc 1 45 0 discriminator 3
	l32r	a2, .LC12
	beqi	a3, 2, .L18
	.loc 1 45 0 discriminator 5
	l32r	a2, .LC13
	beqi	a3, 3, .L18
	.loc 1 45 0 discriminator 7
	l32r	a2, .LC14
	beqi	a3, 4, .L18
	.loc 1 45 0 discriminator 9
	l32r	a2, .LC15
	beqi	a3, 5, .L18
	.loc 1 45 0 discriminator 11
	l32r	a2, .LC16
	beqi	a3, 6, .L18
	.loc 1 45 0 discriminator 13
	l32r	a2, .LC17
	beqi	a3, 7, .L18
	.loc 1 45 0 discriminator 15
	l32r	a2, .LC18
	beqi	a3, 8, .L18
	.loc 1 45 0 discriminator 17
	movi.n	a5, 9
	l32r	a2, .LC19
	beq	a3, a5, .L18
	.loc 1 45 0 discriminator 19
	l32r	a2, .LC20
	beqi	a3, 10, .L18
	.loc 1 45 0 discriminator 21
	movi.n	a5, 0xb
	l32r	a2, .LC21
	beq	a3, a5, .L18
	.loc 1 45 0 discriminator 23
	l32r	a2, .LC22
	beqi	a3, 12, .L18
	.loc 1 45 0 discriminator 25
	movi.n	a5, 0xd
	l32r	a2, .LC23
	beq	a3, a5, .L18
	.loc 1 45 0 discriminator 27
	l32r	a2, .LC25
	l32r	a5, .LC24
	addi	a3, a3, -14
	movnez	a2, a5, a3
.L18:
.LVL37:
	.loc 1 47 0 is_stmt 1 discriminator 60
	movi.n	a5, 0x35
	movi	a3, -0x131
	bbc	a5, a15, .L19
	.loc 1 48 0
	l32i.n	a5, a2, 0
	and	a3, a5, a3
	s32i.n	a3, a2, 0
	l32i.n	a3, a2, 0
	j	.L56
.L19:
	.loc 1 50 0
	l32i.n	a5, a2, 0
	and	a3, a5, a3
	s32i.n	a3, a2, 0
	l32i.n	a5, a2, 0
	movi.n	a3, 0x30
	or	a3, a5, a3
.L56:
	s32i.n	a3, a2, 0
	.loc 1 53 0
	l32i.n	a3, a14, 8
	.loc 1 54 0
	l32i.n	a5, a2, 0
	.loc 1 53 0
	beqz.n	a3, .L21
	.loc 1 54 0
	movi	a3, 0x80
	or	a3, a5, a3
	j	.L57
.L21:
	.loc 1 56 0
	movi	a3, -0x81
	and	a3, a5, a3
.L57:
	s32i.n	a3, a2, 0
	.loc 1 59 0
	l32i.n	a2, a14, 4
.LVL38:
	bnei	a2, 1, .L23
	.loc 1 60 0
	call0	vPortEnterCritical
.LVL39:
	.loc 1 62 0
	l32i.n	a3, a12, 0
.LVL40:
	.loc 1 64 0
	movi.n	a2, 4
	or	a2, a3, a2
.LVL41:
	.loc 1 65 0
	s32i.n	a2, a12, 0
	j	.L58
.LVL42:
.L23:
	.loc 1 68 0
	bnei	a2, 3, .L24
	.loc 1 69 0
	call0	vPortEnterCritical
.LVL43:
	.loc 1 71 0
	l32i.n	a3, a12, 0
.LVL44:
	.loc 1 72 0
	movi.n	a2, -2
	and	a2, a3, a2
.LVL45:
	.loc 1 73 0
	movi.n	a3, 1
	or	a2, a2, a3
.LVL46:
	.loc 1 74 0
	s32i.n	a2, a12, 0
	.loc 1 75 0
	l32r	a3, .LC31
	l32r	a2, .LC30
.LVL47:
	s32i.n	a3, a2, 0
.LVL48:
.L58:
	.loc 1 77 0
	call0	vPortExitCritical
.LVL49:
.L24:
	.loc 1 80 0
	l32i.n	a3, a14, 12
	mov.n	a2, a15
	call0	gpio_pin_intr_state_set
.LVL50:
.L17:
	addi.n	a15, a15, 1
.LVL51:
	addi.n	a12, a12, 4
	.loc 1 84 0
	bnei	a15, 16, .L25
	.loc 1 85 0
	l32i.n	a0, sp, 28
	l32i.n	a12, sp, 24
	l32i.n	a13, sp, 20
.LVL52:
	l32i.n	a14, sp, 16
.LVL53:
	l32i.n	a15, sp, 12
.LVL54:
	addi	sp, sp, 32
	ret.n
.LFE0:
	.size	gpio_config, .-gpio_config
	.literal_position
	.literal .LC32, 1610614688
	.literal .LC33, 1610614672
	.literal .LC34, 1610614644
	.align	4
	.global	gpio16_output_conf
	.type	gpio16_output_conf, @function
gpio16_output_conf:
.LFB7:
	.loc 1 181 0
	.loc 1 182 0
	l32r	a4, .LC32
	movi	a2, -0x44
	l32i.n	a3, a4, 0
	and	a2, a3, a2
	movi.n	a3, 1
	or	a2, a2, a3
	s32i.n	a2, a4, 0
	.loc 1 185 0
	l32r	a4, .LC33
	movi.n	a2, -2
	l32i.n	a5, a4, 0
	and	a2, a5, a2
	s32i.n	a2, a4, 0
	.loc 1 188 0
	l32r	a2, .LC34
	l32i.n	a4, a2, 0
	or	a3, a4, a3
	s32i.n	a3, a2, 0
	ret.n
.LFE7:
	.size	gpio16_output_conf, .-gpio16_output_conf
	.literal_position
	.literal .LC35, 1610614632
	.align	4
	.global	gpio16_output_set
	.type	gpio16_output_set, @function
gpio16_output_set:
.LFB8:
	.loc 1 193 0
.LVL55:
	.loc 1 194 0
	l32r	a4, .LC35
	movi.n	a3, -2
	l32i.n	a5, a4, 0
	extui	a2, a2, 0, 1
.LVL56:
	and	a3, a5, a3
	or	a2, a3, a2
	s32i.n	a2, a4, 0
	ret.n
.LFE8:
	.size	gpio16_output_set, .-gpio16_output_set
	.literal_position
	.literal .LC36, 1610614688
	.literal .LC37, 1610614672
	.literal .LC38, 1610614644
	.align	4
	.global	gpio16_input_conf
	.type	gpio16_input_conf, @function
gpio16_input_conf:
.LFB9:
	.loc 1 199 0
	.loc 1 200 0
	l32r	a4, .LC36
	movi	a2, -0x44
	l32i.n	a3, a4, 0
	and	a3, a3, a2
	movi.n	a2, 1
	or	a2, a3, a2
	s32i.n	a2, a4, 0
	.loc 1 203 0
	l32r	a4, .LC37
	movi.n	a2, -2
	l32i.n	a3, a4, 0
	and	a3, a3, a2
	s32i.n	a3, a4, 0
	.loc 1 206 0
	l32r	a3, .LC38
	l32i.n	a4, a3, 0
	and	a2, a4, a2
	s32i.n	a2, a3, 0
	ret.n
.LFE9:
	.size	gpio16_input_conf, .-gpio16_input_conf
	.literal_position
	.literal .LC39, 1610614668
	.align	4
	.global	gpio16_input_get
	.type	gpio16_input_get, @function
gpio16_input_get:
.LFB10:
	.loc 1 211 0
	.loc 1 212 0
	l32r	a2, .LC39
	l32i.n	a2, a2, 0
	.loc 1 213 0
	extui	a2, a2, 0, 1
	ret.n
.LFE10:
	.size	gpio16_input_get, .-gpio16_input_get
	.section	.debug_frame,"",@progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x3
	.string	""
	.uleb128 0x1
	.sleb128 -4
	.uleb128 0
	.byte	0xc
	.uleb128 0x1
	.uleb128 0
	.align	4
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.align	4
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.align	4
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.byte	0x4
	.4byte	.LCFI0-.LFB3
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0x80
	.uleb128 0x1
	.align	4
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.byte	0x4
	.4byte	.LCFI2-.LFB4
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI3-.LCFI2
	.byte	0x8c
	.uleb128 0x2
	.byte	0x8d
	.uleb128 0x3
	.byte	0x80
	.uleb128 0x1
	.align	4
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.align	4
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.byte	0x4
	.4byte	.LCFI4-.LFB6
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI5-.LCFI4
	.byte	0x80
	.uleb128 0x1
	.byte	0x8c
	.uleb128 0x2
	.byte	0x8d
	.uleb128 0x3
	.align	4
.LEFDE10:
.LSFDE12:
	.4byte	.LEFDE12-.LASFDE12
.LASFDE12:
	.4byte	.Lframe0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.byte	0x4
	.4byte	.LCFI6-.LFB0
	.byte	0xe
	.uleb128 0x20
	.byte	0x4
	.4byte	.LCFI7-.LCFI6
	.byte	0x8d
	.uleb128 0x3
	.byte	0x8e
	.uleb128 0x4
	.byte	0x4
	.4byte	.LCFI8-.LCFI7
	.byte	0x80
	.uleb128 0x1
	.byte	0x8c
	.uleb128 0x2
	.byte	0x8f
	.uleb128 0x5
	.align	4
.LEFDE12:
.LSFDE14:
	.4byte	.LEFDE14-.LASFDE14
.LASFDE14:
	.4byte	.Lframe0
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.align	4
.LEFDE14:
.LSFDE16:
	.4byte	.LEFDE16-.LASFDE16
.LASFDE16:
	.4byte	.Lframe0
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.align	4
.LEFDE16:
.LSFDE18:
	.4byte	.LEFDE18-.LASFDE18
.LASFDE18:
	.4byte	.Lframe0
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.align	4
.LEFDE18:
.LSFDE20:
	.4byte	.LEFDE20-.LASFDE20
.LASFDE20:
	.4byte	.Lframe0
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.align	4
.LEFDE20:
	.text
.Letext0:
	.file 2 "c:\\espressif\\xtensa-lx106-elf\\xtensa-lx106-elf\\include\\stdint.h"
	.file 3 "c:/Espressif/ESP8266_RTOS_SDK/include/espressif/c_types.h"
	.file 4 "c:/Espressif/ESP8266_RTOS_SDK/include/freertos/portmacro.h"
	.file 5 "libraries/gpio.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.4byte	0x4ee
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF66
	.byte	0x1
	.4byte	.LASF67
	.4byte	.LASF68
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x3
	.4byte	.LASF3
	.byte	0x2
	.byte	0x2a
	.4byte	0x37
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x3
	.4byte	.LASF4
	.byte	0x2
	.byte	0x36
	.4byte	0x50
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.4byte	.LASF6
	.byte	0x2
	.byte	0x51
	.4byte	0x69
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF8
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF9
	.uleb128 0x3
	.4byte	.LASF10
	.byte	0x3
	.byte	0x2b
	.4byte	0x2c
	.uleb128 0x3
	.4byte	.LASF11
	.byte	0x3
	.byte	0x30
	.4byte	0x45
	.uleb128 0x3
	.4byte	.LASF12
	.byte	0x3
	.byte	0x34
	.4byte	0x5e
	.uleb128 0x2
	.byte	0x4
	.byte	0x4
	.4byte	.LASF13
	.uleb128 0x2
	.byte	0x8
	.byte	0x4
	.4byte	.LASF14
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF15
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF16
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF17
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF18
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.4byte	.LASF19
	.uleb128 0x6
	.4byte	0xdd
	.uleb128 0x7
	.4byte	0xb4
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0xd2
	.uleb128 0x3
	.4byte	.LASF20
	.byte	0x4
	.byte	0xb8
	.4byte	0xdd
	.uleb128 0x9
	.byte	0x4
	.4byte	0x69
	.byte	0x5
	.byte	0x5c
	.4byte	0x11f
	.uleb128 0xa
	.4byte	.LASF21
	.byte	0
	.uleb128 0xa
	.4byte	.LASF22
	.byte	0x1
	.uleb128 0xa
	.4byte	.LASF23
	.byte	0x2
	.uleb128 0xa
	.4byte	.LASF24
	.byte	0x3
	.uleb128 0xa
	.4byte	.LASF25
	.byte	0x4
	.uleb128 0xa
	.4byte	.LASF26
	.byte	0x5
	.byte	0
	.uleb128 0x3
	.4byte	.LASF27
	.byte	0x5
	.byte	0x63
	.4byte	0xee
	.uleb128 0x9
	.byte	0x4
	.4byte	0x69
	.byte	0x5
	.byte	0x65
	.4byte	0x14f
	.uleb128 0xa
	.4byte	.LASF28
	.byte	0
	.uleb128 0xa
	.4byte	.LASF29
	.byte	0x1
	.uleb128 0xa
	.4byte	.LASF30
	.byte	0x2
	.uleb128 0xa
	.4byte	.LASF31
	.byte	0x3
	.byte	0
	.uleb128 0x3
	.4byte	.LASF32
	.byte	0x5
	.byte	0x6a
	.4byte	0x12a
	.uleb128 0x9
	.byte	0x4
	.4byte	0x69
	.byte	0x5
	.byte	0x6c
	.4byte	0x173
	.uleb128 0xa
	.4byte	.LASF33
	.byte	0
	.uleb128 0xa
	.4byte	.LASF34
	.byte	0x1
	.byte	0
	.uleb128 0x3
	.4byte	.LASF35
	.byte	0x5
	.byte	0x6f
	.4byte	0x15a
	.uleb128 0xb
	.byte	0x10
	.byte	0x5
	.byte	0x71
	.4byte	0x1b7
	.uleb128 0xc
	.4byte	.LASF36
	.byte	0x5
	.byte	0x72
	.4byte	0x89
	.byte	0
	.uleb128 0xc
	.4byte	.LASF37
	.byte	0x5
	.byte	0x73
	.4byte	0x14f
	.byte	0x4
	.uleb128 0xc
	.4byte	.LASF38
	.byte	0x5
	.byte	0x74
	.4byte	0x173
	.byte	0x8
	.uleb128 0xc
	.4byte	.LASF39
	.byte	0x5
	.byte	0x75
	.4byte	0x11f
	.byte	0xc
	.byte	0
	.uleb128 0x3
	.4byte	.LASF40
	.byte	0x5
	.byte	0x76
	.4byte	0x17e
	.uleb128 0xd
	.4byte	.LASF62
	.byte	0x1
	.byte	0x61
	.byte	0x1
	.4byte	0x1fb
	.uleb128 0xe
	.4byte	.LASF41
	.byte	0x1
	.byte	0x61
	.4byte	0x94
	.uleb128 0xe
	.4byte	.LASF42
	.byte	0x1
	.byte	0x61
	.4byte	0x94
	.uleb128 0xe
	.4byte	.LASF43
	.byte	0x1
	.byte	0x61
	.4byte	0x94
	.uleb128 0xe
	.4byte	.LASF44
	.byte	0x1
	.byte	0x61
	.4byte	0x94
	.byte	0
	.uleb128 0xf
	.4byte	0x1c2
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x22d
	.uleb128 0x10
	.4byte	0x1ce
	.4byte	.LLST0
	.uleb128 0x11
	.4byte	0x1d9
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x11
	.4byte	0x1e4
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x11
	.4byte	0x1ef
	.uleb128 0x1
	.byte	0x55
	.byte	0
	.uleb128 0x12
	.4byte	.LASF60
	.byte	0x1
	.byte	0x6c
	.4byte	0x94
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x13
	.4byte	.LASF45
	.byte	0x1
	.byte	0x7c
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x292
	.uleb128 0x14
	.string	"fn"
	.byte	0x1
	.byte	0x7c
	.4byte	0xb4
	.4byte	.LLST1
	.uleb128 0x14
	.string	"arg"
	.byte	0x1
	.byte	0x7c
	.4byte	0xb4
	.4byte	.LLST2
	.uleb128 0x15
	.4byte	.LVL6
	.4byte	0x4c8
	.uleb128 0x16
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x34
	.uleb128 0x16
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x16
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x53
	.byte	0
	.byte	0
	.uleb128 0x13
	.4byte	.LASF46
	.byte	0x1
	.byte	0x84
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x2e5
	.uleb128 0x14
	.string	"i"
	.byte	0x1
	.byte	0x84
	.4byte	0x94
	.4byte	.LLST3
	.uleb128 0x17
	.4byte	.LASF47
	.byte	0x1
	.byte	0x84
	.4byte	0x11f
	.4byte	.LLST4
	.uleb128 0x18
	.4byte	.LASF49
	.byte	0x1
	.byte	0x86
	.4byte	0x94
	.4byte	.LLST5
	.uleb128 0x19
	.4byte	.LVL9
	.4byte	0x4e3
	.uleb128 0x19
	.4byte	.LVL14
	.4byte	0x4ea
	.byte	0
	.uleb128 0x13
	.4byte	.LASF48
	.byte	0x1
	.byte	0x95
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x317
	.uleb128 0x1a
	.string	"i"
	.byte	0x1
	.byte	0x97
	.4byte	0x7e
	.4byte	.LLST6
	.uleb128 0x18
	.4byte	.LASF49
	.byte	0x1
	.byte	0x98
	.4byte	0x94
	.4byte	.LLST7
	.byte	0
	.uleb128 0x13
	.4byte	.LASF50
	.byte	0x1
	.byte	0xa6
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x36a
	.uleb128 0x14
	.string	"i"
	.byte	0x1
	.byte	0xa6
	.4byte	0x94
	.4byte	.LLST8
	.uleb128 0x17
	.4byte	.LASF47
	.byte	0x1
	.byte	0xa6
	.4byte	0x11f
	.4byte	.LLST9
	.uleb128 0x18
	.4byte	.LASF49
	.byte	0x1
	.byte	0xa8
	.4byte	0x94
	.4byte	.LLST10
	.uleb128 0x19
	.4byte	.LVL22
	.4byte	0x4e3
	.uleb128 0x19
	.4byte	.LVL26
	.4byte	0x4ea
	.byte	0
	.uleb128 0x13
	.4byte	.LASF51
	.byte	0x1
	.byte	0x1e
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x466
	.uleb128 0x17
	.4byte	.LASF52
	.byte	0x1
	.byte	0x1e
	.4byte	0x466
	.4byte	.LLST11
	.uleb128 0x18
	.4byte	.LASF53
	.byte	0x1
	.byte	0x20
	.4byte	0x89
	.4byte	.LLST12
	.uleb128 0x18
	.4byte	.LASF54
	.byte	0x1
	.byte	0x21
	.4byte	0x94
	.4byte	.LLST13
	.uleb128 0x18
	.4byte	.LASF55
	.byte	0x1
	.byte	0x22
	.4byte	0x7e
	.4byte	.LLST14
	.uleb128 0x18
	.4byte	.LASF49
	.byte	0x1
	.byte	0x23
	.4byte	0x94
	.4byte	.LLST15
	.uleb128 0x1b
	.4byte	0x1c2
	.4byte	.LBB6
	.4byte	.LBE6-.LBB6
	.byte	0x1
	.byte	0x26
	.4byte	0x402
	.uleb128 0x10
	.4byte	0x1ef
	.4byte	.LLST16
	.uleb128 0x10
	.4byte	0x1e4
	.4byte	.LLST17
	.uleb128 0x10
	.4byte	0x1d9
	.4byte	.LLST17
	.uleb128 0x10
	.4byte	0x1ce
	.4byte	.LLST17
	.byte	0
	.uleb128 0x1b
	.4byte	0x1c2
	.4byte	.LBB8
	.4byte	.LBE8-.LBB8
	.byte	0x1
	.byte	0x28
	.4byte	0x43a
	.uleb128 0x10
	.4byte	0x1ef
	.4byte	.LLST20
	.uleb128 0x10
	.4byte	0x1e4
	.4byte	.LLST21
	.uleb128 0x10
	.4byte	0x1d9
	.4byte	.LLST20
	.uleb128 0x10
	.4byte	0x1ce
	.4byte	.LLST20
	.byte	0
	.uleb128 0x19
	.4byte	.LVL39
	.4byte	0x4e3
	.uleb128 0x19
	.4byte	.LVL43
	.4byte	0x4e3
	.uleb128 0x19
	.4byte	.LVL49
	.4byte	0x4ea
	.uleb128 0x15
	.4byte	.LVL50
	.4byte	0x317
	.uleb128 0x16
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x2
	.byte	0x7f
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0x1b7
	.uleb128 0x1c
	.4byte	.LASF58
	.byte	0x1
	.byte	0xb4
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x13
	.4byte	.LASF56
	.byte	0x1
	.byte	0xc0
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x4a2
	.uleb128 0x17
	.4byte	.LASF57
	.byte	0x1
	.byte	0xc0
	.4byte	0x7e
	.4byte	.LLST24
	.byte	0
	.uleb128 0x1c
	.4byte	.LASF59
	.byte	0x1
	.byte	0xc6
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x12
	.4byte	.LASF61
	.byte	0x1
	.byte	0xd2
	.4byte	0x7e
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x1d
	.4byte	.LASF63
	.byte	0x4
	.byte	0xba
	.4byte	0x4e3
	.uleb128 0x7
	.4byte	0x7e
	.uleb128 0x7
	.4byte	0xe3
	.uleb128 0x7
	.4byte	0xb4
	.byte	0
	.uleb128 0x1e
	.4byte	.LASF64
	.byte	0x4
	.byte	0x89
	.uleb128 0x1e
	.4byte	.LASF65
	.byte	0x4
	.byte	0x8a
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL1-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x2
	.byte	0x76
	.sleb128 0
	.4byte	.LVL2-.Ltext0
	.4byte	.LFE1-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST1:
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL5-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL5-.Ltext0
	.4byte	.LVL6-1-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL6-1-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST2:
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL4-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL4-.Ltext0
	.4byte	.LVL6-1-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL6-1-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x53
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST3:
	.4byte	.LVL7-.Ltext0
	.4byte	.LVL8-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL8-.Ltext0
	.4byte	.LVL15-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LVL15-.Ltext0
	.4byte	.LFE4-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST4:
	.4byte	.LVL7-.Ltext0
	.4byte	.LVL9-1-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL9-1-.Ltext0
	.4byte	.LVL10-.Ltext0
	.2byte	0x1
	.byte	0x5c
	.4byte	.LVL10-.Ltext0
	.4byte	.LFE4-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x53
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST5:
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL12-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL13-.Ltext0
	.4byte	.LVL14-1-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LLST6:
	.4byte	.LVL16-.Ltext0
	.4byte	.LVL17-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST7:
	.4byte	.LVL18-.Ltext0
	.4byte	.LVL19-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL19-.Ltext0
	.4byte	.LVL20-.Ltext0
	.2byte	0x7
	.byte	0x73
	.sleb128 0
	.byte	0xb
	.2byte	0xfc7f
	.byte	0x1a
	.byte	0x9f
	.4byte	.LVL20-.Ltext0
	.4byte	.LFE5-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LLST8:
	.4byte	.LVL21-.Ltext0
	.4byte	.LVL22-1-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL22-1-.Ltext0
	.4byte	.LVL28-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LVL28-.Ltext0
	.4byte	.LFE6-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST9:
	.4byte	.LVL21-.Ltext0
	.4byte	.LVL22-1-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL22-1-.Ltext0
	.4byte	.LVL27-.Ltext0
	.2byte	0x1
	.byte	0x5c
	.4byte	.LVL27-.Ltext0
	.4byte	.LFE6-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x53
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST10:
	.4byte	.LVL23-.Ltext0
	.4byte	.LVL24-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL24-.Ltext0
	.4byte	.LVL25-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL25-.Ltext0
	.4byte	.LVL26-1-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	0
	.4byte	0
.LLST11:
	.4byte	.LVL29-.Ltext0
	.4byte	.LVL31-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL31-.Ltext0
	.4byte	.LVL53-.Ltext0
	.2byte	0x1
	.byte	0x5e
	.4byte	.LVL53-.Ltext0
	.4byte	.LFE0-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST12:
	.4byte	.LVL30-.Ltext0
	.4byte	.LVL31-.Ltext0
	.2byte	0x2
	.byte	0x72
	.sleb128 0
	.4byte	.LVL31-.Ltext0
	.4byte	.LVL36-.Ltext0
	.2byte	0x2
	.byte	0x7e
	.sleb128 0
	.4byte	.LVL36-.Ltext0
	.4byte	.LVL52-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	0
	.4byte	0
.LLST13:
	.4byte	.LVL37-.Ltext0
	.4byte	.LVL38-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LLST14:
	.4byte	.LVL30-.Ltext0
	.4byte	.LVL36-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL36-.Ltext0
	.4byte	.LVL50-.Ltext0
	.2byte	0x1
	.byte	0x5f
	.4byte	.LVL50-.Ltext0
	.4byte	.LVL51-.Ltext0
	.2byte	0x3
	.byte	0x7f
	.sleb128 1
	.byte	0x9f
	.4byte	.LVL51-.Ltext0
	.4byte	.LVL54-.Ltext0
	.2byte	0x3
	.byte	0x7f
	.sleb128 0
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST15:
	.4byte	.LVL40-.Ltext0
	.4byte	.LVL41-.Ltext0
	.2byte	0x6
	.byte	0x73
	.sleb128 0
	.byte	0x9
	.byte	0xfb
	.byte	0x1a
	.byte	0x9f
	.4byte	.LVL41-.Ltext0
	.4byte	.LVL42-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL44-.Ltext0
	.4byte	.LVL45-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL45-.Ltext0
	.4byte	.LVL47-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL47-.Ltext0
	.4byte	.LVL48-.Ltext0
	.2byte	0x2
	.byte	0x7c
	.sleb128 0
	.4byte	0
	.4byte	0
.LLST16:
	.4byte	.LVL32-.Ltext0
	.4byte	.LVL33-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	0
	.4byte	0
.LLST17:
	.4byte	.LVL32-.Ltext0
	.4byte	.LVL33-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST20:
	.4byte	.LVL34-.Ltext0
	.4byte	.LVL35-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST21:
	.4byte	.LVL34-.Ltext0
	.4byte	.LVL35-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	0
	.4byte	0
.LLST24:
	.4byte	.LVL55-.Ltext0
	.4byte	.LVL56-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL56-.Ltext0
	.4byte	.LFE8-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
	.section	.debug_aranges,"",@progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0
	.4byte	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF36:
	.string	"GPIO_Pin"
.LASF62:
	.string	"gpio_output_conf"
.LASF40:
	.string	"GPIO_ConfigTypeDef"
.LASF51:
	.string	"gpio_config"
.LASF20:
	.string	"_xt_isr"
.LASF2:
	.string	"short int"
.LASF17:
	.string	"sizetype"
.LASF34:
	.string	"GPIO_PullUp_EN"
.LASF56:
	.string	"gpio16_output_set"
.LASF27:
	.string	"GPIO_INT_TYPE"
.LASF57:
	.string	"value"
.LASF61:
	.string	"gpio16_input_get"
.LASF11:
	.string	"uint16"
.LASF3:
	.string	"uint8_t"
.LASF10:
	.string	"uint8"
.LASF48:
	.string	"gpio_pin_wakeup_disable"
.LASF21:
	.string	"GPIO_PIN_INTR_DISABLE"
.LASF52:
	.string	"pGPIOConfig"
.LASF13:
	.string	"float"
.LASF8:
	.string	"long long int"
.LASF16:
	.string	"long int"
.LASF26:
	.string	"GPIO_PIN_INTR_HILEVEL"
.LASF38:
	.string	"GPIO_Pullup"
.LASF42:
	.string	"clear_mask"
.LASF50:
	.string	"gpio_pin_intr_state_set"
.LASF31:
	.string	"GPIO_Mode_Sigma_Delta"
.LASF35:
	.string	"GPIO_Pullup_IF"
.LASF68:
	.string	"C:\\\\ESP8266_Git\\\\ESP8266_Template"
.LASF1:
	.string	"unsigned char"
.LASF29:
	.string	"GPIO_Mode_Out_OD"
.LASF67:
	.string	"libraries/gpio.c"
.LASF25:
	.string	"GPIO_PIN_INTR_LOLEVEL"
.LASF28:
	.string	"GPIO_Mode_Input"
.LASF0:
	.string	"signed char"
.LASF9:
	.string	"long long unsigned int"
.LASF6:
	.string	"uint32_t"
.LASF7:
	.string	"unsigned int"
.LASF4:
	.string	"uint16_t"
.LASF15:
	.string	"long unsigned int"
.LASF55:
	.string	"io_num"
.LASF43:
	.string	"enable_mask"
.LASF5:
	.string	"short unsigned int"
.LASF64:
	.string	"vPortEnterCritical"
.LASF18:
	.string	"char"
.LASF41:
	.string	"set_mask"
.LASF19:
	.string	"_Bool"
.LASF24:
	.string	"GPIO_PIN_INTR_ANYEDGE"
.LASF33:
	.string	"GPIO_PullUp_DIS"
.LASF65:
	.string	"vPortExitCritical"
.LASF47:
	.string	"intr_state"
.LASF22:
	.string	"GPIO_PIN_INTR_POSEDGE"
.LASF12:
	.string	"uint32"
.LASF37:
	.string	"GPIO_Mode"
.LASF14:
	.string	"double"
.LASF63:
	.string	"_xt_isr_attach"
.LASF45:
	.string	"gpio_intr_handler_register"
.LASF44:
	.string	"disable_mask"
.LASF54:
	.string	"io_reg"
.LASF59:
	.string	"gpio16_input_conf"
.LASF60:
	.string	"gpio_input_get"
.LASF30:
	.string	"GPIO_Mode_Output"
.LASF58:
	.string	"gpio16_output_conf"
.LASF66:
	.string	"GNU C89 5.1.0 -mlongcalls -mtext-section-literals -mno-serialize-volatile -g -Os -std=gnu90 -fno-inline-functions"
.LASF32:
	.string	"GPIOMode_TypeDef"
.LASF49:
	.string	"pin_reg"
.LASF46:
	.string	"gpio_pin_wakeup_enable"
.LASF39:
	.string	"GPIO_IntrType"
.LASF53:
	.string	"gpio_pin_mask"
.LASF23:
	.string	"GPIO_PIN_INTR_NEGEDGE"
	.ident	"GCC: (GNU) 5.1.0"
