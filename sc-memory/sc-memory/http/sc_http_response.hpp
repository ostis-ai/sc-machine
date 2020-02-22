#pragma once

#include "../sc_defines.hpp"
#include "../sc_utils.hpp"

#include <cstdint>
#include <string>

class ScHttpResponse
{
protected:
  friend class ScHttpRequest;

  _SC_EXTERN ScHttpResponse(std::string const & data, uint8_t resultCode);

public:
  _SC_EXTERN std::string const & GetData() const;
  _SC_EXTERN std::string GetResultCodeString() const;
  
  _SC_EXTERN bool IsSuccess() const;

private:
  std::string m_data;
  uint8_t m_resultCode;
};

SHARED_PTR_TYPE(ScHttpResponse)
