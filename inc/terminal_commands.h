#pragma once

#include <string>

namespace debug
{

    #define NON_PRINT      "\e["       // Escape character
    #define EOM            "\e[0m"     // End of message, clear all the settings used befores
    #define COMBINED       ";"         // \e[CMD1;CMD2;CMD3 ...
    #define EOC            "m"         // End of attribute
    #define CLEAR_DISPLAY  "\e[2J"     // Clear display and reset cursor
    #define CLEAR_LINE     "\e[1K"     // Clear line and reset cursor
    #define RESET_CURSOR   "\r"        // Reset cursor position
    #define VERTICAL_TAB   "\v"        // Move cursor up (??)
    #define BELL           "\a"        // System bell
    #define MOVE_UP        "A"         // \e[<N>A
    #define MOVE_DW        "B"         // \e[<N>B
    #define MOVE_FW        "C"         // \e[<N>C
    #define MOVE_BW        "D"         // \e[<N>D

    // Messages state options
    #define INFO           "Info"
    #define STAT           "Stat"
    #define WARN           "Warn"
    #define ERROR          "Err "
    #define SYS            "Sys "

    // What you want to color?
    #define TEXT       3
    #define BACKGROUND 4
    #define LIGHTER    9

    /* Text/Foreground
    In order to colorise the part of the terminal that you choose
    (text, background, ...) you first choose the code number refered
    to the part of the temrinal that you want to print and then
    put the color code.
    Example: I want blue background so the code will be:
    BACKGROUND + BLUE = 44
    */
    #define BLACK   0
    #define RED     1
    #define GREEN   2
    #define YELLOW  3
    #define BLUE    4
    #define MAGENTA 5
    #define CYAN    6
    #define GRAY    7
    //#define XXX   8
    #define DEFAULT 9

    // Text formatting
    #define NORMAL     0
    #define BOLD       1
    #define DIM        2
    //#define XXX        3
    #define UNDERLINED 4
    #define BLINK      5
    #define INVERTED   6
    #define HIDDEN     7

    #define SEPARATOR " "

}; // namespace debug
