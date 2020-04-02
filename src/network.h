#pragma once

#include "Node.h"

#include "Block_Consensus.h"
#include "Sampler.h"
#include "agglomerative_merge.h"
#include "get_move_results.h"

#include "vector_helpers.h"

#include <unordered_map>

// These are seperate and will change based on compiler environemnt and only apply to the
// constructor and externally callable methods.
using Input_String_Vec = std::vector<string>;
using Input_Int_Vec    = std::vector<int>;

template <typename T>
using String_Map = std::unordered_map<string, T>;

struct State_Dump {
  std::vector<string> ids;
  std::vector<string> types;
  std::vector<string> parents;
  std::vector<int> levels;
  State_Dump() {};
  State_Dump(const int size)
  {
    ids.reserve(size);
    types.reserve(size);
    parents.reserve(size);
    levels.reserve(size);
  }
  int size() const { return ids.size(); }
};

enum Partite_Structure {
  unipartite,             // One node type
  multipartite,           // Multiple node types, all possible edges between non_like types allowed
  multipartite_restricted // Multiple node types, only specific type combos allowed for edges
};

struct MCMC_Sweeps {
  std::vector<double> sweep_entropy_delta;
  std::vector<int> sweep_num_nodes_moved;
  Block_Consensus block_consensus;
  std::vector<string> nodes_moved;
  double entropy_delta = 0.0;
  MCMC_Sweeps(const int n)
  {
    // Preallocate the entropy change and num groups moved in sweep vectors and
    sweep_entropy_delta.reserve(n);
    sweep_num_nodes_moved.reserve(n);
  }
};

struct Collapse_Results {
  double entropy_delta = 0; // Will keep track of the overall entropy change from this collapse
  int num_blocks;
  std::vector<Block_Mergers> merge_steps; // Will keep track of results at each step of the merger
  std::vector<State_Dump> states;
  Collapse_Results(const int n)
      : num_blocks(n)
  {
  }
};

class SBM_Network {

  private:
  // Data
  std::vector<Type_Vec> nodes;
  std::vector<string> types;
  Int_Map<string> type_name_to_int;
  std::map<int, std::set<int>> connection_types;
  String_Map<Node*> id_to_node;
  Partite_Structure edge_types;

  int block_counter = 0; // Keeps track of how many block we've had

  // =========================================================================
  // Private helper methods
  // =========================================================================
  int get_type_index(const string name) const
  {
    const auto name_it = type_name_to_int.find(name);

    // If this is a previously unseen type, add new entry
    if (name_it == type_name_to_int.end())
      LOGIC_ERROR("Type " + name + " doesn't exist in network");

    return name_it->second;
  }

  void check_for_level(const int level) const
  {
    // Make sure we have the requested level
    if (level >= nodes.size())
      RANGE_ERROR("Node requested in level that does not exist");
  }

  void check_for_type(const int type_index) const
  {
    if (type_index >= num_types())
      RANGE_ERROR("Type " + as_str(type_index) + " does not exist in network.");
  }

  void validate_edge(const int type_a, const int type_b, const bool loading = false)
  {
    if (edge_types == unipartite) {
      return; // Do nothing
    }

    if (loading || edge_types == multipartite) {
      // Load the type into connection types for later use
      connection_types[type_a].insert(type_b);
      connection_types[type_b].insert(type_a);
    } else {
      // If we're in a restricted multipartite network
      // make sure that this is an acceptable edgetype combo
      const bool edge_not_allowed = connection_types.at(type_a).count(type_b) == 0;

      if (edge_not_allowed)
        LOGIC_ERROR("Connection provided between nodes of types "
                    + types[type_a] + " & " + types[type_b]
                    + " which was not a specified combination in allowed edge types");
    }
  }

  Node* add_node(const std::string& id,
                 const int type_index = 0,
                 const int level      = 0)
  {
    // Build new node pointer outside of vector first for ease of pointer retrieval
    auto new_node = Node_UPtr(new Node(id, level, type_index, num_types()));

    // Get raw pointer to node to return
    Node* node_ptr = new_node.get();

    // Place this node in the id-to-node map if its a data-level node
    if (level == 0)
      id_to_node.emplace(id, node_ptr);

    // Move node unique pointer into its type in map
    nodes[level][type_index].push_back(std::move(new_node));

    return node_ptr;
  }

  Node* add_block_node(const int type_index, const int level = 1)
  {
    return add_node("bl_" + types[type_index] + "_" + as_str(block_counter++), type_index, level);
  }

