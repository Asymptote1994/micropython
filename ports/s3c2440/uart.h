#ifndef __UART_H__
#define __UART_H__

#define PCLK            50000000    // clock_init函数设置PCLK为50MHz
#define UART_CLK        PCLK        //  UART0的时钟源设为PCLK
#define UART_BAUD_RATE  115200      // 波特率
#define UART_BRD        ((UART_CLK  / (UART_BAUD_RATE * 16)) - 1)

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);

void s3c2440_uart0_init(void);
void s3c2440_putc(char c);
// unsigned char s3c2440_getc(void);
// void s3c2440_puts(unsigned char *s);
// unsigned char *s3c2440_gets(unsigned char *s);

#endif

