/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_console.hpp"

/// Common C++ headers
#include <iostream>
#include <string>
#include <cstdio>  // for getch()

#if SC_IS_PLATFORM_WIN32
#  include <windows.h>  // for WinAPI and Sleep()
#  define _NO_OLDNAMES  // for MinGW compatibility
#  include <conio.h>    // for getch() and kbhit()
#  define getch _getch
#  define kbhit _kbhit
#else
#  include <termios.h>    // for getch() and kbhit()
#  include <unistd.h>     // for getch(), kbhit() and (u)sleep()
#  include <sys/ioctl.h>  // for getkey()
#  include <ctime>        // for kbhit()

/// Function: getch
/// Get character without waiting for Return to be pressed.
/// Windows has this in conio.h
int getch()
{
  // Here be magic.
  struct termios oldt
  {
  }, newt{};
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}

/// Function: kbhit
/// Determines if keyboard has been hit.
/// Windows has this in conio.h
int kbhit()
{
  // Here be dragons.
  static struct termios oldt, newt;
  int cnt = 0;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_iflag = 0;      // input mode
  newt.c_oflag = 0;      // output mode
  newt.c_cc[VMIN] = 1;   // minimum time to wait
  newt.c_cc[VTIME] = 1;  // minimum characters to wait for
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ioctl(0, FIONREAD, &cnt);  // Read count
  struct timeval tv
  {
  };
  tv.tv_sec = 0;
  tv.tv_usec = 100;
  select(STDIN_FILENO + 1, nullptr, nullptr, nullptr, &tv);  // A small time delay
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return cnt;  // Return number of characters
}
#endif                    // SC_PLATFORM_WIN32

#define SC_CONSOLE_PRINT(st) \
  { \
    std::cout << st; \
  }

