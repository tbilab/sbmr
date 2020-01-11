#include "Sampler.h" 





// =============================================================================
// Setup the integer seeding device with user provided random seed, or -- if the
// random seed was passed as -1 -- with a computer generated seed
// =============================================================================
void Sampler::initialize_seed(int random_seed) 
{
  // If the random seed is the null value of -1, use the default
  // computer derived random seed. 
  if(random_seed == -1) 
  {
    //Will be used to obtain a seed for the random number engine
    std::random_device rd;  
    
    //Standard mersenne_twister_engine seeded with rd()
    rand_int_gen int_gen(rd()); 
  } 
  else 
  {
    // Otherwise, use the desired seed
    rand_int_gen int_gen(random_seed); 
  }
  
  // Setup the random uniform generation function
  rand_unif_gen unif_gen(0.0, 1.0);
}


// =============================================================================
// Draw a single sample from a random uniform (0 - 1] distribution
// =============================================================================
double Sampler::draw_unif() 
{
  return unif_gen(int_gen);
}

// =============================================================================
// Draw single sample from a discrete random uniform (0 - max_val] distribution
// =============================================================================
int Sampler::sample(int max_val) 
{
  std::uniform_int_distribution<int> dist(0, max_val);
  
  return dist(int_gen);
}


// =============================================================================
// Sample a random element from a list of node pointers
// =============================================================================
NodePtr Sampler::sample(std::list<NodePtr> node_list)
{
  // Select a random index to grab
  int random_index = sample(node_list.size()-1);
  
  // Start an iterator at begining of list
  auto block_it = node_list.begin();
  
  // Step through list till we've walked the desired number of steps to the
  // chosen index
  int step = 0;
  while (step != random_index) 
  {
    step++;
    block_it++;
  }
  
  return *block_it;
}


// =============================================================================
// Sample random node from vector of nodes
// Easier than list because we can just index to a spot
// =============================================================================
NodePtr Sampler::sample(std::vector<NodePtr> node_vec)
{
  // Select a random index to grab
  int random_index = sample(node_vec.size() - 1);
  
  // Return that element
  return node_vec.at(random_index);
}

void Sampler::shuffle_nodes(std::vector<NodePtr> &node_vec,
                            const std::shared_ptr<std::map<string, NodePtr>> &node_map,
                            std::mt19937 &sampler)
{
  // Initialize vector size to hold nodes
  node_vec.clear();

  // Fill in vector with map elements
  for (auto node_it = node_map->begin();
       node_it != node_map->end();
       node_it++)
  {
    node_vec.push_back(node_it->second);
  }

  // Shuffle node order
  std::shuffle(node_vec.begin(), node_vec.end(), sampler);
}
