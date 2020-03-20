#include "../Sampler.h"
#include "../Node.h"
#include "print_helpers.h"
#include "catch.hpp"

#include <iostream>

TEST_CASE("Same seeds means same results", "[Sampler]")
{
  // Setup multuple generators with same seed
  Sampler sampler_1(42);
  Sampler sampler_2(42);

  for (int i = 0; i < 20; i++) {
    // Same result should occur over multiple samples
    REQUIRE(sampler_1.draw_unif() == sampler_2.draw_unif());
    REQUIRE(sampler_1.get_rand_int(100) == sampler_2.get_rand_int(100));
  }
}

TEST_CASE("Different seeds means different results", "[Sampler]")
{
  // Setup multuple generators with same seed
  Sampler sampler_1(42);
  Sampler sampler_2(312);
  for (int i = 0; i < 20; i++) {
    REQUIRE(sampler_1.draw_unif() != sampler_2.draw_unif());
    REQUIRE(sampler_1.get_rand_int(100) != sampler_2.get_rand_int(100));
  }
}

TEST_CASE("Lots of Samples", "[Sampler]")
{
  Sampler my_sampler;

  int num_samples = 1000;
  double min_draw = 100;
  double max_draw = -1;

  // Draw and add a bunch of values to a vector
  for (int i = 0; i < num_samples; ++i) {
    double current_draw = my_sampler.draw_unif();

    if (current_draw < min_draw)
      min_draw = current_draw;
    if (current_draw > max_draw)
      max_draw = current_draw;
  }

  REQUIRE(min_draw > 0.0);
  REQUIRE(max_draw < 1.0);
}

TEST_CASE("Sampling from nested vectors", "[Sampler]")
{

  std::vector<std::vector<int>> vec_of_vecs;

  // Make a vector of three different vectors each with three integers
  vec_of_vecs.push_back({0,1,2});
  vec_of_vecs.push_back({3,4,5});
  vec_of_vecs.push_back({6,7,8});

  // Initialize a random sampler and seed
  Sampler my_sampler(42);
  const int num_samples = 1000;

  int num_times_4 = 0;
  int num_times_3 = 0;

  for (int i = 0; i < num_samples; i++) {
    const int sampled_int = my_sampler.sample(vec_of_vecs, 9);

    if(sampled_int == 4) num_times_4++;

    if(sampled_int == 3) num_times_3++;
  }

  const double proportion_4 = double(num_times_4)/double(num_samples);
  const double proportion_3 = double(num_times_4)/double(num_samples);
  const double thresh = 0.01;
  const double true_prop = 1.0/9.0;
  // Hope that four is chosen 1/9th of the time
  REQUIRE(proportion_4 > true_prop - thresh);
  REQUIRE(proportion_4 < true_prop + thresh);

  REQUIRE(proportion_3 > true_prop - thresh);
  REQUIRE(proportion_3 < true_prop + thresh);
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

    if (current_draw < min_draw)
      min_draw = current_draw;
    if (current_draw > max_draw)
      max_draw = current_draw;
  }

  REQUIRE(min_draw == 0);
  REQUIRE(max_draw == max_val);
}

TEST_CASE("Node list and vector sampling", "[Sampler]")
{

  Sampler sampler_1(42);
  Sampler sampler_2(42);

  // Build three nodes
  Node_UPtr n1 = Node_UPtr(new Node { "n1", 0, 1 });
  Node_UPtr n2 = Node_UPtr(new Node { "n2", 0, 1 });
  Node_UPtr n3 = Node_UPtr(new Node { "n3", 0, 1 });

  // Add three nodes to a list
  Node_Vec nodes_list;
  nodes_list.push_back(n1.get());
  nodes_list.push_back(n2.get());
  nodes_list.push_back(n3.get());

  // Add three nodes to vector
  Node_Vec nodes_vec;
  nodes_vec.push_back(n1.get());
  nodes_vec.push_back(n2.get());
  nodes_vec.push_back(n3.get());

  // Run a bunch of samples and makes sure we grab a given element rougly 1/3rd of the time
  const int num_samples     = 10000;
  int times_n2_sampled_list = 0;
  int times_n2_sampled_vec  = 0;
  int times_list_agreed     = 0;
  int times_vec_agreed      = 0;

  for (int i = 0; i < num_samples; ++i) {
    std::string list_sample_id = sampler_1.sample(nodes_list)->id();
    std::string vec_sample_id  = sampler_1.sample(nodes_vec)->id();

    if (sampler_2.sample(nodes_list)->id() == list_sample_id) {
      times_list_agreed++;
    }
    if (sampler_2.sample(nodes_vec)->id() == vec_sample_id) {
      times_vec_agreed++;
    }

    if (list_sample_id == "n2") {
      times_n2_sampled_list++;
    }
    if (vec_sample_id == "n2") {
      times_n2_sampled_vec++;
    }
  }

  // Make sure list sampled a given correct amount
  REQUIRE(double(times_n2_sampled_list)
              / double(num_samples)
          == Approx(0.333333).epsilon(0.03));

  // Make sure vector sampled a given correct amount
  REQUIRE(double(times_n2_sampled_vec)
              / double(num_samples)
          == Approx(0.333333).epsilon(0.03));

  REQUIRE(times_list_agreed == num_samples);
  REQUIRE(times_vec_agreed == num_samples);
}

TEST_CASE("Node vector shuffling respects seeds", "[Sampler]")
{
  const int num_nodes   = 25;
  const int num_samples = 50;

  Sampler sampler_1(42);
  Sampler sampler_2(42);
  Sampler sampler_3(312);

  std::vector<Node_UPtr> nodes_master;
  Node_Vec nodes_vec1;
  Node_Vec nodes_vec2;
  Node_Vec nodes_vec3;
  nodes_master.reserve(num_nodes);
  nodes_vec1.reserve(num_nodes);
  nodes_vec2.reserve(num_nodes);
  nodes_vec3.reserve(num_nodes);

  // Build nodes and add to vectors
  for (int i = 0; i < num_nodes; i++) {
    nodes_master.emplace_back(new Node { "n" + std::to_string(i), 0, 1 });
    Node* new_node = nodes_master[i].get();
    nodes_vec1.push_back(new_node);
    nodes_vec2.push_back(new_node);
    nodes_vec3.push_back(new_node);
  }

  for (int i = 0; i < num_samples; ++i) {

    // Shuffle vec 1 with first sampler
    sampler_1.shuffle(nodes_vec1);
    sampler_2.shuffle(nodes_vec2);
    sampler_3.shuffle(nodes_vec3);

    int num_should_match   = 0;
    int num_shouldnt_match = 0;
    // Loop through all elements in both vectors and make sure they match
    for (int i = 0; i < num_nodes; i++) {
      if (nodes_vec1[i] == nodes_vec2[i])
        num_should_match++;
      if (nodes_vec1[i] == nodes_vec3[i])
        num_shouldnt_match++;
    }
    REQUIRE(num_should_match == num_nodes);
    REQUIRE(num_shouldnt_match < num_nodes);
  }
}
