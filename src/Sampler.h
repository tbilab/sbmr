// =============================================================================
// include guard
// =============================================================================
#ifndef __SAMPLER_INCLUDED__
#define __SAMPLER_INCLUDED__

#include "error_and_message_macros.h"
#include <list>
#include <random>
#include <vector>

using Random_Engine = std::mt19937;
using C_Unif        = std::uniform_real_distribution<>;
using D_Unif        = std::uniform_int_distribution<>;

//=================================
// Main class declaration
//=================================
class Sampler {
  private:
  C_Unif gen_cont_unif;

  public:
  // Attributes
  // ===========================================================================
  Random_Engine generator; // Generates random unsigned ints

  // Constructors
  // ===========================================================================

  // Setup with a random seed based on clock
  Sampler()
  {
  }

  // Setup with a random seed based on passed seed
  Sampler(int seed)
      : generator(seed)
  {
  }

  // Copy construction. Just pick up the old generator in its current state
  Sampler(const Sampler& old_sampler)
      : generator(old_sampler.generator)
  {
  }

  // Move operations
  Sampler& operator  =(Sampler&& moved_sampler)
  {
    generator = std::move(moved_sampler.generator);
    return *this;
  };

  Sampler& operator=(const Sampler&) = delete;
  Sampler(Sampler&&) = delete;
  // ==========================================
  // Methods
  // =============================================================================
  // Draw a single sample from a random uniform (0 - 1] distribution
  // =============================================================================
  double draw_unif() { return gen_cont_unif(generator); }

  // =============================================================================
  // Draw single sample from a discrete random uniform (0 - max_val] distribution
  // =============================================================================
  int get_rand_int(const int max_val)
  {
    // Initialize a uniform random number generator with max at desired value
    D_Unif dist(0, max_val);

    return dist(generator);
  }

  // =============================================================================
  // Sample a random element from a list
  // =============================================================================
  template <typename T>
  T sample(const std::list<T>& node_list)
  {
    const int size_of_list = node_list.size();

    if (size_of_list == 0) {
      LOGIC_ERROR("List to sample has no elements.");
    }

    // Start an iterator at begining of list
    auto block_it = node_list.begin();

    // Select a random index to grab and advance list iterator till we've walked
    // the desired number of steps
    std::advance(block_it, get_rand_int(node_list.size() - 1));

    return *block_it;
  }

  // =============================================================================
  // Sample random node from vector
  // Easier than list because we can just index to a spot
  // =============================================================================
  template <typename T>
  T& sample(std::vector<T>& node_vec)
  {
    // Select a random index to return element at that index
    return node_vec.at(get_rand_int(node_vec.size() - 1));
  }

  template <typename T>
  T& sample(std::vector<std::vector<T>>& vec_of_vecs, int n = -1)
  {
    // If size was not provided, get it
    if (n == -1) {
      n = total_num_elements(vec_of_vecs);
    }

    int random_index = get_rand_int(n - 1);

    // Loop through subvectors and see if we can index into sub vector with random index
    // If we can't then subtract the current subvector size from random index and keep going
    for (auto& sub_vec : vec_of_vecs) {
      const int current_size = sub_vec.size();
      if (current_size <= random_index) {
        random_index -= current_size;
      } else {
        return sub_vec[random_index];
      }
    }
    LOGIC_ERROR("Random element could not be selected. Check formation of vectors");
    // Default return is just the first element... potentially dangerous
    return vec_of_vecs.at(0).at(0);
  }

  // =============================================================================
  // Shuffle a vector in-place
  // =============================================================================
  template <typename T>
  void shuffle(std::vector<T>& vec)
  {
    std::shuffle(vec.begin(), vec.end(), generator);
  }
};

#endif