#include "../Sampler.h"
#include "print_helpers.h"
#include "catch.hpp"

#include <iostream>

TEST_CASE("Same seeds means same results", "[Sampler]")
{
  // Setup multuple generators with same seed
  Sampler sampler_1(42);
  Sampler sampler_2(42);

  // The two samplers should give back the same result
  REQUIRE(sampler_1.draw_unif() == sampler_2.draw_unif());
}

TEST_CASE("Different seeds means different results", "[Sampler]")
{
  // Setup multuple generators with same seed
  Sampler sampler_1(42);
  Sampler sampler_2(312);

  // The two samplers should give back the same result
  REQUIRE(sampler_1.draw_unif() != sampler_2.draw_unif());
}


TEST_CASE("Lots of Samples", "[Sampler]")
{
  Sampler my_sampler;

  int    num_samples = 1000;
  double min_draw    = 100;
  double max_draw    = -1;

  // Draw and add a bunch of values to a vector
  for (int i = 0; i < num_samples; ++i) {
    double current_draw = my_sampler.draw_unif();

    if (current_draw < min_draw) min_draw = current_draw;
    if (current_draw > max_draw) max_draw = current_draw;
  }

  REQUIRE(min_draw > 0.0);
  REQUIRE(max_draw < 1.0);
}

TEST_CASE("Uniform integer sampling", "[Sampler]")
{
  Sampler my_sampler;

  int num_samples = 1000;
  int max_val     = 12;

  int min_draw = 100;
  int max_draw = -1;

  for (int i = 0; i < num_samples; ++i) {
    int current_draw = my_sampler.get_rand_int(max_val);

    if (current_draw < min_draw) min_draw = current_draw;
    if (current_draw > max_draw) max_draw = current_draw;
  }

  REQUIRE(min_draw == 0);
  REQUIRE(max_draw == max_val);
}

TEST_CASE("Node list and vector sampling", "[Sampler]")
{

  Sampler my_sampler;

  // Build three nodes
  NodePtr n1 = std::make_shared<Node>("n1", 0, 1);
  NodePtr n2 = std::make_shared<Node>("n2", 0, 1);
  NodePtr n3 = std::make_shared<Node>("n3", 0, 1);

  // Add three nodes to a list
  NodeList nodes_list;
  nodes_list.push_back(n1);
  nodes_list.push_back(n2);
  nodes_list.push_back(n3);

  // Add three nodes to vector
  NodeVec nodes_vec;
  nodes_vec.push_back(n1);
  nodes_vec.push_back(n2);
  nodes_vec.push_back(n3);

  // Run a bunch of samples and makes sure we grab a given element rougly 1/3rd of the time
  int num_samples           = 10000;
  int times_n2_sampled_list = 0;
  int times_n2_sampled_vec  = 0;

  for (int i = 0; i < num_samples; ++i) {
    if (my_sampler.sample(nodes_list)->id == "n2") times_n2_sampled_list++;
    if (my_sampler.sample(nodes_vec)->id == "n2") times_n2_sampled_vec++;
  }

  // Make sure list sampled a given correct amount
  REQUIRE(
      double(times_n2_sampled_list) / double(num_samples) == Approx(0.333333).epsilon(0.03));

  // Make sure vector sampled a given correct amount
  REQUIRE(
      double(times_n2_sampled_vec) / double(num_samples) == Approx(0.333333).epsilon(0.03));
}
