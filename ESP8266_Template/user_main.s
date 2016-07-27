	.file	"user_main.c"
	.text
.Ltext0:
	.literal_position
	.align	4
	.global	blink_task
	.type	blink_task, @function
blink_task:
.LFB0:
	.file 1 "user/user_main.c"
	.loc 1 50 0
.LVL0:
	addi	sp, sp, -16
.LCFI0:
	s32i.n	a12, sp, 8
.LCFI1:
	.loc 1 51 0
	movi.n	a12, 0
	.loc 1 50 0
	s32i.n	a13, sp, 4
	s32i.n	a14, sp, 0
	s32i.n	a0, sp, 12
.LCFI2:
	.loc 1 56 0
	movi.n	a14, 4
	mov.n	a13, a12
.LVL1:
.L3:
	.loc 1 55 0
	movi.n	a2, 1
	sub	a12, a2, a12
.LVL2:
	extui	a12, a12, 0, 8
.LVL3:
	.loc 1 56 0
	mov.n	a3, a13
	slli	a2, a12, 2
	moveqz	a3, a14, a12
	movi.n	a5, 0
	movi.n	a4, 4
	call0	gpio_output_conf
.LVL4:
	.loc 1 57 0
	movi.n	a2, 0x14
	call0	vTaskDelay
.LVL5:
	.loc 1 58 0
	j	.L3
.LFE0:
	.size	blink_task, .-blink_task
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"blink_task"
	.text
	.literal_position
	.literal .LC0, 1610614840
	.literal .LC2, .LC1
	.literal .LC3, blink_task
	.align	4
	.global	user_init
	.type	user_init, @function
user_init:
.LFB1:
	.loc 1 70 0
	.loc 1 72 0
	l32r	a2, .LC0
	.loc 1 70 0
	addi	sp, sp, -32
.LCFI3:
	.loc 1 72 0
	l32i.n	a4, a2, 0
	movi	a3, -0x131
	.loc 1 70 0
	s32i.n	a0, sp, 28
.LCFI4:
	.loc 1 72 0
	and	a3, a4, a3
	s32i.n	a3, a2, 0
	l32i.n	a3, a2, 0
	.loc 1 75 0
	movi.n	a5, 0
	.loc 1 72 0
	s32i.n	a3, a2, 0
	.loc 1 75 0
	l32r	a3, .LC2
	l32r	a2, .LC3
	s32i.n	a5, sp, 4
	s32i.n	a5, sp, 0
	mov.n	a7, a5
	movi.n	a6, 2
	movi	a4, 0x100
	call0	xTaskGenericCreate
.LVL6:
	.loc 1 76 0
	l32i.n	a0, sp, 28
	addi	sp, sp, 32
	ret.n
.LFE1:
	.size	user_init, .-user_init
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
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.byte	0x4
	.4byte	.LCFI0-.LFB0
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0x8c
	.uleb128 0x2
	.byte	0x4
	.4byte	.LCFI2-.LCFI1
	.byte	0x8d
	.uleb128 0x3
	.byte	0x8e
	.uleb128 0x4
	.byte	0x80
	.uleb128 0x1
	.align	4
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.byte	0x4
	.4byte	.LCFI3-.LFB1
	.byte	0xe
	.uleb128 0x20
	.byte	0x4
	.4byte	.LCFI4-.LCFI3
	.byte	0x80
	.uleb128 0x1
	.align	4
.LEFDE2:
	.text