  int num_nodes_of_type(const int type_i, const int level = 0) const
  {
    check_for_level(level);
    check_for_type(type_i);
    return nodes.at(level).at(type_i).size();
  }

  public:
  // Have sampler object be public for use by other functions
  Sampler sampler;

  // =========================================================================
  // Constructors
  // =========================================================================
  SBM_Network(const Input_String_Vec& node_ids,
              const Input_String_Vec& node_types,
              const Input_String_Vec& edges_a,
              const Input_String_Vec& edges_b,
              const Input_String_Vec& all_types,
              const int random_seed                   = 42,
              const Input_String_Vec& allowed_edges_a = {},
              const Input_String_Vec& allowed_edges_b = {})
      : sampler(random_seed)
      , types(all_types)
      , type_name_to_int(build_val_to_index_map(all_types))
      , edge_types(all_types.size() == 1
                       ? unipartite
                       : allowed_edges_a.size() == 0
                           ? multipartite
                           : multipartite_restricted)
  {
    build_block_level(node_ids.size()); // Setup empty first level of nodes with conservative space reserving

    // Fill in map to get allowed connection types if they are provided
    if (edge_types == multipartite_restricted) {
      for (int i = 0; i < allowed_edges_a.size(); i++) {
        validate_edge(get_type_index(allowed_edges_a[i]),
                      get_type_index(allowed_edges_b[i]), true);
      }
    }

    // Add nodes to network
    for (int i = 0; i < node_ids.size(); i++) {
      add_node(node_ids[i], node_types[i]);
    }

    // Connect nodes with edges
    for (int i = 0; i < edges_a.size(); i++) {
      add_edge(edges_a[i], edges_b[i]);
    }
  }

  // Builds a network without any nodes or edges
  // Calls the main constructor with empty node and edge vectors
  SBM_Network(const Input_String_Vec& all_types       = { "node" },
              const int random_seed                   = 42,
              const Input_String_Vec& allowed_edges_a = {},
              const Input_String_Vec& allowed_edges_b = {})
      : SBM_Network(Input_String_Vec {},
                    Input_String_Vec {},
                    Input_String_Vec {},
                    Input_String_Vec {},
                    all_types,
                    random_seed,
                    allowed_edges_a,
                    allowed_edges_b)
  {
  }

  // Move constructor
  SBM_Network(SBM_Network&& moved_net)
  {
    nodes            = std::move(moved_net.nodes);
    types            = std::move(moved_net.types);
    type_name_to_int = std::move(moved_net.type_name_to_int);
    connection_types = std::move(moved_net.connection_types);
    id_to_node       = std::move(moved_net.id_to_node);
    edge_types       = std::move(moved_net.edge_types);
    sampler          = std::move(moved_net.sampler);
    block_counter    = moved_net.block_counter;
  }

  // =========================================================================
  // Information
  // =========================================================================
  int num_nodes() const
  {
    return total_num_elements(nodes);
  }

  int num_nodes_at_level(const int level) const
  {
    check_for_level(level);
    return total_num_elements(nodes.at(level));
  }

  int num_levels() const
  {
    return nodes.size();
  }


  int num_nodes_of_type(const string& type, const int level = 0) const
  {
    check_for_level(level);
    return nodes.at(level).at(get_type_index(type)).size();
  }

  int num_types() const
  {
    return types.size();
  }

  int num_possible_neighbor_blocks(const int type, const int level) const
  {
    if (edge_types == unipartite) {
      return num_nodes_of_type(type, level + 1);
    }

    const std::set<int>& node_neighbor_types = connection_types.at(type);

    return std::accumulate(
        node_neighbor_types.begin(),
        node_neighbor_types.end(),
        0,
        [&](int n, const int type) {
          return n + num_nodes_of_type(type, level + 1);
        });
  }

  int num_possible_neighbor_blocks(Node* node) const
  {

    return num_possible_neighbor_blocks(node->type(), node->level());
  }

  // =========================================================================
  // Node Modification
  // =========================================================================
  Node* add_node(const std::string& id,
                 const std::string& type = "a",
                 const int level         = 0)
  {
    return add_node(id, get_type_index(type), level);
  }

  template <typename Node_Ref>
  void delete_node(const Node_Ref& node_to_remove)
  {
    auto& node_vector            = nodes[node_to_remove->level()][node_to_remove->type()];
    const bool delete_successful = delete_from_vector(node_vector, node_to_remove);

    if (!delete_successful)
      LOGIC_ERROR("Tried to delete a node that doesn't exist");
  }

