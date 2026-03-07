#include "st7920_driver.h"

#include <string.h>

#include "spi.h"
#include "u8g2/u8g2.h"

/*
 * Nhan xet thuc te voi module ban dang dung:
 * - CS thuong active HIGH
 * Neu can test, doi 0 -> 1.
 */
#define ST7920_CS_ACTIVE_LOW 0u

/* Dua 1 phan u8g2 core vao file nay de tranh sua makefile generated. */
#include "../../third_party/csrc/u8g2_setup.c"
#include "../../third_party/csrc/u8g2_d_setup.c"
#include "../../third_party/csrc/u8g2_d_memory.c"
#include "../../third_party/csrc/u8g2_buffer.c"
#include "../../third_party/csrc/u8g2_cleardisplay.c"
#include "../../third_party/csrc/u8g2_font.c"
#include "../../third_party/csrc/u8g2_fonts.c"
#include "../../third_party/csrc/u8g2_hvline.c"
#include "../../third_party/csrc/u8g2_ll_hvline.c"
#include "../../third_party/csrc/u8g2_line.c"
#include "../../third_party/csrc/u8g2_circle.c"
#include "../../third_party/csrc/u8g2_box.c"
#include "../../third_party/csrc/u8g2_bitmap.c"
#include "../../third_party/csrc/u8g2_intersection.c"

#define ST7920_CS_PORT GPIOB
#define ST7920_CS_PIN  GPIO_PIN_12

static u8g2_t g_u8g2;
static uint8_t g_cursor_row = 0u;
static uint8_t g_cursor_col = 0u;
static char g_lines[4][17];

static void ST7920_WriteCS(uint8_t active)
{
#if ST7920_CS_ACTIVE_LOW
    HAL_GPIO_WritePin(ST7920_CS_PORT, ST7920_CS_PIN, active ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(ST7920_CS_PORT, ST7920_CS_PIN, active ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

/* ----------------- ST7920 Functions ------------------- */
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    GPIO_InitTypeDef gpio = {0};
    (void)u8x8;
    (void)arg_ptr;

    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            __HAL_RCC_GPIOB_CLK_ENABLE();

            gpio.Pin = ST7920_CS_PIN | RST_Pin;
            gpio.Mode = GPIO_MODE_OUTPUT_PP;
            gpio.Pull = GPIO_NOPULL;
            gpio.Speed = GPIO_SPEED_FREQ_LOW;
            HAL_GPIO_Init(GPIOB, &gpio);

            ST7920_WriteCS(0u);
            HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
            break;

        case U8X8_MSG_DELAY_MILLI:
            /* Phuong an 1: giu delay dong bo de timing u8g2 on dinh. */
            HAL_Delay(arg_int);
            break;

        case U8X8_MSG_GPIO_CS:
            ST7920_WriteCS(arg_int ? 1u : 0u);
            break;

        case U8X8_MSG_GPIO_DC:
            /* ST7920 serial khong dung chan DC rieng. */
            break;

        case U8X8_MSG_GPIO_RESET:
            HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        default:
            break;
    }
    return 1u;
}

/* Normal SPI */
uint8_t u8x8_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    (void)u8x8;

    switch(msg)
    {
        case U8X8_MSG_BYTE_SET_DC:
            /* ST7920 serial khong can DC. */
            break;

        case U8X8_MSG_BYTE_SEND:
            (void)HAL_SPI_Transmit(&hspi2, (uint8_t *)arg_ptr, arg_int, 1000u);
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            ST7920_WriteCS(1u);
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
            ST7920_WriteCS(0u);
            break;

        case U8X8_MSG_BYTE_INIT:
            ST7920_WriteCS(0u);
            break;

        default:
            return 0u;
    }
    return 1u;
}

static void ST7920_RenderAll(void)
{
    u8g2_ClearBuffer(&g_u8g2);
    u8g2_SetFont(&g_u8g2, u8g2_font_6x12_tr);

    u8g2_DrawStr(&g_u8g2, 0u, 14u, g_lines[0]);
    u8g2_DrawStr(&g_u8g2, 0u, 30u, g_lines[1]);
    u8g2_DrawStr(&g_u8g2, 0u, 46u, g_lines[2]);
    u8g2_DrawStr(&g_u8g2, 0u, 62u, g_lines[3]);

    u8g2_SendBuffer(&g_u8g2);
}

void ST7920_Init(void)
{
    uint8_t i;

    for (i = 0u; i < 4u; i++)
    {
        memset(g_lines[i], ' ', 16u);
        g_lines[i][16] = '\0';
    }

    u8g2_Setup_st7920_s_128x64_f(&g_u8g2, U8G2_R0, u8x8_spi, u8x8_gpio_and_delay);

    u8g2_InitDisplay(&g_u8g2);
    u8g2_SetPowerSave(&g_u8g2, 0u);
    u8g2_ClearDisplay(&g_u8g2);

    g_cursor_row = 0u;
    g_cursor_col = 0u;
    ST7920_RenderAll();
}

void ST7920_Clear(void)
{
    uint8_t i;
    for (i = 0u; i < 4u; i++)
    {
        memset(g_lines[i], ' ', 16u);
        g_lines[i][16] = '\0';
    }
    ST7920_RenderAll();
}

void ST7920_SetCursor(uint8_t row, uint8_t col)
{
    if (row > 3u) row = 0u;
    if (col > 15u) col = 15u;

    g_cursor_row = row;
    g_cursor_col = col;
}

void ST7920_WriteString(const char* str)
{
    uint8_t len;

    if (str == NULL) return;

    memset(g_lines[g_cursor_row], ' ', 16u);
    len = (uint8_t)strlen(str);
    if (len > (uint8_t)(16u - g_cursor_col)) len = (uint8_t)(16u - g_cursor_col);
    memcpy(&g_lines[g_cursor_row][g_cursor_col], str, len);
    g_lines[g_cursor_row][16] = '\0';

    ST7920_RenderAll();
}