.Letext0:
	.file 2 "c:\\espressif\\xtensa-lx106-elf\\xtensa-lx106-elf\\include\\stdint.h"
	.file 3 "c:/Espressif/ESP8266_RTOS_SDK/include/espressif/c_types.h"
	.file 4 "c:/Espressif/ESP8266_RTOS_SDK/include/freertos/projdefs.h"
	.file 5 "c:/Espressif/ESP8266_RTOS_SDK/include/freertos/portmacro.h"
	.file 6 "c:/Espressif/ESP8266_RTOS_SDK/include/freertos/task.h"
	.file 7 "libraries/gpio.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.4byte	0x27d
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF26
	.byte	0x1
	.4byte	.LASF27
	.4byte	.LASF28
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x3
	.4byte	.LASF4
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
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF3
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x3
	.4byte	.LASF5
	.byte	0x2
	.byte	0x51
	.4byte	0x5e
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x3
	.4byte	.LASF9
	.byte	0x3
	.byte	0x34
	.4byte	0x53
	.uleb128 0x2
	.byte	0x4
	.byte	0x4
	.4byte	.LASF10
	.uleb128 0x2
	.byte	0x8
	.byte	0x4
	.4byte	.LASF11
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF12
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF13
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF14
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF15
	.uleb128 0x2
	.byte	0x1
	.byte	0x2
	.4byte	.LASF16
	.uleb128 0x6
	.4byte	0xbc
	.uleb128 0x7
	.4byte	0x93
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0xb1
	.uleb128 0x3
	.4byte	.LASF17
	.byte	0x4
	.byte	0x45
	.4byte	0xbc
	.uleb128 0x3
	.4byte	.LASF18
	.byte	0x5
	.byte	0x62
	.4byte	0x8c
	.uleb128 0x3
	.4byte	.LASF19
	.byte	0x6
	.byte	0x5f
	.4byte	0x93
	.uleb128 0x9
	.4byte	.LASF29
	.byte	0xc
	.byte	0x6
	.byte	0x77
	.4byte	0x114
	.uleb128 0xa
	.4byte	.LASF20
	.byte	0x6
	.byte	0x79
	.4byte	0x93
	.byte	0
	.uleb128 0xa
	.4byte	.LASF21
	.byte	0x6
	.byte	0x7a
	.4byte	0x8c
	.byte	0x4
	.uleb128 0xa
	.4byte	.LASF22
	.byte	0x6
	.byte	0x7b
	.4byte	0x8c
	.byte	0x8
	.byte	0
	.uleb128 0x3
	.4byte	.LASF23
	.byte	0x6
	.byte	0x7c
	.4byte	0xe3
	.uleb128 0x8
	.byte	0x4
	.4byte	0x125
	.uleb128 0xb
	.4byte	0x25
	.uleb128 0xb
	.4byte	0x11f
	.uleb128 0x8
	.byte	0x4
	.4byte	0x8c
	.uleb128 0xc
	.4byte	.LASF30
	.byte	0x1
	.byte	0x31
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x1ab
	.uleb128 0xd
	.4byte	.LASF31
	.byte	0x1
	.byte	0x31
	.4byte	0x93
	.4byte	.LLST0
	.uleb128 0xe
	.4byte	.LASF32
	.byte	0x1
	.byte	0x33
	.4byte	0x2c
	.4byte	.LLST1
	.uleb128 0xf
	.4byte	.LVL4
	.4byte	0x1ff
	.4byte	0x19b
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x4
	.byte	0x7c
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x53
	.uleb128 0xd
	.byte	0x7e
	.sleb128 0
	.byte	0x7d
	.sleb128 0
	.byte	0x7c
	.sleb128 0
	.byte	0x30
	.byte	0x29
	.byte	0x28
	.2byte	0x1
	.byte	0x16
	.byte	0x13
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x2
	.byte	0x7d
	.sleb128 0
	.byte	0
	.uleb128 0x11
	.4byte	.LVL5
	.4byte	0x21f
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x1
	.byte	0x44
	.byte	0
	.byte	0
	.uleb128 0x12
	.4byte	.LASF33
	.byte	0x1
	.byte	0x45
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x1ff
	.uleb128 0x11
	.4byte	.LVL6
	.4byte	0x231
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x5
	.byte	0x3
	.4byte	blink_task
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x53
	.uleb128 0x5
	.byte	0x3
	.4byte	.LC1
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x54
	.uleb128 0x3
	.byte	0xa
	.2byte	0x100
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x55
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x56
	.uleb128 0x1
	.byte	0x32
	.uleb128 0x10
	.uleb128 0x1
	.byte	0x57
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x10
	.uleb128 0x2
	.byte	0x71
	.sleb128 0
	.uleb128 0x1
	.byte	0x30
	.uleb128 0x10
	.uleb128 0x2
	.byte	0x71
	.sleb128 4
	.uleb128 0x1
	.byte	0x30
	.byte	0
	.byte	0
	.uleb128 0x13
	.4byte	.LASF24
	.byte	0x7
	.byte	0xf5
	.4byte	0x21f
	.uleb128 0x7
	.4byte	0x73
	.uleb128 0x7
	.4byte	0x73
	.uleb128 0x7
	.4byte	0x73
	.uleb128 0x7
	.4byte	0x73
	.byte	0
	.uleb128 0x14
	.4byte	.LASF25
	.byte	0x6
	.2byte	0x20d
	.4byte	0x231
	.uleb128 0x7
	.4byte	0xcd
	.byte	0
	.uleb128 0x15
	.4byte	.LASF34
	.byte	0x6
	.2byte	0x5c4
	.4byte	0x95
	.4byte	0x26a
	.uleb128 0x7
	.4byte	0xc2
	.uleb128 0x7
	.4byte	0x12a
	.uleb128 0x7
	.4byte	0x45
	.uleb128 0x7
	.4byte	0x93
	.uleb128 0x7
	.4byte	0x8c
	.uleb128 0x7
	.4byte	0x26a
	.uleb128 0x7
	.4byte	0x12f
	.uleb128 0x7
	.4byte	0x27b
	.byte	0
	.uleb128 0x8
	.byte	0x4
	.4byte	0xd8
	.uleb128 0x8
	.byte	0x4
	.4byte	0x276
	.uleb128 0xb
	.4byte	0x114
	.uleb128 0xb
	.4byte	0x270
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
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
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
	.uleb128 0xa
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
	.uleb128 0xb
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
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
	.uleb128 0x87
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
	.uleb128 0xd
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
	.uleb128 0xe
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
	.uleb128 0xf
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0x18
	.uleb128 0x2111
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
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
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
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
	.4byte	.LFE0-.Ltext0
	.2byte	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST1:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL1-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x5c
	.4byte	.LVL3-.Ltext0
	.4byte	.LFE0-.Ltext0
	.2byte	0x1
	.byte	0x5c
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
.LASF22:
	.string	"ulParameters"
