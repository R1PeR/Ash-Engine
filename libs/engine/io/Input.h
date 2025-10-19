#ifndef LIBS_ENGINE_INPUT_H
#define LIBS_ENGINE_INPUT_H
#include <stdbool.h>
#include <stdint.h>

enum
{
    INPUT_KEYCODE_NULL = 0,  // Key: NULL, used for no key pressed
    // Alphanumeric keys
    INPUT_KEYCODE_APOSTROPHE    = 39,  // Key: '
    INPUT_KEYCODE_COMMA         = 44,  // Key: ,
    INPUT_KEYCODE_MINUS         = 45,  // Key: -
    INPUT_KEYCODE_PERIOD        = 46,  // Key: .
    INPUT_KEYCODE_SLASH         = 47,  // Key: /
    INPUT_KEYCODE_ZERO          = 48,  // Key: 0
    INPUT_KEYCODE_ONE           = 49,  // Key: 1
    INPUT_KEYCODE_TWO           = 50,  // Key: 2
    INPUT_KEYCODE_THREE         = 51,  // Key: 3
    INPUT_KEYCODE_FOUR          = 52,  // Key: 4
    INPUT_KEYCODE_FIVE          = 53,  // Key: 5
    INPUT_KEYCODE_SIX           = 54,  // Key: 6
    INPUT_KEYCODE_SEVEN         = 55,  // Key: 7
    INPUT_KEYCODE_EIGHT         = 56,  // Key: 8
    INPUT_KEYCODE_NINE          = 57,  // Key: 9
    INPUT_KEYCODE_SEMICOLON     = 59,  // Key: ;
    INPUT_KEYCODE_EQUAL         = 61,  // Key: =
    INPUT_KEYCODE_A             = 65,  // Key: A | a
    INPUT_KEYCODE_B             = 66,  // Key: B | b
    INPUT_KEYCODE_C             = 67,  // Key: C | c
    INPUT_KEYCODE_D             = 68,  // Key: D | d
    INPUT_KEYCODE_E             = 69,  // Key: E | e
    INPUT_KEYCODE_F             = 70,  // Key: F | f
    INPUT_KEYCODE_G             = 71,  // Key: G | g
    INPUT_KEYCODE_H             = 72,  // Key: H | h
    INPUT_KEYCODE_I             = 73,  // Key: I | i
    INPUT_KEYCODE_J             = 74,  // Key: J | j
    INPUT_KEYCODE_K             = 75,  // Key: K | k
    INPUT_KEYCODE_L             = 76,  // Key: L | l
    INPUT_KEYCODE_M             = 77,  // Key: M | m
    INPUT_KEYCODE_N             = 78,  // Key: N | n
    INPUT_KEYCODE_O             = 79,  // Key: O | o
    INPUT_KEYCODE_P             = 80,  // Key: P | p
    INPUT_KEYCODE_Q             = 81,  // Key: Q | q
    INPUT_KEYCODE_R             = 82,  // Key: R | r
    INPUT_KEYCODE_S             = 83,  // Key: S | s
    INPUT_KEYCODE_T             = 84,  // Key: T | t
    INPUT_KEYCODE_U             = 85,  // Key: U | u
    INPUT_KEYCODE_V             = 86,  // Key: V | v
    INPUT_KEYCODE_W             = 87,  // Key: W | w
    INPUT_KEYCODE_X             = 88,  // Key: X | x
    INPUT_KEYCODE_Y             = 89,  // Key: Y | y
    INPUT_KEYCODE_Z             = 90,  // Key: Z | z
    INPUT_KEYCODE_LEFT_BRACKET  = 91,  // Key: [
    INPUT_KEYCODE_BACKSLASH     = 92,  // Key: '\'
    INPUT_KEYCODE_RIGHT_BRACKET = 93,  // Key: ]
    INPUT_KEYCODE_GRAVE         = 96,  // Key: `
    // Function keys
    INPUT_KEYCODE_SPACE         = 32,   // Key: Space
    INPUT_KEYCODE_ESCAPE        = 256,  // Key: Esc
    INPUT_KEYCODE_ENTER         = 257,  // Key: Enter
    INPUT_KEYCODE_TAB           = 258,  // Key: Tab
    INPUT_KEYCODE_BACKSPACE     = 259,  // Key: Backspace
    INPUT_KEYCODE_INSERT        = 260,  // Key: Ins
    INPUT_KEYCODE_DELETE        = 261,  // Key: Del
    INPUT_KEYCODE_RIGHT         = 262,  // Key: Cursor right
    INPUT_KEYCODE_LEFT          = 263,  // Key: Cursor left
    INPUT_KEYCODE_DOWN          = 264,  // Key: Cursor down
    INPUT_KEYCODE_UP            = 265,  // Key: Cursor up
    INPUT_KEYCODE_PAGE_UP       = 266,  // Key: Page up
    INPUT_KEYCODE_PAGE_DOWN     = 267,  // Key: Page down
    INPUT_KEYCODE_HOME          = 268,  // Key: Home
    INPUT_KEYCODE_END           = 269,  // Key: End
    INPUT_KEYCODE_CAPS_LOCK     = 280,  // Key: Caps lock
    INPUT_KEYCODE_SCROLL_LOCK   = 281,  // Key: Scroll down
    INPUT_KEYCODE_NUM_LOCK      = 282,  // Key: Num lock
    INPUT_KEYCODE_PRINT_SCREEN  = 283,  // Key: Print screen
    INPUT_KEYCODE_PAUSE         = 284,  // Key: Pause
    INPUT_KEYCODE_F1            = 290,  // Key: F1
    INPUT_KEYCODE_F2            = 291,  // Key: F2
    INPUT_KEYCODE_F3            = 292,  // Key: F3
    INPUT_KEYCODE_F4            = 293,  // Key: F4
    INPUT_KEYCODE_F5            = 294,  // Key: F5
    INPUT_KEYCODE_F6            = 295,  // Key: F6
    INPUT_KEYCODE_F7            = 296,  // Key: F7
    INPUT_KEYCODE_F8            = 297,  // Key: F8
    INPUT_KEYCODE_F9            = 298,  // Key: F9
    INPUT_KEYCODE_F10           = 299,  // Key: F10
    INPUT_KEYCODE_F11           = 300,  // Key: F11
    INPUT_KEYCODE_F12           = 301,  // Key: F12
    INPUT_KEYCODE_LEFT_SHIFT    = 340,  // Key: Shift left
    INPUT_KEYCODE_LEFT_CONTROL  = 341,  // Key: Control left
    INPUT_KEYCODE_LEFT_ALT      = 342,  // Key: Alt left
    INPUT_KEYCODE_LEFT_SUPER    = 343,  // Key: Super left
    INPUT_KEYCODE_RIGHT_SHIFT   = 344,  // Key: Shift right
    INPUT_KEYCODE_RIGHT_CONTROL = 345,  // Key: Control right
    INPUT_KEYCODE_RIGHT_ALT     = 346,  // Key: Alt right
    INPUT_KEYCODE_RIGHT_SUPER   = 347,  // Key: Super right
    INPUT_KEYCODE_KB_MENU       = 348,  // Key: KB menu
    // Keypad keys
    INPUT_KEYCODE_KP_0        = 320,  // Key: Keypad 0
    INPUT_KEYCODE_KP_1        = 321,  // Key: Keypad 1
    INPUT_KEYCODE_KP_2        = 322,  // Key: Keypad 2
    INPUT_KEYCODE_KP_3        = 323,  // Key: Keypad 3
    INPUT_KEYCODE_KP_4        = 324,  // Key: Keypad 4
    INPUT_KEYCODE_KP_5        = 325,  // Key: Keypad 5
    INPUT_KEYCODE_KP_6        = 326,  // Key: Keypad 6
    INPUT_KEYCODE_KP_7        = 327,  // Key: Keypad 7
    INPUT_KEYCODE_KP_8        = 328,  // Key: Keypad 8
    INPUT_KEYCODE_KP_9        = 329,  // Key: Keypad 9
    INPUT_KEYCODE_KP_DECIMAL  = 330,  // Key: Keypad .
    INPUT_KEYCODE_KP_DIVIDE   = 331,  // Key: Keypad /
    INPUT_KEYCODE_KP_MULTIPLY = 332,  // Key: Keypad *
    INPUT_KEYCODE_KP_SUBTRACT = 333,  // Key: Keypad -
    INPUT_KEYCODE_KP_ADD      = 334,  // Key: Keypad +
    INPUT_KEYCODE_KP_ENTER    = 335,  // Key: Keypad Enter
    INPUT_KEYCODE_KP_EQUAL    = 336,  // Key: Keypad =
    // Android key buttons
    INPUT_KEYCODE_BACK        = 4,   // Key: Android back button
    INPUT_KEYCODE_MENU        = 82,  // Key: Android menu button
    INPUT_KEYCODE_VOLUME_UP   = 24,  // Key: Android volume up button
    INPUT_KEYCODE_VOLUME_DOWN = 25   // Key: Android volume down button
};

