#ifndef SSD1322_H
#define SSD1322_H

#include <stdint.h>

#define SSD1322_ENABLE_GRAY_SCALE_TABLE                 0x00
#define SSD1322_SET_COLUMN_ADDRESS                      0x15
#define SSD1322_WRITE_RAM_COMMAND                       0x5C
#define SSD1322_READ_RAM_COMMAND                        0x5D
#define SSD1322_SET_ROW_ADDRESS                         0x75
#define SSD1322_SET_REMAP_AND_DUAL_COM_LINE_MODE        0xA0
#define SSD1322_SET_DISPLAY_START_LINE                  0xA1
#define SSD1322_SET_DISPLAY_OFFSET                      0xA2
#define SSD1322_ENTIRE_DISPLAY_OFF                      0xA4
#define SSD1322_ENTIRE_DISPLAY_ON                       0xA5
#define SSD1322_NORMAL_DISPLAY                          0xA6
#define SSD1322_INVERSE_DISPLAY                         0xA7
#define SSD1322_ENABLE_PARTIAL_DISPLAY                  0xA8
#define SSD1322_EXIT_PARTIAL_DISPLAY                    0xA9
#define SSD1322_FUNCTION_SELECTION                      0xAB
#define SSD1322_SET_DISPLAY_OFF                         0xAE
#define SSD1322_SET_DISPLAY_ON                          0xAF
#define SSD1322_SET_PHASE_LENGTH                        0xB1
#define SSD1322_SET_CLOCK_DIVIDER                       0xB3
#define SSD1322_SET_GPIO                                0xB5
#define SSD1322_SET_SECOND_PRECHARGE_PERIOD             0xB6
#define SSD1322_SET_GRAY_SCALE_TABLE                    0xB8
#define SSD1322_SELECT_DEFAULT_LINEAR_GRAY_SCALE_TABLE  0xB9
#define SSD1322_SET_PRECHARGE_VOLTAGE                   0xBB
#define SSD1322_SET_VCOMH                               0xBE
#define SSD1322_SET_CONTRAST_CURRENT                    0xC1
#define SSD1322_MASTER_CURRENT_CONTROL                  0xC7
#define SSD1322_SET_MUX_RATIO                           0xCA
#define SSD1322_SET_COMMAND_LOCK                        0xFD

#define SSD1322_ENABLE_VSL_EXTERN                       0xB4
#define SSD1322_ENHANCE_DRIVING_SCHEME_CAPABILITY       0xD1

// Scrolling #defines
#define SSD1322_ACTIVATE_SCROLL                         0x2F
#define SSD1322_DEACTIVATE_SCROLL                       0x2E
#define SSD1322_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1322_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1322_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1322_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1322_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A

#define DISP_WIDTH      256
#define DISP_HEIGHT     64

#define MIN_SEG         28
#define MAX_SEG         91

void ssd1322Init(void);

#endif // SSD1322_H