namespace impl
{
const std::string ANSI_CLS = "\033[2J\033[3J";
const std::string ANSI_CONSOLE_TITLE_PRE = "\033]0;";
const std::string ANSI_CONSOLE_TITLE_POST = "\007";
const std::string ANSI_ATTRIBUTE_RESET = "\033[0m";
const std::string ANSI_CURSOR_HIDE = "\033[?25l";
const std::string ANSI_CURSOR_SHOW = "\033[?25h";
const std::string ANSI_CURSOR_HOME = "\033[H";
const std::string ANSI_BLACK = "\033[22;30m";
const std::string ANSI_RED = "\033[22;31m";
const std::string ANSI_GREEN = "\033[22;32m";
const std::string ANSI_BROWN = "\033[22;33m";
const std::string ANSI_BLUE = "\033[22;34m";
const std::string ANSI_MAGENTA = "\033[22;35m";
const std::string ANSI_CYAN = "\033[22;36m";
const std::string ANSI_GREY = "\033[22;37m";
const std::string ANSI_DARKGREY = "\033[01;30m";
const std::string ANSI_LIGHTRED = "\033[01;31m";
const std::string ANSI_LIGHTGREEN = "\033[01;32m";
const std::string ANSI_YELLOW = "\033[01;33m";
const std::string ANSI_LIGHTBLUE = "\033[01;34m";
const std::string ANSI_LIGHTMAGENTA = "\033[01;35m";
const std::string ANSI_LIGHTCYAN = "\033[01;36m";
const std::string ANSI_WHITE = "\033[01;37m";
const std::string ANSI_BACKGROUND_BLACK = "\033[40m";
const std::string ANSI_BACKGROUND_RED = "\033[41m";
const std::string ANSI_BACKGROUND_GREEN = "\033[42m";
const std::string ANSI_BACKGROUND_YELLOW = "\033[43m";
const std::string ANSI_BACKGROUND_BLUE = "\033[44m";
const std::string ANSI_BACKGROUND_MAGENTA = "\033[45m";
const std::string ANSI_BACKGROUND_CYAN = "\033[46m";
const std::string ANSI_BACKGROUND_WHITE = "\033[47m";
const std::string ANSI_EMPTY = "";

/// Function: getkey
/// Reads a key press (blocking) and returns a key code.
///
/// See <Key codes for keyhit()>
///
/// Note:
/// Only Arrows, Esc, Enter and Space are currently working properly.
ScConsole::KeyCode GetKey()
{
#if !SC_IS_PLATFORM_WIN32
  int cnt = kbhit();  // for ANSI escapes processing
#endif
  int k = getch();
  switch (k)
  {
  case 0:
  {
    int kk;
    switch (kk = getch())
    {
    case 71:
      return ScConsole::KeyCode::NumPad7;
    case 72:
      return ScConsole::KeyCode::NumPad8;
    case 73:
      return ScConsole::KeyCode::NumPad9;
    case 75:
      return ScConsole::KeyCode::NumPad4;
    case 77:
      return ScConsole::KeyCode::NumPad6;
    case 79:
      return ScConsole::KeyCode::NumPad1;
    case 80:
      return ScConsole::KeyCode::NumPad2;
    case 81:
      return ScConsole::KeyCode::NumPad3;
    case 82:
      return ScConsole::KeyCode::NumPad0;
    case 83:
      return ScConsole::KeyCode::NumDel;
    default:
      return static_cast<ScConsole::KeyCode>(kk - 59 + static_cast<uint8_t>(ScConsole::KeyCode::F1));  // Function keys
    }
  }
  case 224:
  {
    int kk;
    switch (kk = getch())
    {
    case 71:
      return ScConsole::KeyCode::Home;
    case 72:
      return ScConsole::KeyCode::Up;
    case 73:
      return ScConsole::KeyCode::PageUp;
    case 75:
      return ScConsole::KeyCode::Left;
    case 77:
      return ScConsole::KeyCode::Right;
    case 79:
      return ScConsole::KeyCode::End;
    case 80:
      return ScConsole::KeyCode::Down;
    case 81:
      return ScConsole::KeyCode::PageDown;
    case 82:
      return ScConsole::KeyCode::Insert;
    case 83:
      return ScConsole::KeyCode::Delete;
    default:
      return static_cast<ScConsole::KeyCode>(kk - 123 + static_cast<uint8_t>(ScConsole::KeyCode::F1));  // Function keys
    }
  }
  case 13:
    return ScConsole::KeyCode::Enter;
#if SC_IS_PLATFORM_WIN32
  case 27:
    return ScConsole::KeyCode::Escape;
#else   // _WIN32
  case 155:  // single-character CSI
  case 27:
  {
    // Process ANSI escape sequences
    if (cnt >= 3 && getch() == '[')
    {
      switch (k = getch())
      {
      case 'A':
        return ScConsole::KeyCode::Up;
      case 'B':
        return ScConsole::KeyCode::Down;
      case 'C':
        return ScConsole::KeyCode::Right;
      case 'D':
        return ScConsole::KeyCode::Left;
      }
    }
    else
    {
      return ScConsole::KeyCode::Escape;
    }
  }
#endif  // _WIN32
  default:
    return static_cast<ScConsole::KeyCode>(k);
  }
}

int nb_getch()
{
  if (kbhit())
    return getch();
  else
    return 0;
}

std::string const & GetANSIColor(ScConsole::Color const c)
{
  switch (c)
  {
  case ScConsole::Color::Reset:
    return ANSI_ATTRIBUTE_RESET;
  case ScConsole::Color::Black:
    return ANSI_BLACK;
  case ScConsole::Color::Blue:
    return ANSI_BLUE;  // non-ANSI
  case ScConsole::Color::Green:
    return ANSI_GREEN;
  case ScConsole::Color::Cyan:
    return ANSI_CYAN;  // non-ANSI
  case ScConsole::Color::Red:
    return ANSI_RED;  // non-ANSI
  case ScConsole::Color::Magneta:
    return ANSI_MAGENTA;
  case ScConsole::Color::Brown:
    return ANSI_BROWN;
  case ScConsole::Color::Grey:
    return ANSI_GREY;
  case ScConsole::Color::DarkGrey:
    return ANSI_DARKGREY;
  case ScConsole::Color::LightBlue:
    return ANSI_LIGHTBLUE;  // non-ANSI
  case ScConsole::Color::LightGreen:
    return ANSI_LIGHTGREEN;
  case ScConsole::Color::LightCyan:
    return ANSI_LIGHTCYAN;  // non-ANSI;
  case ScConsole::Color::LightRed:
    return ANSI_LIGHTRED;  // non-ANSI;
  case ScConsole::Color::LightMagneta:
    return ANSI_LIGHTMAGENTA;
  case ScConsole::Color::Yellow:
    return ANSI_YELLOW;  // non-ANSI
  case ScConsole::Color::White:
    return ANSI_WHITE;
  }
  return ANSI_EMPTY;
}

std::string const & GetANSIBackgroundColor(ScConsole::Color const c)
{
  switch (c)
  {
  case ScConsole::Color::Reset:
    return ANSI_ATTRIBUTE_RESET;
  case ScConsole::Color::Black:
    return ANSI_BACKGROUND_BLACK;
  case ScConsole::Color::Blue:
    return ANSI_BACKGROUND_BLUE;
  case ScConsole::Color::Green:
    return ANSI_BACKGROUND_GREEN;
  case ScConsole::Color::Cyan:
    return ANSI_BACKGROUND_CYAN;
  case ScConsole::Color::Red:
    return ANSI_BACKGROUND_RED;
  case ScConsole::Color::Magneta:
    return ANSI_BACKGROUND_MAGENTA;
  case ScConsole::Color::Brown:
    return ANSI_BACKGROUND_YELLOW;
  case ScConsole::Color::Grey:
    return ANSI_BACKGROUND_WHITE;
  default:
    break;
  }
  return ANSI_EMPTY;
}

void SetCursorVisibility(bool isVisible)
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO structCursorInfo;
  GetConsoleCursorInfo(hConsoleOutput, &structCursorInfo);  // Get current cursor size
  structCursorInfo.bVisible = (isVisible ? TRUE : FALSE);
  SetConsoleCursorInfo(hConsoleOutput, &structCursorInfo);
#else
  SC_CONSOLE_PRINT((isVisible ? ANSI_CURSOR_SHOW : ANSI_CURSOR_HIDE));
#endif
}

}  // namespace impl

