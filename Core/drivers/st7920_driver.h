#ifndef DRIVERS_ST7920_DRIVER_H_
#define DRIVERS_ST7920_DRIVER_H_

#include "main.h"

void ST7920_Init(void);
void ST7920_Clear(void);
void ST7920_SetCursor(uint8_t row, uint8_t col);
void ST7920_WriteString(const char* str);

#endif /* DRIVERS_ST7920_DRIVER_H_ */
