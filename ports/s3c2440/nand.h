#ifndef __NAND_H__
#define __NAND_H__

void nand_init(void);
void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);

#endif

