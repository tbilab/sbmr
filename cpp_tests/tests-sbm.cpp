#include "catch.hpp"

#include <iomanip>

#include "../helpers.h"
#include "../SBM.h"


// Loads a simple bipartite sbm model with optional hierarchy added
SBM build_simple_SBM()
{
  
  // This function builds a network with the following structure
  /*
   +----+               +----+
   +----+   |    |---------------|    |
   |    |   | a1 |-----      ----| b1 |   +----+
   |a11 |---|    |      \  /    -|    |---|    |
   |    |   +----+       \/   /  +----+   |b11 |
   +----+   +----+       /\  /   +----+   |    |
   |    |----- /  \ ----|    |---|    |
   +----+   | a2 |---------------| b2 |   +----+      
   |    |---|    |       /    ---|    |   
   |a12 |   +----+      /   /    +----+   +----+
   |    |   +----+     /   /     +----+   |    |
   |    |---|    |----    /      |    |   |b12 |
   +----+   | a3 |-------        | b3 |---|    |
   |    |------    -----|    |   +----+      
   +----+   +----+      \  /     +----+   +----+
   |    |   +----+       \/      +----+   |    |
   |a13 |---|    |       /\      |    |   |b13 |
   |    |   | a4 |------/  \-----| b4 |---|    |
   +----+   |    |               |    |   +----+
   +----+               +----+
   */   
  SBM my_SBM;
  
  // Add nodes to graph first
  NodePtr a1 = my_SBM.add_node("a1", 0);
  NodePtr a2 = my_SBM.add_node("a2", 0);
  NodePtr a3 = my_SBM.add_node("a3", 0);
  NodePtr a4 = my_SBM.add_node("a4", 0);
  NodePtr b1 = my_SBM.add_node("b1", 1);
  NodePtr b2 = my_SBM.add_node("b2", 1);
  NodePtr b3 = my_SBM.add_node("b3", 1);
  NodePtr b4 = my_SBM.add_node("b4", 1);
  
  // Add connections
  my_SBM.add_connection(a1, b1);
  my_SBM.add_connection(a1, b2);
  my_SBM.add_connection(a2, b1);
  my_SBM.add_connection(a2, b2);
  my_SBM.add_connection(a3, b1);
  my_SBM.add_connection(a3, b2);
  my_SBM.add_connection(a3, b4);
  my_SBM.add_connection(a4, b3);
  
  // Make 2 type 0/a groups
  NodePtr a11 = my_SBM.add_node("a11", 0, 1);
  NodePtr a12 = my_SBM.add_node("a12", 0, 1);
  NodePtr a13 = my_SBM.add_node("a13", 0, 1);
  NodePtr b11 = my_SBM.add_node("b11", 1, 1);
  NodePtr b12 = my_SBM.add_node("b12", 1, 1);
  NodePtr b13 = my_SBM.add_node("b13", 1, 1);
  
  // Assign nodes to their groups
  a1->set_parent(a11);
  a2->set_parent(a12);
  a3->set_parent(a12);
  a4->set_parent(a13);
  b1->set_parent(b11);
  b2->set_parent(b11);
  b3->set_parent(b12);
  b4->set_parent(b13);
  
  
  return my_SBM;
}

// Gets average of the last n elements for a paseed vector of integers
inline float avg_last_n(std::vector<int> vec, int n)
{
  return std::accumulate(vec.end() - n, 
                         vec.end(), 
                         0.0 ) / float(n);
}

// Print a state dump for debugging purposes
inline void print_state_dump(State_Dump state)
{
  int n = state.id.size();

  for (int i = 0; i < n; i++)
  {
    std::cout << std::setw (7) << state.id[i] << ", " 
              << std::setw (7) << state.parent[i] << ", " 
              << std::setw (2) << state.level[i] << ", " << std::endl;
  }
}


TEST_CASE("Generate Node move proposals", "[SBM]")
{
    double tol = 0.01;
    double eps = 0.01;
    SBM my_SBM = build_simple_SBM();

    NodePtr a1 = my_SBM.get_node_by_id("a1");

    // Initialize a sampler to choose group
    Sampler my_sampler;

    int num_trials = 5000;
    int num_times_no_move = 0;
    NodePtr old_group = a1->parent;

    // Run multiple trials and of move and see how often a given node is moved
    for (int i = 0; i < num_trials; ++i)
    {
        // Do move attempt (dry run)
        NodePtr new_group = my_SBM.propose_move(a1, eps);

        if (new_group->id == old_group->id) num_times_no_move++;
    }

    double frac_of_time_no_change = double(num_times_no_move)/double(num_trials);

    // Prob of a1 staying in a1_1 should be approximately (2 + eps)/(6 + 4*eps)
    // Make sure model's decisions to move a1 reflects this.
    double two = 2;
    double six = 6;
    double four = 4;

    REQUIRE(
        Approx((two + eps)/(six + four*eps)).epsilon(tol) == 
        frac_of_time_no_change
    );
}


