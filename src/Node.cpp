#include "Node.h"

#include <iostream>

// =============================================================================
// Replace 'this' with a shared smart pointer
// =============================================================================
inline NodePtr Node::this_ptr()
{
  return shared_from_this();
}

// =============================================================================
// Add edge to another node
// =============================================================================
inline void Node::add_edge(const NodePtr& node)
{
  //PROFILE_FUNCTION();

  // propigate new edge upwards to all parents
  NodePtr current_node  = this_ptr();
  int     current_level = level;

  while (current_node) {
    // Add node to base edges
    (current_node->edges).push_back(node);
    current_node->degree++;
    current_node = current_node->parent;
    current_level++;
  }
}

// =============================================================================
// Add or remove edges from nodes edge list
// =============================================================================
void Node::update_edges_from_node(const NodePtr& node, const bool& remove)
{
  // PROFILE_FUNCTION();

  // Grab list of nodes from node being removed or added we are updating
  const auto changed_node_edges = node->edges;

  // Keep track of which node in the hierarchy is being updated.
  // Starts with this node
  auto node_being_updated = this_ptr();

  // While we still have a node to continue to in the hierarchy...
  while (node_being_updated) {
    // Loop through all the edges that are being updated...
    // Grab reference to the current nodes edges list
    NodeList& curr_edges = node_being_updated->edges;

    for (const auto& edge_to_update : changed_node_edges) {
      if (remove) {
        // Scan through this nodes edges untill we find the first instance
        // of the connected node we want to remove
        auto last_place = curr_edges.end();
        for (auto con_it = curr_edges.begin();
             con_it != last_place;
             con_it++) {
          if (*con_it == edge_to_update) {
            curr_edges.erase(con_it);
            break;
          }
        }
      }
      else {
        // Just add this edge to nodes edges
        curr_edges.push_back(edge_to_update);
      }
    }

    // Update degree of current node
    node_being_updated->degree = node_being_updated->edges.size();

    // Update current node to nodes parent
    node_being_updated = node_being_updated->parent;
  }
}

// =============================================================================
// Set current node parent/cluster
// =============================================================================
void Node::set_parent(NodePtr parent_node_ptr)
{
  //PROFILE_FUNCTION();

  if (level != parent_node_ptr->level - 1) {
    LOGIC_ERROR("Parent node must be one level above child");
  }

  // Remove self from previous parents children list (if it existed)
  if (parent) {
    // Remove this node's edges contribution from parent's
    parent->update_edges_from_node(this_ptr(), true);

    // Remove self from previous children
    parent->remove_child(this_ptr());
  }

  // Set this node's parent
  parent = parent_node_ptr;

  // Add this node's edges to parent's degree count
  parent->update_edges_from_node(this_ptr(), false);

  // Add this node to new parent's children list
  parent_node_ptr->add_child(this_ptr());
}

// =============================================================================f
// Add a node to the children vector
// =============================================================================
inline void Node::add_child(const NodePtr& new_child_node)
{
  //PROFILE_FUNCTION();
  // Add new child node to the set of children. An unordered set is used because
  // repeat children can't happen.
  (this_ptr()->children).insert(new_child_node);
}

// =============================================================================
// Find and erase a child node
// =============================================================================
inline void Node::remove_child(const NodePtr& child_node)
{
  //PROFILE_FUNCTION();

  // children.erase(children.find(child_node));
  children.erase(child_node);
}

// =============================================================================
// Get parent of current node at a given level
// =============================================================================
inline NodePtr Node::get_parent_at_level(const int& level_of_parent) 
{
  // First we need to make sure that the requested level is not less than that
  // of the current node.
  if (level_of_parent < level) {
    LOGIC_ERROR("Requested parent level (" + std::to_string(level_of_parent) + ") lower than current node level (" + std::to_string(level) + ").");
  }

  // Start with this node as current node
  NodePtr current_node = this_ptr();

  while (current_node->level != level_of_parent) {
    if (!parent) {
      RANGE_ERROR("No parent at level " + std::to_string(level_of_parent) + " for " + id);
    }

    // Traverse up parents until we've reached just below where we want to go
    current_node = current_node->parent;
  }

  // Return the final node, aka the parent at desired level
  return current_node;
}

// =============================================================================
// Get all nodes connected to Node at a given level with specified type
// We return a vector because we need random access to elements in this array
// and that isn't provided to us with the list format.
// =============================================================================
NodeVec Node::get_edges_of_type(const std::string& node_type, const int& desired_level) const
{
  // Vector to return containing parents at desired level for edges
  NodeVec level_cons;

  // Conservatively assume all edges will be taken
  level_cons.reserve(edges.size());

  // Go through every child node's edges list, find parent at
  // desired level and place in connected nodes vector
  for (const auto& edge : edges) {
    if (edge->type == node_type) {
      level_cons.push_back(edge->get_parent_at_level(desired_level));
    }
  }

  return level_cons;
}

// =============================================================================
// Collapse a nodes edge to a given level into a map of
// connected block id->count
// =============================================================================
NodeEdgeMap Node::gather_edges_to_level(const int& level) const
{
  // Setup an edge count map for node
  NodeEdgeMap edges_counts;

  // Fill out edge count map by
  // - looping over all edges
  // - mapping them to the desired level
  // - and adding to their counts
  for (const NodePtr& curr_edge : edges) {
    edges_counts[curr_edge->get_parent_at_level(level)]++;
  }

  return edges_counts;
}

// =============================================================================
// Static method to connect two nodes to each other with edge
// =============================================================================
void Node::connect_nodes(const NodePtr& node1_ptr, const NodePtr& node2_ptr)
{
  //PROFILE_FUNCTION();
  node1_ptr->add_edge(node2_ptr);
  node2_ptr->add_edge(node1_ptr);
}
