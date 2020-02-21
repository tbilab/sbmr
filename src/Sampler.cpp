#include "Sampler.h"


// =============================================================================
// Draw a single sample from a random uniform (0 - 1] distribution
// =============================================================================
double Sampler::draw_unif()
{
  return unif_gen(generator);
}

// =============================================================================
// Draw single sample from a discrete random uniform (0 - max_val] distribution
// =============================================================================
int Sampler::get_rand_int(const int& max_val)
{
  // Initialize a uniform random number generator with max at desired value
  std::uniform_int_distribution<int> dist(0, max_val);

  return dist(generator);
}

// =============================================================================
// Sample a random element from a list of node pointers
// =============================================================================
NodePtr Sampler::sample(const NodeList& node_list)
{
  // Start an iterator at begining of list
  auto block_it = node_list.begin();
  
  // Select a random index to grab and advance list iterator till we've walked 
  // the desired number of steps
  std::advance(block_it, get_rand_int(node_list.size() - 1));

  // Return current element for iterator
  return *block_it;
}

// =============================================================================
// Sample random node from vector of nodes
// Easier than list because we can just index to a spot
// =============================================================================
NodePtr Sampler::sample(const NodeVec& node_vec)
{
  // Select a random index to return element at that index
  return node_vec.at(get_rand_int(node_vec.size() - 1));
}
