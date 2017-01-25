/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

// Originaly got it there: https://github.com/tapio/rlutil/blob/master/rlutil.h

#include "../sc_defines.hpp"

#include <cinttypes>
#include <string>

class ScConsole final
{
public:
  enum class Color : uint8_t
  {
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magneta,
    Brown,
    Grey,
    DarkGrey,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagneta,
    Yellow,
    White
  };

  enum class KeyCode : uint8_t
  {
    Escape          = 0,
    Enter           = 1,
    Space           = 32,

    Insert          = 2,
    Home            = 3,
    PageUp          = 4,
    Delete          = 5,
    End             = 6,
    PageDown        = 7,

    Up              = 14,
    Down            = 15,
    Left            = 16,
    Right           = 17,

    F1              = 18,
    F2              = 19,
    F3              = 20,
    F4              = 21,
    F5              = 22,
    F6              = 23,
    F7              = 24,
    F8              = 25,
    F9              = 26,
    F10             = 27,
    F11             = 28,
    F12             = 29,

    NumDel          = 30,
    NumPad0         = 31,
    NumPad1         = 127,
    NumPad2         = 128,
    NumPad3         = 129,
    NumPad4         = 130,
    NumPad5         = 131,
    NumPad6         = 132,
    NumPad7         = 133,
    NumPad8         = 134,
    NumPad9         = 135
  };

  _SC_EXTERN static void SetColor(Color c);
  _SC_EXTERN static void SetBackgroundColor(Color c);
  _SC_EXTERN static Color GetDefaultColor();
  _SC_EXTERN static void ResetColor();

  _SC_EXTERN static void SetCursorPos(int x, int y);
  _SC_EXTERN static void Clear();

  /// Prints the supplied string without advancing the cursor
  _SC_EXTERN static void SetString(std::string const & str);

  _SC_EXTERN static void ShowCursor();
  _SC_EXTERN static void HideCursor();

  // Returns -1 on error
  _SC_EXTERN static int GetRowsNum();
  // Returns -1 on error
  _SC_EXTERN static int GetColsNum();

  _SC_EXTERN static void WaitAnyKey(std::string const & message);

  /// Set title of console window
  _SC_EXTERN static void SetTitle(std::string const & title);

  struct CursorHideGuard final
  {
    CursorHideGuard() { ScConsole::HideCursor(); }
    ~CursorHideGuard() { ScConsole::ShowCursor(); }
  };
};