enum
{
    INPUT_MOUSE_BUTTON_LEFT    = 0,  // Mouse button left
    INPUT_MOUSE_BUTTON_RIGHT   = 1,  // Mouse button right
    INPUT_MOUSE_BUTTON_MIDDLE  = 2,  // Mouse button middle (pressed wheel)
    INPUT_MOUSE_BUTTON_SIDE    = 3,  // Mouse button side (advanced mouse device)
    INPUT_MOUSE_BUTTON_EXTRA   = 4,  // Mouse button extra (advanced mouse device)
    INPUT_MOUSE_BUTTON_FORWARD = 5,  // Mouse button forward (advanced mouse device)
    INPUT_MOUSE_BUTTON_BACK    = 6,  // Mouse button back (advanced mouse device)
};


bool Input_IsKeyPressed(uint16_t key);
bool Input_IsKeyDown(uint16_t key);
bool Input_IsKeyReleased(uint16_t key);
bool Input_IsKeyUp(uint16_t key);

bool    Input_IsMouseButtonPressed(uint16_t button);
bool    Input_IsMouseButtonDown(uint16_t button);
bool    Input_IsMouseButtonReleased(uint16_t button);
bool    Input_IsMouseButtonUp(uint16_t button);
int16_t Input_GetMouseX(void);
int16_t Input_GetMouseY(void);
int16_t Input_GetMouseDeltaX(void);
int16_t Input_GetMouseDeltaY(void);
#endif
