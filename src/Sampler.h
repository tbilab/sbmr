// =============================================================================
// include guard
// =============================================================================
#ifndef __SAMPLER_INCLUDED__
#define __SAMPLER_INCLUDED__

#include "Node.h"

#include <random>

// This is a mouthful and confusing
using TwisterSeedMaker = std::mt19937::result_type;

//=================================
// Main class declaration
//=================================
class Sampler {
  public:
  // Constructors
  // ===========================================================================

  // Setup with a random seed based on clock
  Sampler()
      : generator(TwisterSeedMaker(time(0)))
  {
  }

  // Setup with a random seed based on passed seed
  Sampler(int seed)
      : generator(TwisterSeedMaker(seed))
  {
  }

  // Copy construction. Just pick up the old generator in its current state
  Sampler(const Sampler& old_sampler)
      : generator(old_sampler.generator)
  {
  }
  Sampler& operator=(const Sampler&) = delete;

  // Move operations
  Sampler(Sampler&&) = delete;
  Sampler& operator=(Sampler&&) = delete;

  // Attributes
  // ===========================================================================
  std::mt19937                     generator; // Generates random unsigned ints
  std::uniform_real_distribution<> unif_gen;  // Generates random uniforms

  // ==========================================
  // Methods
  double  draw_unif();                             // Return random uniform value between 0 and 1.
  int     get_rand_int(const int max_val);        // Sample from discrete random uniform from 0 to max
  NodePtr sample(const NodeList& nodes_to_sample); // Sample random node from a list of nodes
  NodePtr sample(const NodeVec& nodes_to_sample);  // Sample random node from vector of nodes
};

#endif