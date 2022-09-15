#pragma once

#include <istream>
#include <streambuf>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <algorithm>

#include "../../sc-core/sc-store/sc_types.h"
#include "../sc_debug.hpp"

/// Class to create and execute system commands
class ScExec : public std::istream
{
public:
  /*! Creates command subprocess and execute it
   * @content Command content
   */
  explicit ScExec(std::vector<std::string> const & content)
    : std::istream(nullptr)
  {
    std::stringstream stream;

    if (!content.empty())
    {
      std::for_each(content.cbegin(), --content.cend(), [&stream](auto const & item) {
        stream << item << " ";
      });
      stream << *--content.cend();
    }

    m_buffer = new ScExecBuffer(stream.str().c_str());
    rdbuf(m_buffer);
  }

  ~ScExec() override
  {
    delete m_buffer;
  }

protected:
  class ScExecBuffer : public std::streambuf
  {
  public:
    explicit ScExecBuffer(sc_char const * command)
    {
      std::array<sc_char, 256> buffer{};
      std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
      if (!pipe)
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Invalid ScExec body");
      }
      while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
      {
        output += buffer.data();
      }
      setg((sc_char *)output.c_str(), (sc_char *)output.c_str(), (sc_char *)(output.c_str() + output.size()));
    }

  protected:
    std::string output;
  };

  ScExecBuffer * m_buffer;
};