#if SC_IS_PLATFORM_WIN32
WORD ColorToWin(ScConsole::Color c)
{
  switch (c)
  {
  case ScConsole::Color::Blue:
    return FOREGROUND_BLUE;

  case ScConsole::Color::LightBlue:
    return FOREGROUND_BLUE | FOREGROUND_GREEN;

  case ScConsole::Color::Green:
  case ScConsole::Color::LightGreen:
    return FOREGROUND_GREEN;

  case ScConsole::Color::LightRed:
  case ScConsole::Color::Red:
    return FOREGROUND_RED;

  case ScConsole::Color::Magneta:
  case ScConsole::Color::LightMagneta:
    return FOREGROUND_BLUE | FOREGROUND_RED;

  case ScConsole::Color::Yellow:
    return FOREGROUND_GREEN | FOREGROUND_RED;

  case ScConsole::Color::Black:
    return 0;
  case ScConsole::Color::Grey:
  case ScConsole::Color::DarkGrey:
    return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

  default:
    break;
  }

  return FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
}
#endif

void ScConsole::SetColor(Color c)
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(hConsole, &csbi);

  SetConsoleTextAttribute(
      hConsole, (csbi.wAttributes & 0xFFF0) | ColorToWin(c));  // Foreground colors take up the least significant byte
#else
  SC_CONSOLE_PRINT(impl::GetANSIColor(c));
#endif
}

void ScConsole::SetBackgroundColor(Color c)
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(hConsole, &csbi);

  SetConsoleTextAttribute(
      hConsole,
      (csbi.wAttributes & 0xFF0F) | (((WORD)c) << 4));  // Background colors take up the second-least significant byte
#else
  SC_CONSOLE_PRINT(impl::GetANSIBackgroundColor(c));
