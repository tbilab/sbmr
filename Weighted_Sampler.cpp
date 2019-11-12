#include "Weighted_Sampler.h" 


// =======================================================
// Constructor 
// =======================================================
Weighted_Sampler::Weighted_Sampler()
{
   // Setup with the default random seed
  initialize_seed(-1);
}

// When seed is provided, pass onto initialize seed
Weighted_Sampler::Weighted_Sampler(int random_seed)
{
  initialize_seed(random_seed);
}

void Weighted_Sampler::initialize_seed(int random_seed) 
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

double Weighted_Sampler::draw_unif() 
{
  return unif_gen(int_gen);
}

int Weighted_Sampler::sample(int max_val) 
{
  std::uniform_int_distribution<int> dist(0, max_val);
  
  return dist(int_gen);
}

int Weighted_Sampler::sample(std::vector<double> const &weights) 
{
  // Make sure weights sum to 1
  std::vector<double> weights_norm = normalize_vector(weights);

  // Draw a random uniform value
  double random_value = draw_unif();

  double current_sum = 0;
  int current_pos;

  // Scan up through weights, stopping when the current sum passes the drawn value
  for (current_pos = 0; current_pos < weights_norm.size(); ++current_pos) {
    current_sum += weights_norm[current_pos];

    // If we've gone over the random value then we're done
    if (current_sum >= random_value) break;
  }

  return current_pos;
}


NodePtr Weighted_Sampler::sample(list<NodePtr> node_list)
{
  // Select a random index to grab
  int random_index = sample(node_list.size()-1);
  
  // Start an iterator at begining of list
  auto group_it = node_list.begin();
  
  // Step through list till we've walked the desired number of steps to the
  // chosen index
  int step = 0;
  while (step != random_index) 
  {
    step++;
    group_it++;
  }
  
  return *group_it;
}