  void add_edge(const string& node_a, const string& node_b)
  {
    Node* a = get_node_by_id(node_a);
    Node* b = get_node_by_id(node_b);

    validate_edge(a->type(), b->type());

    a->add_neighbor(b);
    b->add_neighbor(a);
  }

  void shuffle_nodes(const int type, const int level)
  {
    check_for_level(level);
    check_for_type(type);
    sampler.shuffle(nodes[level][type]);
  }

  // =========================================================================
  // Block Specific Modification (node levels 1+)
  // =========================================================================
  void build_block_level(const int reserve_size = 0)
  {
    nodes.emplace_back(num_types());

    // If we were told to reserve a size for each type vec, do so.
    if (reserve_size > 0) {
      for (auto& type_vec : nodes.at(num_levels() - 1)) {
        type_vec.reserve(reserve_size);
      }
    }
  }

  void initialize_blocks(int num_blocks = -1)
  {
    const bool one_block_per_node = num_blocks == -1;
    const int block_level         = num_levels();
    const int child_level         = block_level - 1;

    // Build empty level
    build_block_level(one_block_per_node ? 0 : num_blocks);

    // Loop over all node types
    for (int type_i = 0; type_i < num_types(); type_i++) {

      Node_UPtr_Vec& nodes_of_type  = nodes[child_level][type_i];
      Node_UPtr_Vec& blocks_of_type = nodes[child_level + 1][type_i];

      // If we're in the 1-block-per-node mode make sure we reflect that in reserved size
      if (one_block_per_node)
        num_blocks = nodes_of_type.size();

      if (num_blocks > nodes_of_type.size())
        LOGIC_ERROR("Can't initialize more blocks than there are nodes of a given type");

      // Reserve enough spaces for the blocks to be inserted
      blocks_of_type.reserve(num_blocks);

      for (int i = 0; i < num_blocks; i++) {
        add_block_node(type_i, block_level);
      }

      // Shuffle child nodes if we're randomly assigning blocks
      if (!one_block_per_node)
        sampler.shuffle(nodes_of_type);

      // Loop through now shuffled children nodes
      for (int i = 0; i < nodes_of_type.size(); i++) {
        // Add blocks one at a time, looping back after end to each node
        nodes_of_type[i]->set_parent(blocks_of_type[i % num_blocks].get());
      }
    }
  }

  void merge_blocks(Node* absorbed_block, Node* absorbing_block)
  {
    // Place all children of absorbed block into absorbing block
    for (const auto& child_node : absorbed_block->children()) {
      // Don't bother wasting computation on removing the child node
      // from the absorbed block's children's list.
      child_node->set_parent(absorbing_block, false);
    }

    // Remove all children before removing and trigger destructor
    absorbed_block->empty_children();
    delete_node(absorbed_block);
  }

  void remove_block_levels_above(const int last_level_index)
  {
    const int highest_index = num_levels() - 1;

    // Make sure request makes sense.
    if (last_level_index < 0) LOGIC_ERROR("Can't remove data level");
    if (last_level_index > highest_index) LOGIC_ERROR("Can't set highest level to "
                                                      + as_str(last_level_index)
                                                      + ", highest level in network is "
                                                      + as_str(highest_index));

    // Say we have three levels and want to get to just nodes (index = 0)
    // num_levels() = 3 - 0 - 1 -> remove 2 levels
    const int num_levels_to_remove = highest_index - last_level_index;

    for (int i = 0; i < num_levels_to_remove; i++) {
      // Remove the last layer of nodes.
      nodes.pop_back();
    }
  }

  // =============================================================================
  // Model Fitting
  // =============================================================================
  void swap_blocks(Node* child_node,
                   Node* new_block,
                   const bool remove_empty = true)
  {
    Node* old_block          = child_node->parent();
    const bool has_old_block = old_block != nullptr;

    child_node->set_parent(new_block);

    // If the old block is now empty and we're removing empty blocks, delete it
    if (has_old_block && remove_empty && old_block->is_empty()) {
      auto& block_vector           = nodes[old_block->level()][old_block->type()];
      const bool delete_successful = delete_from_vector(block_vector, old_block);

      if (!delete_successful)
        LOGIC_ERROR("Tried to delete a node that doesn't exist");
    }
  }

