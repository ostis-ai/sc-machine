#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "keynodes.generated.hpp"

class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  static ScAddr kNrelFormat();

private:
  SC_PROPERTY(Keynode("nrel_format"), ForceCreate(ScType::NodeConstNoRole))
  static ScAddr ms_kNrelFormat;
};
