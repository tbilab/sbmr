// =============================================================================
// include guard
// =============================================================================
#ifndef __SAMPLER_INCLUDED__
#define __SAMPLER_INCLUDED__

// #include <unordered_set>
#include <random>
#include <vector>
#include "helpers.h"
#include "Node.h"

typedef std::mt19937 rand_int_gen;
typedef std::uniform_real_distribution<> rand_unif_gen;

//=================================
// Main class declaration
//=================================
class Sampler {
  public:
    // Constructor
    Sampler();
    Sampler(int random_seed);
     
    // ==========================================
    // Attributes
    rand_int_gen   int_gen;  // Generates random integers
    rand_unif_gen  unif_gen; // Generates random uniforms
    
    // ==========================================
    // Methods   
    void     initialize_seed(int random_seed); // Sets stuff up for us, used to decide if we want to have deterministic seeding
    double   draw_unif();                      // Return random uniform value between 0 and 1.
    int      sample(int max_val);              // Sample from discrete random uniform from 0 to max
    NodePtr  sample(list<NodePtr>);            // Sample random node from a list of nodes
    NodePtr  sample(vector<NodePtr>);          // Sample random node from vector of nodes
      
    int      sample(std::vector<double> const &weights); // Sample an index from a vector of weights
      
  };

#endif