/*
 * 张旭写于 2018.12.27  9:42pm
 *
 * Asymptote的程序入口
 *
 *
 *
 */

.equ	MEM_CTL_BASE, 0x48000000

.equ 	CLKDIVN, 0x4C000014
.equ 	MPLLCON, 0x4C000004
#define S3C2440_MPLL_200MHZ ((0x5c<<12)|(0x01<<4)|(0x02))
#define S3C2440_MPLL_400MHZ ((0x5c<<12)|(0x01<<4)|(0x01))

.text
.global _start

/*****************************************************************************/

@程序入口
_start:

@ 0x00: 复位异常的向量地址
	b Reset

@ 0x04: 未定义指令中止模式的向量地址
HandleUndef:
	b HandleUndef

@ 0x08: 管理模式的向量地址（在用户模式下，可通过SWI指令进入该模式）
HandleSvc:
	b HandleSvc

@ 0x0c: 指令预取中止异常的向量地址
HandlePrefetchAbort:
	b HandlePrefetchAbort

@ 0x10: 数据访问中止异常的向量地址
HandleDataAbort:
	b HandleDataAbort

@ 0x14: 保留
HandleNotUsed:
	b HandleNotUsed

@ 0x18: 中断模式的向量地址
HandleIRQ:
	b HandleIRQ

@ 0x1c: 快中断模式的向量地址
HandleFIQ:
	b HandleFIQ


/*****************************************************************************/
@复位异常入口
Reset:
	ldr sp, =4096     	    	@重启默认进入SVC模式，此处设置SVC模式栈指针

	bl disable_watch_dog
	bl clock_init
	bl sdram_init
	
	bl nand_init		    	@初始化 NAND Flash

	@nand_read函数需要3个参数：
	ldr r1, =0x30000000     	@1.目标地址=0x30000000，这是SDRAM的起始地址
	mov r0, #0		    		@2.源地址 = 0
	mov r2, #(1024*200)	    	@3.复制长度 = 200KB
	bl  nand_read		    	@调用C函数nand_read

   	ldr pc, =on_sdram           @跳到SDRAM中继续执行

on_sdram:
    msr cpsr_c, #0xdf       @进入系统模式
    ldr sp, =0x34000000     @设置系统模式栈指针

	ldr lr, =loop
	ldr pc, =main

loop:
	b loop

/***************************************************************/
disable_watch_dog:
	ldr r0, =0x53000000     @关闭看门狗
	mov r1, #0
	str r1, [r0]
	mov pc, lr

clock_init:
	ldr r0, =CLKDIVN     
	@ mov r1, #0x03		@1:2:4
	mov r1, #0x05		@1:4:8
	str r1, [r0]

	mrc p15, 0, r1, c1, c0, 0
	orr r1, r1, #0xc0000000
	mcr p15, 0, r1, c1, c0, 0

	ldr r0, =MPLLCON  
	@ ldr r1, =S3C2440_MPLL_200MHZ
	ldr r1, =S3C2440_MPLL_400MHZ
	str r1, [r0]
	mov pc, lr

sdram_init:
	mov r1, #MEM_CTL_BASE	@r1存放存储控制器基地址
	adrl r2, mem_cfg_val	@
	add r3, r1, #13*4		@r3存放存储控制器尾地址
1:
	ldr r4, [r2], #4		@
	str r4, [r1], #4
	cmp r1, r3
	bne 1b
	mov pc, lr

.align 4
mem_cfg_val:
	.long 0x22011110	 //BWSCON
	.long 0x00000700	 //BANKCON0
	.long 0x00000700	 //BANKCON1
	.long 0x00000700	 //BANKCON2
	.long 0x00000700	 //BANKCON3  
	.long 0x00000740	 //BANKCON4
	.long 0x00000700	 //BANKCON5
	.long 0x00018005	 //BANKCON6
	.long 0x00018005	 //BANKCON7
	.long 0x008C04F4	 // REFRESH
	.long 0x000000B1	 //BANKSIZE
	.long 0x00000030	 //MRSRB6
	.long 0x00000030	 //MRSRB7