#endif
}

ScConsole::Color ScConsole::GetDefaultColor()
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  static char initialized = 0;  // bool
  static WORD attributes;

  if (!initialized)
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    attributes = csbi.wAttributes;
    initialized = 1;
  }
  return static_cast<Color>(attributes);
#else
  return Color::White;
#endif
}

void ScConsole::ResetColor()
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)GetDefaultColor());
#else
  SC_CONSOLE_PRINT(impl::ANSI_ATTRIBUTE_RESET);
#endif
}

void ScConsole::SetCursorPos(int x, int y)
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  COORD coord;
  // TODO: clamping/assert for x/y <= 0?
  coord.X = (SHORT)(x - 1);
  coord.Y = (SHORT)(y - 1);  // Windows uses 0-based coordinates
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
  SC_CONSOLE_PRINT("\033[" << y << ";" << x << "H");
#endif
}

void ScConsole::Clear()
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  // Based on https://msdn.microsoft.com/en-us/library/windows/desktop/ms682022%28v=vs.85%29.aspx
  const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  const COORD coordScreen = {0, 0};
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(hConsole, &csbi);
  const DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);

  GetConsoleScreenBufferInfo(hConsole, &csbi);
  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

  SetConsoleCursorPosition(hConsole, coordScreen);
#else
  SC_CONSOLE_PRINT(impl::ANSI_CLS);
  SC_CONSOLE_PRINT(impl::ANSI_CURSOR_HOME);
#endif
}

void ScConsole::SetString(std::string const & str)
{
  auto len = static_cast<unsigned int>(str.size());

#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  const char * const s = str.data();

  HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD numberOfCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
  WriteConsoleOutputCharacter(hConsoleOutput, s, len, csbi.dwCursorPosition, &numberOfCharsWritten);
#else
  SC_CONSOLE_PRINT(str);
  SC_CONSOLE_PRINT("\033[" << len << 'D');
#endif
}

void ScConsole::ShowCursor()
{
  impl::SetCursorVisibility(true);
}

void ScConsole::HideCursor()
{
  impl::SetCursorVisibility(false);
}

int ScConsole::GetRowsNum()
{
#if SC_IS_PLATFORM_WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
  {
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;  // Window height
  }
  else
  {
    return -1;
  }
#else
#  ifdef TIOCGSIZE
  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
  return ts.ts_lines;
#  elif defined(TIOCGWINSZ)
  struct winsize ts
  {
  };
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
  return ts.ws_row;
#  else   // TIOCGSIZE
  return -1;
#  endif  // TIOCGSIZE
#endif    // WIN32
}

int ScConsole::GetColsNum()
{
#if SC_IS_PLATFORM_WIN32
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
  {
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;  // Window width
  }
  else
  {
    return -1;
  }
#else
#  ifdef TIOCGSIZE
  struct ttysize ts;
  ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
  return ts.ts_cols;
#  elif defined(TIOCGWINSZ)
  struct winsize ts
  {
  };
  ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
  return ts.ws_col;
#  else   // TIOCGSIZE
  return -1;
#  endif  // TIOCGSIZE
#endif    // _WIN32
}

void ScConsole::WaitAnyKey(std::string const & message)
{
  SC_CONSOLE_PRINT(message);
  getch();
}

void ScConsole::SetTitle(std::string const & title)
{
#if SC_IS_PLATFORM_WIN32 && !defined(SC_CONSOLE_USE_ANSI)
  SetConsoleTitleA(title.c_str());
#else
  SC_CONSOLE_PRINT(impl::ANSI_CONSOLE_TITLE_PRE);
  SC_CONSOLE_PRINT(title);
  SC_CONSOLE_PRINT(impl::ANSI_CONSOLE_TITLE_POST);
#endif
}

ScConsole::Output ScConsole::Print()
{
  return Output();
}

ScConsole::Output ScConsole::PrintLine()
{
  return Output(true);
}

void ScConsole::Endl()
{
  std::cout << std::endl;
}
