#include <unistd.h>
#include "py/mpconfig.h"

#include "s3c2440_regs.h"
#include "uart.h"

#define PCLK            50000000    // clock_init函数设置PCLK为50MHz
#define UART_CLK        PCLK        //  UART0的时钟源设为PCLK
#define UART_BAUD_RATE  115200      // 波特率
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

void s3c2440_uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3用作TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3内部上拉

    ULCON0  = 0x03;     // 8N1(8个数据位，无较验，1个停止位)
    UCON0   = 0x05;     // 查询方式，UART时钟源为PCLK
    UFCON0  = 0x00;     // 不使用FIFO
    UMCON0  = 0x00;     // 不使用流控
    UBRDIV0 = UART_BRD; // 波特率为115200
}

/* Used by printf in mylibc.a */
void putc(unsigned char c)
{
	while (!(UTRSTAT0 & (1 << 2)));
	UTXH0 = c;
}

/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void)
{
	while (!(UTRSTAT0 & (1)));

    return URXH0;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len)
{
    while (len--) {
		while (!(UTRSTAT0 & (1 << 2)));
    	UTXH0 = *str++;
    }
}
