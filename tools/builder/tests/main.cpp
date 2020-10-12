
#include "sc-test-framework/sc_test_unit.hpp"

int main(int argc, char ** argv) try
{
  utils::ScLog::GetInstance()->SetFileName("sc-builder-test");
  test::ScTestUnit::RunAll(argc, argv);
  utils::ScLog::GetInstance()->Shutdown();
  SC_WAIT_KEY_IMPL()

  return 0;
}
catch (utils::ScException const & ex)
{
  ScMemory::LogUnmute();
  SC_LOG_ERROR(ex.Description());
  SC_WAIT_KEY_IMPL()
}
