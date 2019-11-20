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
inline float avg_last_n(vector<int> vec, int n)
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

TEST_CASE("Basic initialization of network", "[SBM]")
{
    SBM my_SBM;

    // Add some nodes to SBM
    my_SBM.add_node("n1", 0);
    my_SBM.add_node("n2", 0);
    my_SBM.add_node("n3", 0);
    my_SBM.add_node("m1", 1);
    my_SBM.add_node("m2", 1);
    my_SBM.add_node("m3", 1);
    my_SBM.add_node("m4", 1);

    // Create a group node
    my_SBM.create_group_node(0, 1);
    my_SBM.create_group_node(1, 1);

    // How many nodes at the 'data' level do we have?
    REQUIRE(my_SBM.nodes.at(0)->size() == 7);

    REQUIRE(
        print_node_ids(*my_SBM.nodes.at(0)) ==
        "m1, m2, m3, m4, n1, n2, n3" 
    );

    // We should have two levels
    REQUIRE(my_SBM.nodes.size() == 2);

    // Group name convention <type>-<level>_<id>
    REQUIRE(
        print_node_ids(*my_SBM.nodes.at(1)) ==
        "0-1_0, 1-1_1"
    );

    // Filter to a given node type
    REQUIRE(
        "n1, n2, n3" == 
        print_node_ids(my_SBM.get_nodes_of_type_at_level(0,0))
    );
    REQUIRE(
        "m1, m2, m3, m4" == 
        print_node_ids(my_SBM.get_nodes_of_type_at_level(1,0))
    );
    REQUIRE(
        "1-1_1" == 
        print_node_ids(my_SBM.get_nodes_of_type_at_level(1,1))
    );
  
  // Get number of levels
  REQUIRE(my_SBM.nodes.size() == 2);
  
  // There should be two types of print_node_ids(*my_SBM.nodes.at(0))nodes
  REQUIRE(my_SBM.unique_node_types.size() == 2);
}

TEST_CASE("Tracking node types", "[SBM]")
{
  SBM my_SBM;
  
  // Add some nodes to SBM
  my_SBM.add_node("n1", 0);
  my_SBM.add_node("n2", 0);
  
  // There should only be one type of node so far
  REQUIRE(1 ==  my_SBM.unique_node_types.size());
  
  my_SBM.add_node("m1", 1);
  
  // There should now be two types of nodes
  REQUIRE(2 == my_SBM.unique_node_types.size());
  
  my_SBM.add_node("m2", 1);
  my_SBM.add_node("n3", 0);
  
  // There should still just be two types of nodes
  REQUIRE(2 == my_SBM.unique_node_types.size());
  
  my_SBM.add_node("m3", 1);
  my_SBM.add_node("o1", 2);
  my_SBM.add_node("o2", 2);
  
  // There should now be three types of nodes
  REQUIRE(3 == my_SBM.unique_node_types.size());
}


TEST_CASE("Initializing a group for every node", "[SBM]")
{
    SBM my_SBM;

    my_SBM.add_node("a1", 1);
    my_SBM.add_node("a2", 1);
    my_SBM.add_node("a3", 1);
    my_SBM.add_node("a4", 1);
    my_SBM.add_node("a5", 1);
    my_SBM.add_node("a10", 1);
    my_SBM.add_node("a11", 1);
    my_SBM.add_node("a13", 1);
    my_SBM.add_node("a14", 1);
    my_SBM.add_node("a6", 1);
    my_SBM.add_node("a7", 1);
    my_SBM.add_node("a8", 1);
    my_SBM.add_node("a9", 1);
    my_SBM.add_node("a12", 1);
    my_SBM.add_node("b1", 0);
    my_SBM.add_node("b2", 0);
    my_SBM.add_node("b3", 0);
    my_SBM.add_node("b4", 0);

    // There should be a total of 18 nodes at base level
    REQUIRE(18 == my_SBM.get_level(0)->size());

    // And zero nodes at the group level
    REQUIRE(0 == my_SBM.get_level(1)->size());

    // Now assignin every node their own parent group
    my_SBM.give_every_node_a_group_at_level(0);

    // There should now be a total of 18 nodes at level 1
    REQUIRE(18 == my_SBM.get_level(1)->size());
  
    // And a node from this new level should have a single child
    REQUIRE(1 == my_SBM.get_node_from_level(1)->children.size());
}