  Node* propose_move(Node* node, const int level_of_proposed, const double eps = 0.1)
  {
    // Sample a random neighbor block
    Node* neighbor_block = sampler.sample(node->neighbors(), node->degree())->parent();

    // Get all the nodes connected to neighbor block of the node-to-move's type
    const Node_Ptr_Vec& neighbor_edges_to_t = neighbor_block->neighbors_of_type(node->type());

    // Get a reference to all the blocks that the node-to-move _could_ join
    const Node_UPtr_Vec& all_potential_blocks = get_nodes_of_type(node->type(), level_of_proposed);

    // Decide if we are going to choose a random block for our node
    const double ergo_amnt = eps * all_potential_blocks.size();

    const bool draw_from_neighbor = sampler.draw_unif()
        > ergo_amnt / (double(neighbor_edges_to_t.size()) + ergo_amnt);

    // Decide where we will get new block from and draw from potential candidates
    if (draw_from_neighbor) {
      return sampler.sample(neighbor_edges_to_t)->parent_at_level(level_of_proposed);
    } else {
      return sampler.sample(all_potential_blocks).get();
    }
  }

  // Default proposal that returns a potential new parent block
  Node* propose_move(Node* node, const double eps = 0.1)
  {
    return propose_move(node, node->level() + 1, eps);
  }

  // Propose a merger with another node
  Node* propose_merge(Node* node, const double eps = 0.1)
  {
    return propose_move(node, node->level(), eps);
  }

  Collapse_Results collapse_blocks(const int node_level,
                                   const int B_end,
                                   const int n_checks_per_block,
                                   const int n_mcmc_sweeps,
                                   const double& sigma,
                                   const double& eps,
                                   const bool report_all_steps = true,
                                   const bool allow_exhaustive = true)
  {
    // Make sure we have at least one final block per node type
    if (num_types() > B_end) LOGIC_ERROR("Can't collapse a network with "
                                         + as_str(num_types()) + " node types to "
                                         + as_str(B_end)
                                         + " blocks.\n There needs to be at least one block per node type.");

    const int block_level = node_level + 1;
    const bool using_mcmc = n_mcmc_sweeps > 0;

    // Initialize struct to hold results of collapse
    auto results = Collapse_Results(B_end);

    // Remove any existing block level(s)
    remove_block_levels_above(node_level);

    // Initialize one-block-per-node
    initialize_blocks();

    // Setup variable to track the current number of blocks in the model
    int B_cur = num_nodes_at_level(block_level);

    // Lambda to calculate how many merges a step needs
    auto calc_num_merges = [&B_end, &sigma](const int B) {
      // How many blocks the sigma hueristic wants network to have after next move
      // max of this value and target is taken to avoid overshooting goal
      const int B_next = std::max(int(std::floor(double(B) / sigma)),
                                  B_end);

      return std::max(B - B_next, 1);
    };

    // Keep doing merges until we've reached the desired number of blocks
    while (B_cur > B_end) {
      const int n_merges_to_make = calc_num_merges(B_cur);

      // Perform merges
      auto merge_result = agglomerative_merge(this,
                                              block_level,
                                              n_merges_to_make,
                                              n_checks_per_block,
                                              eps,
                                              allow_exhaustive);
      // Update B_cur
      B_cur -= n_merges_to_make;

      if (using_mcmc) {
        // Update the merge results entropy delta with the changes caused by MCMC sweep
        merge_result.entropy_delta += mcmc_sweep(n_mcmc_sweeps,
                                                 eps,        // eps
                                                 false,      // variable num blocks
                                                 false,      // track pairs
                                                 node_level, // level
                                                 false)      // verbose
                                          .entropy_delta;

        // Check to see if we have any empty blocks after our MCMC sweep and remove them
        auto empty_blocks = Node_Vec();
        for_all_nodes_at_level(block_level, [&empty_blocks](const Node_UPtr& node) {
          if (node->is_empty()) empty_blocks.push_back(node.get());
        });

        // Update current number of blocks to account for the empty blocks
        B_cur -= empty_blocks.size();

        // Remove those empty blocks
        for (const auto& empty_block : empty_blocks) delete_node(empty_block);
      }

      // Update results stuct
      results.entropy_delta += merge_result.entropy_delta;

      if (report_all_steps) {
        results.merge_steps.push_back(merge_result);
        results.states.push_back(state());
      }
    }

    return results;
  }

