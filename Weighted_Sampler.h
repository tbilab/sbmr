//=================================
// include guard
//=================================
#ifndef __WEIGHTED_SAMPLER_INCLUDED__
#define __WEIGHTED_SAMPLER_INCLUDED__

// #include <unordered_set>
#include <random>
#include <vector>
#include "helpers.h"

typedef std::mt19937 rand_int_gen;
typedef std::uniform_real_distribution<> rand_unif_gen;

//=================================
// Main class declaration
//=================================
class Weighted_Sampler {
  public:
    // Constructor
    Weighted_Sampler();
    Weighted_Sampler(int random_seed);
     
    // ==========================================
    // Attributes
    rand_int_gen   int_gen;      // Generates random integers
    rand_unif_gen  unif_gen;     // Generates random uniforms
    
    // ==========================================
    // Methods   
    void     initialize_seed(int random_seed);                // Sets stuff up for us, used to decide if we want to have deterministic seeding
    double   draw_unif();                                     // Return random uniform value between 0 and 1.
    int      draw_unif_int(int max_val);                      // Sample from discrete random uniform from 0 to max
      
    int      sample(std::vector<double> const &weights);
      
  };

#endif