TEST_CASE("Cleaning up empty groups", "[SBM]")
{
    SBM my_SBM;

    // Start with a few nodes in the network
    NodePtr n1 = my_SBM.add_node("n1", 0);
    NodePtr n2 = my_SBM.add_node("n2", 0);
    NodePtr n3 = my_SBM.add_node("n3", 0);
    NodePtr n4 = my_SBM.add_node("n4", 0);

    // Create a few group nodes at first level
    NodePtr g1_1 = my_SBM.create_group_node(0, 1);
    NodePtr g1_2 = my_SBM.create_group_node(0, 1);
    NodePtr g1_3 = my_SBM.create_group_node(0, 1);
    NodePtr g1_4 = my_SBM.create_group_node(0, 1);

    // Create two groups for second level
    NodePtr g2_1 = my_SBM.create_group_node(0, 2);  
    NodePtr g2_2 = my_SBM.create_group_node(0, 2);

    // Add children to groups 1 and two at first level
    n1->set_parent(g1_1);
    n2->set_parent(g1_1);
    n3->set_parent(g1_2);
    n4->set_parent(g1_2);

    // Add children to both level two groups
    g1_1->set_parent(g2_1);
    g1_2->set_parent(g2_1);
    g1_3->set_parent(g2_1);
    g1_4->set_parent(g2_2);


    // Make sure our network is the proper size
    REQUIRE(3 == my_SBM.nodes.size());
    REQUIRE(4 == my_SBM.nodes.at(0)->size());
    REQUIRE(4 == my_SBM.nodes.at(1)->size());
    REQUIRE(2 == my_SBM.nodes.at(2)->size());


    // Run group cleanup
    int num_culled = my_SBM.clean_empty_groups();

    // Three groups should have been cleaned
    REQUIRE(3 == num_culled);

    // Two should have been taken from the first group level
    REQUIRE(2 == my_SBM.nodes.at(1)->size());

    // And 1 should have been taken from the second group level
    REQUIRE(1 == my_SBM.nodes.at(2)->size());

    // Run group cleanup again
    int num_culled_clean = my_SBM.clean_empty_groups();

    // No groups should have been culled
    REQUIRE(0 == num_culled_clean);
}