.LASF20:
	.string	"pvBaseAddress"
.LASF32:
	.string	"state"
.LASF24:
	.string	"gpio_output_conf"
.LASF18:
	.string	"portTickType"
.LASF23:
	.string	"xMemoryRegion"
.LASF29:
	.string	"xMEMORY_REGION"
.LASF19:
	.string	"xTaskHandle"
.LASF33:
	.string	"user_init"
.LASF30:
	.string	"blink_task"
.LASF10:
	.string	"float"
.LASF26:
	.string	"GNU C89 5.1.0 -mlongcalls -mtext-section-literals -mno-serialize-volatile -g -Os -std=gnu90 -fno-inline-functions"
.LASF1:
	.string	"unsigned char"
.LASF12:
	.string	"long unsigned int"
.LASF3:
	.string	"short unsigned int"
.LASF17:
	.string	"pdTASK_CODE"
.LASF11:
	.string	"double"
.LASF9:
	.string	"uint32"
.LASF6:
	.string	"unsigned int"
.LASF8:
	.string	"long long unsigned int"
.LASF4:
	.string	"uint8_t"
.LASF21:
	.string	"ulLengthInBytes"
.LASF28:
	.string	"C:\\\\ESP8266_Git\\\\ESP8266\\\\ESP8266_Template"
.LASF14:
	.string	"sizetype"
.LASF7:
	.string	"long long int"
.LASF15:
	.string	"char"
.LASF31:
	.string	"pvParameters"
.LASF27:
	.string	"user/user_main.c"
.LASF2:
	.string	"short int"
.LASF34:
	.string	"xTaskGenericCreate"
.LASF5:
	.string	"uint32_t"
.LASF13:
	.string	"long int"
.LASF25:
	.string	"vTaskDelay"
.LASF0:
	.string	"signed char"
.LASF16:
	.string	"_Bool"
	.ident	"GCC: (GNU) 5.1.0"