TEST_CASE("Calculate Model entropy", "[SBM]")
{
    // Setup simple SBM model
    SBM my_SBM = build_simple_SBM();

    EdgeCounts l1_edges = my_SBM.gather_edge_counts(1);

    // Compute full entropy at first level of nodes
    double model_entropy = my_SBM.compute_entropy(0);

    // Test entropy is near a hand-calculated value
    // Should be -8 - ( 2*log(2) + 3*log(6) ) - ( 2*log(2/12) + 4*log(4/30) + 1*log(1/5) + 1*log(1) ) = -1.509004
    REQUIRE(
        model_entropy == 
        Approx(-1.509004).epsilon(0.1)
    );


    // Calculate entropy delta caused by moving a node
    NodePtr node_to_move = my_SBM.get_node_by_id("a1");
    NodePtr from_group = node_to_move->parent;
    NodePtr to_group = my_SBM.get_node_by_id("a12", 1);

    // Calculate the entropy delta along with acceptance prob
    Proposal_Res proposal_results = my_SBM.compute_acceptance_prob(
        l1_edges,
        node_to_move,
        to_group,
        0.1,
        0.1
    );

    double entropy_delta = proposal_results.entropy_delta;

    // std::cout << "Moving a1 from a11->a12: prob = " << proposal_results.prob_of_accept << std::endl;

    // Now we will actually move the desired node and test to see if entropy has changed
    // Move node
    node_to_move->set_parent(to_group);

    // Recalculate entropy
    double new_entropy = my_SBM.compute_entropy(0);

    // Get difference from original
    double real_entropy_delta = model_entropy - new_entropy;

    REQUIRE(
        real_entropy_delta ==
        Approx(entropy_delta).epsilon(0.1) 
    );

    REQUIRE(
        proposal_results.entropy_delta == 
        entropy_delta
    );
}


TEST_CASE("Basic MCMC sweeps", "[SBM]")
{
    // Setup simple SBM model
    SBM my_SBM = build_simple_SBM();

    State_Dump pre_sweep = my_SBM.get_state();

    // Run a few rounds of sweeps of the MCMC algorithm on network 
    int num_sweeps = 1000;

    // Loop over a few different epsilon values
    std::vector<double> epsilons = {0.01, 0.9};
    std::vector<double> avg_num_moves;

    for (double eps : epsilons)
    {
        int total_num_changes = 0;

        for (int i = 0; i < num_sweeps; i++)
        {
            int n_changes = my_SBM.mcmc_sweep(0, false, eps, 1.0);
            total_num_changes += n_changes;
        }

        double avg_num_changes = double(total_num_changes)/double(num_sweeps);

        avg_num_moves.push_back(avg_num_changes);
        // std::cout << std::setw (5) << eps << " -- Avg number of changes = " << avg_num_changes << std::endl; 
    }

    // Make sure that we have a more move-prone model when we have a high epsilon value...
    REQUIRE(
        avg_num_moves.at(0) < avg_num_moves.at(epsilons.size() - 1)
    );
}


TEST_CASE("Agglomerative merge steps", "[SBM]")
{
    // Setup simple SBM model
    SBM my_SBM = build_simple_SBM();

    int num_initial_groups = my_SBM.get_level(1)->size();
    double initial_entropy = my_SBM.compute_entropy(0);

    std::cout << "Attempting single merge " << std::endl;
    // Run greedy aglomerative merge with best single merge done
    Merge_Res single_merge = my_SBM.agglomerative_merge(1, true, 5, 1, 0.01);

    // Make sure that we now have one less group than before for each type
    int new_group_num = my_SBM.get_level(1)->size();
    int change_in_groups = num_initial_groups - new_group_num;
    REQUIRE( change_in_groups == 1 );

    // Make sure entropy has gone down as we would expect
    REQUIRE(initial_entropy < single_merge.entropy);

    // Run again but this time merging the best 2
    SBM new_SBM = build_simple_SBM();

    std::cout << "Attempting double merge " << std::endl;
    // Run greedy aglomerative merge with best single merge done
    Merge_Res double_merge = new_SBM.agglomerative_merge(1, 2, true, 5, 0.01);

    // Make sure that we now have two fewer groups per type than before
    REQUIRE(
        2 == 
        num_initial_groups - new_SBM.get_level(1)->size()
    );

    // Entropy should go down even more with two merges
    REQUIRE(single_merge.entropy < double_merge.entropy);
}


// Currently not working due to bugs in agglomerative merging 
TEST_CASE("Agglomerative merging algorithm steps", "[SBM]")
{
  // Setup simple SBM model
  SBM my_SBM = build_simple_SBM();

  int num_initial_groups = my_SBM.get_level(1)->size();
  double initial_entropy = my_SBM.compute_entropy(0);

  // Run full agglomerative merging algorithm till we have just 3 groups left
  my_SBM.agglomerative_run(1,true,5,3,2,0.01);

  // std::cout << "Post-Merging..." << std::endl;
  // print_state_dump(my_SBM.get_state());

  // // Make sure that we now have one less group than before
  // REQUIRE(
  //   my_SBM.get_level(1)->size() == 
  //   3
  // );
}