TEST_CASE("Building an edge count map", "[SBM]")
{
    SBM my_SBM;

    // Base-level nodes
    NodePtr a1 = my_SBM.add_node("a1", 0);
    NodePtr a2 = my_SBM.add_node("a2", 0);
    NodePtr a3 = my_SBM.add_node("a3", 0);
    NodePtr a4 = my_SBM.add_node("a4", 0);
    NodePtr b1 = my_SBM.add_node("b1", 1);
    NodePtr b2 = my_SBM.add_node("b2", 1);
    NodePtr b3 = my_SBM.add_node("b3", 1);
    NodePtr b4 = my_SBM.add_node("b4", 1);

    // level one groups
    NodePtr a11 = my_SBM.add_node("a11", 0, 1);
    NodePtr a12 = my_SBM.add_node("a12", 0, 1);
    NodePtr a13 = my_SBM.add_node("a13", 0, 1);
    NodePtr b11 = my_SBM.add_node("b11", 1, 1);
    NodePtr b12 = my_SBM.add_node("b12", 1, 1);
    NodePtr b13 = my_SBM.add_node("b13", 1, 1);

    // level two groups
    NodePtr a21 = my_SBM.add_node("a21", 0, 2);
    NodePtr a22 = my_SBM.add_node("a22", 0, 2);
    NodePtr b21 = my_SBM.add_node("b21", 1, 2);

    // Add connections
    my_SBM.add_connection(a1, b1);
    my_SBM.add_connection(a1, b2);

    my_SBM.add_connection(a2, b1);
    my_SBM.add_connection(a2, b2);
    my_SBM.add_connection(a2, b3);
    my_SBM.add_connection(a2, b4);

    my_SBM.add_connection(a3, b1);
    my_SBM.add_connection(a3, b2);
    my_SBM.add_connection(a3, b3);

    my_SBM.add_connection(a4, b1);
    my_SBM.add_connection(a4, b3);

    // Set hierarchy

    // Nodes -> level 1
    a1->set_parent(a11);
    a2->set_parent(a12);
    a3->set_parent(a12);
    a4->set_parent(a13);
    b1->set_parent(b11);
    b2->set_parent(b11);
    b3->set_parent(b12);
    b4->set_parent(b13);

    // level 1 -> level 2
    a11->set_parent(a21);
    a12->set_parent(a22);
    a13->set_parent(a22);
    b11->set_parent(b21);
    b12->set_parent(b21);
    b13->set_parent(b21);

    // Make sure our network is the proper size
    REQUIRE(3 == my_SBM.nodes.size());
    REQUIRE(8 == my_SBM.nodes.at(0)->size());
    REQUIRE(6 == my_SBM.nodes.at(1)->size());
    REQUIRE(3 == my_SBM.nodes.at(2)->size());

    // Build the network connection map for first level
    EdgeCounts l1_edges = my_SBM.gather_edge_counts(1);

    // The edge count map should have 6 non-empty entries 
    REQUIRE(6 == l1_edges.size());

    // Check num edges between groups
    REQUIRE(l1_edges[find_edges(a11, b11)] == 2);
    REQUIRE(l1_edges[find_edges(a11, b12)] == 0);
    REQUIRE(l1_edges[find_edges(a11, b13)] == 0);
    REQUIRE(l1_edges[find_edges(a12, b11)] == 4);
    REQUIRE(l1_edges[find_edges(a12, b12)] == 2);
    REQUIRE(l1_edges[find_edges(a12, b13)] == 1);
    REQUIRE(l1_edges[find_edges(a13, b11)] == 1);
    REQUIRE(l1_edges[find_edges(a13, b12)] == 1);
    REQUIRE(l1_edges[find_edges(a13, b13)] == 0);

    // Direction shouldn't matter
    REQUIRE(
        l1_edges[find_edges(a11, b11)] == 
        l1_edges[find_edges(b11, a11)]);

    REQUIRE(
        l1_edges[find_edges(a11, b12)] ==
        l1_edges[find_edges(b12, a11)]);

    REQUIRE(
        l1_edges[find_edges(a11, b13)] ==
        l1_edges[find_edges(b13, a11)]);

    REQUIRE(
        l1_edges[find_edges(a12, b11)] ==
        l1_edges[find_edges(b11, a12)]);

    REQUIRE(
        l1_edges[find_edges(a12, b12)] ==
        l1_edges[find_edges(b12, a12)]);

    REQUIRE(
        l1_edges[find_edges(a12, b13)] ==
        l1_edges[find_edges(b13, a12)]);

    REQUIRE(
        l1_edges[find_edges(a13, b11)] ==
        l1_edges[find_edges(b11, a13)]);

    REQUIRE(
        l1_edges[find_edges(a13, b12)] ==
        l1_edges[find_edges(b12, a13)]);

    REQUIRE(
        l1_edges[find_edges(a13, b13)] ==
        l1_edges[find_edges(b13, a13)]);

    // Repeat for level 2
    EdgeCounts l2_edges = my_SBM.gather_edge_counts(2);

    // Check num edges between groups
    REQUIRE(l2_edges[find_edges(a22, b21)] == 9);


    // Now we will change the group for a node and make sure the changes are
    // updated properly with the update_edge_counts() function

    // Change a3's parent from a12 to a13.
    a3->set_parent(a13);

    // Update the level 1 edge counts
    SBM::update_edge_counts(l1_edges, 1, a3, a12, a13);

    // Make sure that the needed change were made to a12's connections:
    REQUIRE(l1_edges[find_edges(a12, b11)] == 2);
    REQUIRE(l1_edges[find_edges(a12, b12)] == 1);
    REQUIRE(l1_edges[find_edges(a12, b13)] == 1);

    // And to a13's as well...
    REQUIRE(l1_edges[find_edges(a13, b11)] == 3);
    REQUIRE(l1_edges[find_edges(a13, b12)] == 2);
    REQUIRE(l1_edges[find_edges(a13, b13)] == 0);


    // Nothing should have changed for the level 2 connections

    // Update the level 2 edge counts
    SBM::update_edge_counts(l2_edges, 2, a3, a12, a13);

    // Check num edges between groups
    REQUIRE(l2_edges[find_edges(a21, b21)] == 2);
    REQUIRE(l2_edges[find_edges(a22, b21)] == 9);
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

    State_Dump pre_sweep = my_SBM.get_sbm_state();

    // Run a few rounds of sweeps of the MCMC algorithm on network 
    int num_sweeps = 1000;

    // Loop over a few different epsilon values
    vector<double> epsilons = {0.01, 0.9};
    vector<double> avg_num_moves;

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

    // Run greedy aglomerative merge with best single merge done
    Merge_Res single_merge = my_SBM.agglomerative_merge(1, true, 5, 1, 0.01);

    // Make sure that we now have one less group than before
    int new_group_num = my_SBM.get_level(1)->size();
    int change_in_groups = num_initial_groups - new_group_num;
    REQUIRE( change_in_groups == 1 );

    // Make sure entropy has gone down as we would expect
    REQUIRE(initial_entropy > single_merge.entropy);

    // Run again but this time merging the best 2
    SBM new_SBM = build_simple_SBM();

    // Run greedy aglomerative merge with best single merge done
    Merge_Res double_merge = new_SBM.agglomerative_merge(1, true, 5, 2, 0.01);

    // Make sure that we now have one less group than before
    REQUIRE(
        2 == 
        num_initial_groups - new_SBM.get_level(1)->size()
    );

    // Entropy should go down even more with two merges
    REQUIRE(single_merge.entropy > double_merge.entropy);
}


// Currently not working due to bugs in agglomerative merging 
// TEST_CASE("Agglomerative merging algorithm steps", "[SBM]")
// {
      
//     // Setup simple SBM model
//     SBM my_SBM = build_simple_SBM();

//     int level = 0;

//     std::cout << "Start..." << std::endl;
//     print_state_dump(my_SBM.get_sbm_state());

//     my_SBM.give_every_node_a_group_at_level(level);
//     my_SBM.clean_empty_groups();

//     std::cout << "Pre-Merging..." << std::endl;
//     print_state_dump(my_SBM.get_sbm_state());

//     Merge_Res double_merge = my_SBM.agglomerative_merge(1, true, 5, 2, 0.01);

//     // int num_initial_groups = my_SBM.get_level(1)->size();
//     // double initial_entropy = my_SBM.compute_entropy(0);

//     // // Run full agglomerative merging algorithm till we have just 3 groups left
//     // my_SBM.agglomerative_run(1,false,5,3,2,0.01);


//     std::cout << "Post-Merging..." << std::endl;
//     print_state_dump(my_SBM.get_sbm_state());

//     // // Make sure that we now have one less group than before
//     // REQUIRE(
//     //   my_SBM.get_level(1)->size() == 
//     //   3
//     // );
// }