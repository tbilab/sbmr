#include "Node.h"


// =============================================================================
// Replace 'this' with a shared smart pointer
// =============================================================================
inline NodePtr Node::this_ptr()
{
  return shared_from_this();
}

// =============================================================================
// Add connection to another node
// =============================================================================
inline void Node::add_connection(const NodePtr node)
{
  //PROFILE_FUNCTION();

  // propigate new connection upwards to all parents
  NodePtr current_node = this_ptr();
  while (current_node)
  {
    (current_node->connections).push_back(node);
    current_node->degree++;
    current_node = current_node->parent;
  }
}

// =============================================================================
// Add or remove connections from nodes connection list
// =============================================================================
void Node::update_connections_from_node(const NodePtr node, const bool remove)
{
  // PROFILE_FUNCTION();

  // Grab list of nodes from node being removed or added we are updating
  auto connections_being_updated = node->connections;

  // Keep track of which node in the hierarchy is being updated. 
  // Starts with this node
  auto node_being_updated = this_ptr();

  // While we still have a node to continue to in the hierarchy...
  while (node_being_updated)
  {
    // Loop through all the connections that are being updated...
    for (auto & connection_to_update : connections_being_updated)
    {
      // Grab reference to the current nodes connections list
      std::list<NodePtr>& curr_connections = node_being_updated->connections;

      if (remove)
      {
        // Scan through this nodes connections untill we find the first instance
        // of the connected node we want to remove
        auto last_place = curr_connections.end();
        for (auto con_it = curr_connections.begin();
             con_it != last_place;
             con_it++)
        {
          if (*con_it == connection_to_update)
          {
            curr_connections.erase(con_it);
            break;
          }
        }
      }
      else
      {
        // Just add this connection to nodes connections
        curr_connections.push_back(connection_to_update);
      }
    }

    // Update degree of current node
    node_being_updated->degree = node_being_updated->connections.size();

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

  // Remove self from previous parents children list (if it existed)
  if (parent)
  {
    // Remove this node's edges contribution from parent's
    parent->update_connections_from_node(this_ptr(), true);

    // Remove self from previous children
    parent->remove_child(this_ptr());
  }

  // Set this node's parent
  parent = parent_node_ptr;

  // Add this node's edges to parent's degree count
  parent->update_connections_from_node(this_ptr(), false);

  // Add this node to new parent's children list
  parent_node_ptr->add_child(this_ptr());
}

// =============================================================================f
// Add a node to the children vector
// =============================================================================
inline void Node::add_child(const NodePtr new_child_node)
{
  //PROFILE_FUNCTION();
  // Add new child node to the set of children. An unordered set is used because
  // repeat children can't happen.
  (this_ptr()->children).insert(new_child_node);
}

// =============================================================================
// Find and erase a child node
// =============================================================================
inline void Node::remove_child(const NodePtr child_node)
{
  //PROFILE_FUNCTION();
  children.erase(children.find(child_node));
}

// =============================================================================
// Get parent of current node at a given level
// =============================================================================
inline NodePtr Node::get_parent_at_level(const int level_of_parent)
{
  // First we need to make sure that the requested level is not less than that
  // of the current node.
  if (level_of_parent < level)
  {
    throw "Requested parent level lower than current node level.";
  }

  // Start with this node as current node
  NodePtr current_node = this_ptr();

  while (current_node->level != level_of_parent)
  {
    if (!parent)
      throw "No parent present at requested level";

    // Traverse up parents until we've reached just below where we want to go
    current_node = current_node->parent;
  }

  // Return the final node, aka the parent at desired level
  return current_node;
}

// =============================================================================
// Get all nodes connected to Node at a given level.
// We return a vector because we need random access to elements in this array
// and that isn't provided to us with the list format.
// =============================================================================
std::vector<NodePtr> Node::get_connections_to_level(const int desired_level)
{
  //PROFILE_FUNCTION();
  // Vector to return containing parents at desired level for connections
  std::vector<NodePtr> level_cons;
  level_cons.reserve(connections.size());

  // Go through every child node's connections list, find parent at
  // desired level and place in connected nodes vector
  for (auto connection : connections)
  {
    try
    {
      level_cons.push_back(connection->get_parent_at_level(desired_level));
    }
    catch (...)
    {
      // No connections at this level were found
      throw "No parent at level " +
          std::to_string(desired_level) + " for " + id;
    }
  }

  return level_cons;
}

// =============================================================================
// Collapse a nodes connection to a given level into a map of
// connected group id->count
// =============================================================================
std::map<NodePtr, int> Node::gather_connections_to_level(const int level)
{
  //PROFILE_FUNCTION();
  // Gather all connections from the moved node to the level of the groups we're
  // working with
  std::vector<NodePtr> all_connections = get_connections_to_level(level);

  // Setup an edge count map for node
  std::map<NodePtr, int> connections_counts;

  // Fill out edge count map
  for (auto curr_connection = all_connections.begin();
       curr_connection != all_connections.end();
       ++curr_connection)
  {
    connections_counts[*curr_connection]++;
  }

  return connections_counts;
}

// =============================================================================
// Static method to connect two nodes to each other with edge
// =============================================================================
void Node::connect_nodes(NodePtr node1_ptr, NodePtr node2_ptr)
{
  //PROFILE_FUNCTION();
  // Add node2 to connections of node1
  node1_ptr->add_connection(node2_ptr);
  // Do the same for node2
  node2_ptr->add_connection(node1_ptr);
}
