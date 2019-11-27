#include "Node.h"

// =============================================================================
// Replace 'this' with a shared smart pointer
// =============================================================================
NodePtr Node::this_ptr()
{
  return shared_from_this();
}

// =============================================================================
// Add connection to another node
// =============================================================================
void Node::add_connection(NodePtr node)
{
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
void Node::update_connections_from_node(NodePtr node, bool remove)
{
  auto connections_to_remove = node->connections;

  // Starting with this node
  auto current_node = this_ptr();

  while (current_node)
  {
    for (auto connected_node : connections_to_remove)
    {
      if (remove)
      {
        // Scan through this nodes connections untill we find the first instance
        // of the connected node we want to remove
        for (auto con_it = connections.begin();
             con_it != connections.end();
             con_it++)
        {
          if (*con_it == connected_node)
          {
            connections.erase(con_it);
            break;
          }
        }
      }
      else
      {
        // Just add this connection to nodes connections
        (current_node->connections).push_back(connected_node);
      }
    }

    // Update degree of current node
    current_node->degree += (remove ? -1 : 1) * connections_to_remove.size();

    // Update current node to nodes parent
    current_node = current_node->parent;
  }
}

// =============================================================================
// Set current node parent/cluster
// =============================================================================
void Node::set_parent(NodePtr parent_node_ptr)
{
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

// =============================================================================
// Add a node to the children vector
// =============================================================================
void Node::add_child(NodePtr new_child_node)
{
  // Add new child node to the set of children. An unordered set is used because
  // repeat children can't happen.
  (this_ptr()->children).insert(new_child_node);
}

// =============================================================================
// Find and erase a child node
// =============================================================================
void Node::remove_child(NodePtr child_node)
{
  children.erase(children.find(child_node));
}

// =============================================================================
// Get all member nodes of current node at a given level
// =============================================================================
ChildSet Node::get_children_at_level(int desired_level)
{
  // Set to hold all the nodes that are found as children
  ChildSet children_nodes;

  // Start by placing the current node into children queue
  std::queue<NodePtr> children_queue;
  children_queue.push(this_ptr());

  // While the member queue is not empty, pop off a node reference
  while (!children_queue.empty())
  {
    // Grab top reference
    NodePtr current_node = children_queue.front();

    // Remove reference from queue
    children_queue.pop();

    // check if that node is at desired level
    bool at_desired_level = current_node->level == desired_level;

    // if node is at desired level, add it to the return vector
    if (at_desired_level)
    {
      children_nodes.insert(current_node);
    }
    else
    {
      // Otherwise, add each of the member nodes to queue
      for (auto child_it = (current_node->children).begin();
           child_it != (current_node->children).end();
           ++child_it)
      {
        children_queue.push(*child_it);
      }
    }

  } // End queue processing loop

  // Return the vector of member nodes
  return children_nodes;
}

// =============================================================================
// Get parent of current node at a given level
// =============================================================================
NodePtr Node::get_parent_at_level(int level_of_parent)
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
std::vector<NodePtr> Node::get_connections_to_level(int desired_level)
{
  // Vector to return containing parents at desired level for connections
  std::vector<NodePtr> level_cons;
  level_cons.reserve(connections.size());

  // Go through every child node's connections list, find parent at
  // desired level and place in connected nodes vector
  for (auto connection : connections)
  {
    level_cons.push_back(connection->get_parent_at_level(desired_level));
  }

  return level_cons;
}

// =============================================================================
// Collapse a nodes connection to a given level into a map of
// connected group id->count
// =============================================================================
std::map<NodePtr, int> Node::gather_connections_to_level(int level)
{
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
  // Add node2 to connections of node1
  node1_ptr->add_connection(node2_ptr);
  // Do the same for node2
  node2_ptr->add_connection(node1_ptr);
}