  // =============================================================================
  // Runs efficient MCMC sweep algorithm on desired node level
  // =============================================================================
  MCMC_Sweeps mcmc_sweep(const int num_sweeps,
                         const double& eps,
                         const bool variable_num_blocks,
                         const bool track_pairs,
                         const int level    = 0,
                         const bool verbose = false)
  {
    const int block_level = level + 1;

    // Initialize structure that contains the returned values for this/these sweeps
    MCMC_Sweeps results(num_sweeps);

    // Initialize pair tracking map if needed
    if (track_pairs) {
      results.block_consensus.initialize(get_nodes_at_level(level));
    }

    // Check if we have any blocks ready in the network...
    const bool no_blocks_present = num_levels() > block_level + 1;

    if (no_blocks_present) {
      initialize_blocks();

      if (verbose) {
        WARN_ABOUT("No blocks present. Initializing one block per node.");
      }
    }

    // If we are allowing a variable number of blocks, initialize an empty block for each node type
    if (variable_num_blocks) {
      for (int type = 0; type < num_types(); type++) {
        add_block_node(type, block_level);
      }
    }

    if (verbose) {
      OUT_MSG << "sweep_num,"
              << "node,"
              << "current_block,"
              << "proposed_block,"
              << "entropy_delta,"
              << "prob_of_accept,"
              << "move_accepted" << std::endl;
    }

    // Initialize a vector of nodes that will be passed through for a sweep.
    auto nodes = get_flat_level(level);

    for (int i = 0; i < num_sweeps; i++) {
      // Book keeper variables for this sweeps stats
      int num_nodes_moved  = 0;
      double entropy_delta = 0;

      // Shuffle order of nodes to be run through for sweep
      sampler.shuffle(nodes);

      // Setup container to track what pairs of nodes need to have their consensus membership updated for this sweep
      Pair_Set pair_moves;

      int steps_taken = 0;
      // Loop through each node
      for (const auto& curr_node : nodes) {
        // Get a move proposal
        Node* proposed_new_block = propose_move(curr_node, eps);

        Node* old_block = curr_node->parent();

        // If the proposed block is the nodes current block, we don't need to waste
        // time checking because decision will always result in same state.
        if (old_block == proposed_new_block) {
          continue;
        }

        if (verbose) {
          OUT_MSG << i
                  << "," << curr_node->id()
                  << "," << curr_node->parent()->id()
                  << "," << proposed_new_block->id()
                  << ",";
        }
        // Calculate acceptance probability based on posterior changes
        auto proposal_results = get_move_results(curr_node,
                                                 proposed_new_block,
                                                 num_possible_neighbor_blocks(curr_node),
                                                 eps);

        // Make movement decision
        const bool move_accepted = proposal_results.prob_of_accept > sampler.draw_unif();

        if (verbose) {
          OUT_MSG << proposal_results.entropy_delta << "," << proposal_results.prob_of_accept << ","
                  << move_accepted << std::endl;
        }

        // Is the move accepted?
        if (move_accepted) {

          bool remove_empty_block = variable_num_blocks;
          if (variable_num_blocks) {
            // If the old block will still have children after the move and
            // the new block is empty block, this move will cause there to be no
            // empty blocks for this type
            const bool old_wont_be_empty = old_block->num_children() > 1;

            if (proposed_new_block->is_empty()) {
              if (old_wont_be_empty) {
                // Need to add a new block node as we wont have any empty ones left
                add_block_node(curr_node->type(), block_level);
              } else {
                // In this case the old block will become to the empty block so we
                // don't want it to be removed by the swap_blocks function
                remove_empty_block = false;
              }
            }
          }

          swap_blocks(curr_node, proposed_new_block, remove_empty_block);

          // Update results
          results.nodes_moved.push_back(curr_node->id());
          num_nodes_moved++;
          entropy_delta += proposal_results.entropy_delta;

          if (track_pairs) {
            Block_Consensus::update_changed_pairs(curr_node->id(),
                                                  old_block->children(),
                                                  proposed_new_block->children(),
                                                  pair_moves);
          }

        } // End accepted if statement

        // Check for user breakout every 100 iterations.
        steps_taken = (steps_taken + 1) % 100;
        if (steps_taken == 0) {
          ALLOW_USER_BREAKOUT;
        }
      } // End current sweep

      // Update results for this sweep
      results.sweep_num_nodes_moved.push_back(num_nodes_moved);
      results.sweep_entropy_delta.push_back(entropy_delta);
      results.entropy_delta += entropy_delta;

      // Update the concensus pairs map with results if needed.
      if (track_pairs) {
        results.block_consensus.update_pair_tracking_map(pair_moves);
      }
      ALLOW_USER_BREAKOUT; // Let R used break out of loop if need be
    }                      // End multi-sweep loop

    if (variable_num_blocks) {
      // Cleanup the single empty block for each type
      for (const auto& blocks_of_type : get_nodes_at_level(block_level)) {
        for (const auto& block : blocks_of_type) {
          if (block->is_empty()) {
            delete_node(block);
            break; // No need to continue so break out (also would cause problems because we just modified the vector we're looping over)
          }
        }
      }
    }
    return results;
  }

