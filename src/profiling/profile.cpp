// This script serves as a portal into debugging. I use it by pasting in any
// tests that are failing and then stepping through to see what's going on.

#include <iostream>
#include "../helpers.h"
#include "../SBM.h"
#include "../cpp_tests/network_builders.cpp"
#include "Instrument.h"



int main(int argc, char **argv)
{ 

  Instrumentor::Get().BeginSession("Profile");

  // Setup simple SBM model
  SBM my_SBM = build_simulated_SBM();

  auto results = my_SBM.collapse_groups(0, 30);

  Instrumentor::Get().EndSession();
  return 0;
}
