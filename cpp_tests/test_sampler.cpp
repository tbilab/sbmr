#include<gtest/gtest.h>
#include <iostream>
#include <vector>
#include <memory>

#include "../Sampler.h"
#include "../helpers.h"

TEST(testSampler, vector_normalization){
  double tol = 0.01;
  // Setup generator
  Sampler my_sampler;
  
  // Sample vector
  std::vector<double> vec_raw = {1.0, 2.0, 3.0, 4.0};
  
  // Normalize vector
  std::vector<double> vec_norm = normalize_vector(vec_raw);
  
  EXPECT_EQ(vec_norm.size(), vec_raw.size());
  ASSERT_NEAR(vec_norm[0], 0.1, tol);
  ASSERT_NEAR(vec_norm[1], 0.2, tol);
  ASSERT_NEAR(vec_norm[2], 0.3, tol);
  ASSERT_NEAR(vec_norm[3], 0.4, tol);
}

TEST(testSampler, basic){
  // Setup generator
  Sampler my_sampler;

  double drawn_value = my_sampler.draw_unif();

  EXPECT_TRUE((drawn_value < 1.0) & (drawn_value > 0.0));
}

TEST(testSampler, set_seeds){
  
  // Setup multuple generators with same seed
  Sampler sampler_1(42);
  Sampler sampler_2(42);
  
  // The two samplers should give back the same result
  EXPECT_TRUE(sampler_1.draw_unif() == sampler_2.draw_unif());
}

TEST(testSampler, lots_of_samples){
  
  Sampler my_sampler;
  
  int num_samples = 1000;
  double min_draw = 100;
  double max_draw = -1;

  // Draw and add a bunch of values to a vector
  for (int i = 0; i < num_samples; ++i)
  {
    double current_draw = my_sampler.draw_unif();
    
    if (current_draw < min_draw) min_draw = current_draw;
    if (current_draw > max_draw) max_draw = current_draw;
  }
  
  EXPECT_TRUE(min_draw > 0.0);
  EXPECT_TRUE(max_draw < 1.0);
}

TEST(testSampler, drawing_from_weights){

  Sampler my_sampler;

  // Setup some weights
  std::vector<double> weights {0.1, 0.4, 0.3, 0.2};

  int chosen_index = my_sampler.sample(weights);

  // Hopefully chosen index is within range...
  EXPECT_TRUE(chosen_index < 4 & chosen_index > 0);

  int num_samples = 10000;
  int times_el_3_chosen = 0;
    // Sample index from function a bunch of times and record
  for (int i = 0; i < num_samples; ++i)
  {
    // Sample from weights and record if we chose the third element
    if (my_sampler.sample(weights) == 2) times_el_3_chosen++;
  }
   
  double prop_of_el_3 = double(times_el_3_chosen)/double(num_samples);

  // Make sure that the element was chosen roughly as much as it should have
  // been
  ASSERT_NEAR(
    prop_of_el_3,
    weights[2],
    0.01
  );
}


TEST(testSampler, uniform_integer_sampling){
  
  Sampler my_sampler;
  
  int num_samples = 1000;
  int max_val = 12;
  
  int min_draw = 100;
  int max_draw = -1;
  

  for (int i = 0; i < num_samples; ++i)
  {
    int current_draw = my_sampler.sample(max_val);
    
    if (current_draw < min_draw) min_draw = current_draw;
    if (current_draw > max_draw) max_draw = current_draw;
  }
  
  EXPECT_TRUE(min_draw == 0);
  EXPECT_TRUE(max_draw == max_val);
}


TEST(testSampler, node_list_sampling){
  
  Sampler my_sampler;
  
  // Build three nodes
  NodePtr n1 = std::make_shared<Node>("n1", 0, 1);
  NodePtr n2 = std::make_shared<Node>("n2", 0, 1);
  NodePtr n3 = std::make_shared<Node>("n3", 0, 1);
  
  // Add three nodes to a list
  std::list<NodePtr> my_nodes;
  my_nodes.push_back(n1);
  my_nodes.push_back(n2);
  my_nodes.push_back(n3);
  
  // Run a bunch of samples and makes sure we grab a given element rougly 1/3rd of the time
  int num_samples = 1000;
  int times_n2_sampled = 0;

  for (int i = 0; i < num_samples; ++i)
  {
    if (my_sampler.sample(my_nodes)->id == "n2") times_n2_sampled++;
  }

  double prop_n2_sampled = double(times_n2_sampled)/double(num_samples);

  ASSERT_NEAR(
    prop_n2_sampled,
    0.333333,
    0.03
  );
}

int main(int argc, char* argv[]){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
