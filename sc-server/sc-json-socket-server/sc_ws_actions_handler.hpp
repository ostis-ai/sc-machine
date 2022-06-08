#pragma once

#include <string>

class ScWSActionsHandler
{
public:
  virtual std::string Handle(std::string const & requestMessageText) = 0;

  virtual ~ScWSActionsHandler() = default;
};
