// =============================================================================
// include guard
// =============================================================================
#ifndef __SAMPLER_INCLUDED__
#define __SAMPLER_INCLUDED__

#include <random>
#include "helpers.h"
#include "Node.h"

typedef std::mt19937 rand_int_gen;
typedef std::uniform_real_distribution<> rand_unif_gen;

//=================================
// Main class declaration
//=================================
class Sampler
{
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
  // Generates random integers
  rand_int_gen int_gen;   

  // Generates random uniforms
  rand_unif_gen unif_gen; 


  // ==========================================
  // Methods
  // Sets stuff up for us, used to decide if we want to have 
  // deterministic seeding
  void initialize_seed(int random_seed); 

  // Return random uniform value between 0 and 1.
  double draw_unif();                    

  // Sample from discrete random uniform from 0 to max
  int sample(int max_val);               

  // Sample random node from a list of nodes
  NodePtr sample(std::list<NodePtr> nodes_to_sample);    

  // Sample random node from vector of nodes
  NodePtr sample(std::vector<NodePtr> nodes_to_sample);  
};

#endif