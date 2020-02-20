// =============================================================================
// include guard
// =============================================================================
#ifndef __SAMPLER_INCLUDED__
#define __SAMPLER_INCLUDED__

#include "Node.h"

#include <random>

typedef std::mt19937                     rand_int_gen;
typedef std::uniform_real_distribution<> rand_unif_gen;

//=================================
// Main class declaration
//=================================
class Sampler {
  public:
  // Constructors
  // ===========================================================================
  Sampler()
  {
    // Setup with the default random seed
    initialize_seed(-1);
  }

  // When seed is provided, pass onto initialize seed
  Sampler(int random_seed)
  {
    initialize_seed(random_seed);
  }

  // Attributes
  // ===========================================================================
  rand_int_gen  int_gen;  // Generates random integers
  rand_unif_gen unif_gen; // Generates random uniforms

  // ==========================================
  // Methods

  void    initialize_seed(int random_seed);        // Used to decide if we want to have deterministic seeding
  double  draw_unif();                             // Return random uniform value between 0 and 1.
  int     sample(const int& max_val);                     // Sample from discrete random uniform from 0 to max
  NodePtr sample(const NodeList& nodes_to_sample); // Sample random node from a list of nodes
  NodePtr sample(const NodeVec& nodes_to_sample);  // Sample random node from vector of nodes

};

#endif