  // =============================================================================
  // Model State
  // =============================================================================
  State_Dump state() const
  {
    if (num_levels() == 1)
      LOGIC_ERROR("No state to export - Try adding blocks");

    // Initialize the return struct
    State_Dump state(num_nodes());

    // No need to record the last level's nodes as they are already included
    // in the previous node's parent slot
    for (int level = 0; level < num_levels() - 1; level++) {
      for_all_nodes_at_level(level, [&](const Node_UPtr& node) {
        state.ids.push_back(node->id());
        state.types.push_back(types[node->type()]);
        state.parents.push_back(node->parent()->id());
        state.levels.push_back(level);
      });
    }

    return state;
  }

  void update_state(const State_Dump& state)
  {
    remove_block_levels_above(0); // Remove all block levels
    build_block_level();          // Add an empty block level to fill in

    // Make a copy of the id_to_node map (We will later overwrite it)
    String_Map<Node*> node_by_id = id_to_node;

    // Setup map to get blocks/parents by id
    String_Map<Node*> block_by_id;

    // Loop through entries of the state dump
    int last_level = 0;
    for (int i = 0; i < state.size(); i++) {
      const string& id     = state.ids[i];
      const string& parent = state.parents[i];
      const string& type   = state.types[i];
      const int level      = state.levels[i];

      // If the level of the current entry has gone up
      // Swap the maps as the blocks are now the child nodes
      if (last_level != level) {
        node_by_id = std::move(block_by_id); // block_by_id will be empty now
        build_block_level();                 // Setup new level for blocks
        last_level = level;                  // Update the current level
      }

      // Find current entry's node
      Node* current_node = [&]() {
        const auto node_it = node_by_id.find(id);
        if (node_it == node_by_id.end())
          LOGIC_ERROR("Node in state (" + id + ") is not present in network");
        return node_it->second;
      }();

      // Grab parent block pointer
      Node* parent_node = [&]() {
        const auto parent_it = block_by_id.find(parent);
        // If this block is newly seen, create it
        if (parent_it == block_by_id.end()) {
          block_counter++;
          return block_by_id.emplace(parent, add_node(parent, type, level + 1)).first->second;
        }
        return parent_it->second;
      }();

      // Connect node and parent to eachother
      current_node->set_parent(parent_node);
    }
    // update_state(state.ids, state.parents, state.levels, state.types);
  }

  // =========================================================================
  // Node Grabbers
  // =========================================================================
  const Type_Vec& get_nodes_at_level(const int level) const
  {
    check_for_level(level);
    return nodes[level];
  }

  // Get a vector of raw pointers to all nodes in a given level with no type separation
  Node_Vec get_flat_level(const int level) const
  {
    Node_Vec all_nodes;
    all_nodes.reserve(num_nodes_at_level(level));

    for_all_nodes_at_level(level, [&all_nodes](const Node_UPtr& node) {
      all_nodes.push_back(node.get());
    });

    return all_nodes;
  }

  const Node_UPtr_Vec& get_nodes_of_type(const int type_index, const int level = 0) const
  {
    check_for_level(level);
    check_for_type(type_index);
    return nodes[level][type_index];
  }

  const Node_UPtr_Vec& get_nodes_of_type(const std::string& type, const int level = 0) const
  {
    return get_nodes_of_type(get_type_index(type), level);
  }

  // Apply a lambda function over all nodes in network
  void for_all_nodes_at_level(const int level,
                              std::function<void(const Node_UPtr& node)> fn) const
  {
    check_for_level(level);
    for (const auto& nodes_vec : nodes.at(level)) {
      std::for_each(nodes_vec.begin(), nodes_vec.end(), fn);
    }
  }

  Node* get_node_by_id(const string& id) const
  {
    const auto node_it = id_to_node.find(id);

    if (node_it == id_to_node.end()) {
      LOGIC_ERROR("Node " + id + " not found in network");
    }

    return node_it->second;
  }
};