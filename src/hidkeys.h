#ifndef HID_KEYS_H
#define HID_KEYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t HidKey;
enum {
    HIDKEY_A = 4,
    HIDKEY_B = 5,
    HIDKEY_C = 6,
    HIDKEY_D = 7,
    HIDKEY_E = 8,
    HIDKEY_F = 9,
    HIDKEY_G = 10,
    HIDKEY_H = 11,
    HIDKEY_I = 12,
    HIDKEY_J = 13,
    HIDKEY_K = 14,
    HIDKEY_L = 15,
    HIDKEY_M = 16,
    HIDKEY_N = 17,
    HIDKEY_O = 18,
    HIDKEY_P = 19,
    HIDKEY_Q = 20,
    HIDKEY_R = 21,
    HIDKEY_S = 22,
    HIDKEY_T = 23,
    HIDKEY_U = 24,
    HIDKEY_V = 25,
    HIDKEY_W = 26,
    HIDKEY_X = 27,
    HIDKEY_Y = 28,
    HIDKEY_Z = 29,
    HIDKEY_1 = 30,
    HIDKEY_2 = 31,
    HIDKEY_3 = 32,
    HIDKEY_4 = 33,
    HIDKEY_5 = 34,
    HIDKEY_6 = 35,
    HIDKEY_7 = 36,
    HIDKEY_8 = 37,
    HIDKEY_9 = 38,
    HIDKEY_0 = 39,
    HIDKEY_ENTER = 40,
    HIDKEY_ESCAPE = 41,
    HIDKEY_BACKSPACE = 42,
    HIDKEY_TAB = 43,
    HIDKEY_SPACEBAR = 44,
    HIDKEY_UNDERSCORE = 45,
    HIDKEY_PLUS = 46,
    HIDKEY_OPEN_BRACKET = 47,
    HIDKEY_CLOSE_BRACKET = 48,  //
    HIDKEY_BACKSLASH = 49,      // backslash and pipe
    HIDKEY_HASH = 50,           // hash and tilde
    HIDKEY_COLON = 51,          // semicolon
    HIDKEY_QUOTE = 52,          // quote and single quote
    HIDKEY_TILDE = 53,          // grave accent
    HIDKEY_COMMA = 54,          // , >
    HIDKEY_DOT = 55,            // . >
    HIDKEY_SLASH = 56,          // / ?
    HIDKEY_CAPS_LOCK = 57,
    HIDKEY_F1 = 58,
    HIDKEY_F2 = 59,
    HIDKEY_F3 = 60,
    HIDKEY_F4 = 61,
    HIDKEY_F5 = 62,
    HIDKEY_F6 = 63,
    HIDKEY_F7 = 64,
    HIDKEY_F8 = 65,
    HIDKEY_F9 = 66,
    HIDKEY_F10 = 67,
    HIDKEY_F11 = 68,
    HIDKEY_F12 = 69,

    HIDKEY_PRINTSCREEN = 70,
    HIDKEY_SCROLL_LOCK = 71,
    HIDKEY_PAUSE = 72,
    HIDKEY_INSERT = 73,
    HIDKEY_HOME = 74,
    HIDKEY_PAGEUP = 75,
    HIDKEY_DELETE = 76,
    HIDKEY_END = 77,
    HIDKEY_PAGEDOWN = 78,
    HIDKEY_RIGHT = 79,
    HIDKEY_LEFT = 80,
    HIDKEY_DOWN = 81,
    HIDKEY_UP = 82,
    HIDKEY_KP_NUM_LOCK = 83,
    HIDKEY_KP_DIVIDE = 84,
    HIDKEY_KP_AT = 85,
    HIDKEY_KP_MULTIPLY = 85,
    HIDKEY_KP_MINUS = 86,
    HIDKEY_KP_PLUS = 87,
    HIDKEY_KP_ENTER = 88,
    HIDKEY_KP_1 = 89,
    HIDKEY_KP_2 = 90,
    HIDKEY_KP_3 = 91,
    HIDKEY_KP_4 = 92,
    HIDKEY_KP_5 = 93,
    HIDKEY_KP_6 = 94,
    HIDKEY_KP_7 = 95,
    HIDKEY_KP_8 = 96,
    HIDKEY_KP_9 = 97,
    HIDKEY_KP_0 = 98,
    HIDKEY_KP_DOT_AND_DELETE = 99,
    HIDKEY_NON_US_BACKSLASH_AND_SLASH = 100,

    HIDKEY_APPLICATION = 0x65,
    HIDKEY_POWER = 0x66,

    HIDKEY_F13 = 0x68,
    HIDKEY_F14 = 0x69,
    HIDKEY_F15 = 0x6a,
    HIDKEY_F16 = 0x6b,
    HIDKEY_F17 = 0x6c,
    HIDKEY_F18 = 0x6d,
    HIDKEY_F19 = 0x6e,
    HIDKEY_F20 = 0x6f,
    HIDKEY_F21 = 0x70,
    HIDKEY_F22 = 0x71,
    HIDKEY_F23 = 0x72,
    HIDKEY_F24 = 0x73,

    HIDKEY_EXECUTE = 0x74,
    HIDKEY_HELP = 0x75,
    HIDKEY_MENU = 0x76,
    HIDKEY_SELECT = 0x77,
    HIDKEY_STOP = 0x78,
    HIDKEY_AGAIN = 0x79,
    HIDKEY_UNDO = 0x7a,
    HIDKEY_CUT = 0x7b,
    HIDKEY_COPY = 0x7c,
    HIDKEY_PASTE = 0x7d,
    HIDKEY_FIND = 0x7e,
    HIDKEY_MUTE = 0x7f,

    // Modifiers bit sets
    HIDKEY_MODIFIER_NONE        = 0x00,
    HIDKEY_MODIFIER_LEFT_CTRL   = 0x01,
    HIDKEY_MODIFIER_LEFT_SHIFT  = 0x02,
    HIDKEY_MODIFIER_LEFT_ALT    = 0x04,
    HIDKEY_MODIFIER_LEFT_UI     = 0x08,
    HIDKEY_MODIFIER_RIGHT_CTRL  = 0x10,
    HIDKEY_MODIFIER_RIGHT_SHIFT = 0x20,
    HIDKEY_MODIFIER_RIGHT_ALT   = 0x40,
    HIDKEY_MODIFIER_RIGHT_UI    = 0x80,
};

typedef uint8_t HidMediaKey;
enum {
    HIDMEDIAKEY_NONE            = 0x00,
    HIDMEDIAKEY_PREV_TRACK      = 0x01,
    HIDMEDIAKEY_NEXT_TRACK      = 0x02,
    HIDMEDIAKEY_STOP            = 0x04,
    HIDMEDIAKEY_PLAY            = 0x08,
    HIDMEDIAKEY_PAUSE           = 0x10,
    HIDMEDIAKEY_REWIND          = 0x20,
    HIDMEDIAKEY_FFWD            = 0x40,
    HIDMEDIAKEY_MUTE            = 0x80,
};

#ifdef __cplusplus
}
#endif

#endif // HIDKEY_H
