// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <queue> 
#include "Node.h" 
#include "helpers.cpp"

using namespace Rcpp;
using std::string;
using std::vector;
using std::unordered_set;

// For a bit of clarity
typedef vector<Node*> NodeList;
typedef unordered_set<Node*> ChildSet;

// =======================================================
// Constructor that takes the nodes id and level. Assumes default 0 type. 
// =======================================================
Node::Node(string node_id, int level):
  id(node_id),
  level(level),
  type(0),
  has_parent(false){}

// =======================================================
// Constructor that takes the node's id, level, and type. 
// =======================================================
Node::Node(string node_id, int level, int type):
  id(node_id),
  level(level),
  type(type),
  has_parent(false){}

// =======================================================
// Add connection to another node
// =======================================================
void Node::add_connection(Node* node_ptr) {
  // Add element to connections array
  connections.push_back(node_ptr);
}          

// =======================================================
// Set current node parent/cluster
// =======================================================
void Node::set_parent(Node* parent_node_ptr) {
  // Remove self from previous parents children list (if it existed)
  if(has_parent){
    parent_node_ptr->remove_child(this);
  }
  
  // Set this node's parent
  parent = parent_node_ptr;
  
  // Node for sure has parent now so make sure it's noted
  has_parent = true;
  
  // Add this node to new parent's children list
  parent_node_ptr->add_child(this);
}

// =======================================================
// Add a node to the children vector
// =======================================================
void Node::add_child(Node* new_child_node) {
  // Add new child node to the set of children. An unordered set is used because
  // repeat children cant happen.
  (this->children).insert(new_child_node);
}

// =======================================================
// Find and erase a child node
// =======================================================
void Node::remove_child(Node* child_node) {
  children.erase(children.find(child_node)); 
}

// =======================================================
// Get all member nodes of current node at a given level
// =======================================================
NodeList Node::get_children_at_level(int desired_level) {
  NodeList            children_nodes;
  ChildSet::iterator  child_it;
  Node*               current_node;
  bool                at_desired_level;
  std::queue<Node*>   children_queue;

  // Start by placing the current node into children queue
  children_queue.push(this);
  
  // While the member queue is not empty, pop off a node reference
  while (!children_queue.empty()) {
    // Grab top reference
    current_node = children_queue.front(); 
    
    // Remove reference from queue
    children_queue.pop(); 
    
    // check if that node is at desired level
    at_desired_level = current_node->level == desired_level;
    
    // if node is at desired level, add it to the return vector
    if (at_desired_level) {
      children_nodes.push_back(current_node);
    } else {
      // Otherwise, add each of the member nodes to queue 
      for (child_it = (current_node->children).begin(); child_it != (current_node->children).end(); ++child_it) {
        children_queue.push(*child_it);
      }
    }
    
  } // End queue processing loop
  
  // Return the vector of member nodes
  return children_nodes;
}

// =======================================================
// Get parent of current node at a given level
// =======================================================
Node* Node::get_parent_at_level(int level_of_parent) {
  int    level_delta;    // How many levels up do we need to go?
  Node*  current_node;   // What node are we currently looking at?
  
  level_delta = level_of_parent - level;
  
  // First we need to make sure that the requested level is not less than that
  // of the current node.
  if (level_delta < 0) {
    throw "Requested parent level lower than current node level.";
  }
  
  // Start with this node as current node
  current_node = this;
  
  // Traverse up parents until we've reached just below where we want to go
  for(int i = 0; i < level_delta; i++){
    if(!current_node->has_parent) {
      throw "No parent present at requested level";
    }
    current_node = current_node->parent;
  }
  
  // Return the final node, aka the parent at desired level
  return current_node;
}

// =======================================================
// Get all nodes connected to Node at a given level
// =======================================================
NodeList Node::get_connections_to_level(int desired_level) {
  NodeList            connected_nodes; 
  NodeList            leaf_children;
  NodeList::iterator  child_it;
  NodeList::iterator  connection_it;

  // Start by getting all of the level zero children of this node
  leaf_children = get_children_at_level(0);
  
  // Go through every child
  for (child_it = leaf_children.begin(); child_it != leaf_children.end(); ++child_it) {
    
    // Go through every child node's connections vector
    for (connection_it = (*child_it)->connections.begin(); connection_it != (*child_it)->connections.end(); ++connection_it) {
      
      // For each connection of current child, find parent at desired level and
      // place in connected nodes vector
      connected_nodes.push_back(
        (*connection_it)->get_parent_at_level(desired_level)
      );
      
    } // End child connection loop
  } // End child loop
  
  return connected_nodes;
}


// ======================================================= 
// Get number of edges between and fraction of total for starting node
// =======================================================
connection_info Node::connections_to_node(Node* target_node) {
  connection_info    connections;               // connection info struct we're returning
  NodeList::iterator connections_it;            // For iterating through all connected nodes 
  int                n_connections_to_target;   // How many connection for node are to our target node
  NodeList           all_connections_to_level;  // List of every connection from node to leel of target node
  
  
  // Grab all the nodes connected to node at the level of the target node
  all_connections_to_level = this->get_connections_to_level(target_node->level);
  
  // Make sure that there are actually connections for us to look through
  if (all_connections_to_level.size() == 0) {
    throw "Current node has no connections";
  }
  
  // Start with no connection to target...
  n_connections_to_target = 0;
  
  // Go through all the connections
  for (
      connections_it  = all_connections_to_level.begin();
      connections_it != all_connections_to_level.end();
      ++connections_it
  ) { 
    // If connection at level matches the target node, increment target
    // connection counter
    if (*connections_it == target_node) {
      n_connections_to_target++;
    }
    
  }
  
  connections.n_total = all_connections_to_level.size();
  connections.n_between = n_connections_to_target;

  return connections;
}     



// ======================================================= 
// Probability node transitions to a given group
// =======================================================
double Node::prob_of_joining_group(Node* target_group, NodeList groups_to_check, int total_possible_groups) {
  NodeList::iterator   group_it;                      // For parsing through all groups to check
  Node*                group_being_checked;           // What group are we currently comparing to target      
  connection_info      node_to_checked_connections;   // Connection stats for this node to group we're investigating
  connection_info      checked_to_target_connections; // Connection stats for group we're investigating to potential move group
  double               frac_connections_in_group;     // Proportion of connections of node belonging to investigated group
  double               n_between_checked_target;      // How many connections are there between the investigated group and potential move group
  double               n_total_current;               // How many total connections does the investigated group have
  double               epsilon;                       // Ergodicity tuning parameter
  double               cummulative_prob;              // Varibale to accumulate probabilities over sum
  
  epsilon = 0.01; // This will eventually be passed to function
  cummulative_prob = 0.0; // Start out sum at 0.
  
  // Parse through all groups to check
  for(group_it = groups_to_check.begin(); group_it != groups_to_check.end(); ++group_it){
    group_being_checked = *group_it;
    
    // Make sure we're only looking at groups of type different than node.
    if(group_being_checked->type == type) continue;
    
    // What proportion of this node's edges are to nodes in current group?
    node_to_checked_connections = this->connections_to_node(group_being_checked);
    frac_connections_in_group = double(node_to_checked_connections.n_between) / double(node_to_checked_connections.n_total);
    
    // Grab info on how many edges are there between target group and current
    // and how many total edges does the current group has
    checked_to_target_connections = group_being_checked->connections_to_node(target_group);
    n_between_checked_target = checked_to_target_connections.n_between;
    n_total_current = checked_to_target_connections.n_total;
    
    cummulative_prob += frac_connections_in_group * (n_between_checked_target + epsilon) / (n_total_current + epsilon*(total_possible_groups + 1));
  }
  
  return cummulative_prob;
} 



// =======================================================
// Static method to connect two nodes to each other with edge
// =======================================================
void Node::connect_nodes(Node* node1_ptr, Node* node2_ptr) {
  // Add node2 to connections of node1
  node1_ptr->add_connection(node2_ptr);
  // Do the same for node2
  node2_ptr->add_connection(node1_ptr);
}


// [[Rcpp::export]]
List make_node_and_print( ) {
  Node n1("n1", 0, 1),
       n2("n2", 0, 1),
       n3("n3", 0, 1),
       m1("m1", 0, 2),
       m2("m2", 0, 2),
       m3("m3", 0, 2),
       c1("c1", 1, 1),
       c2("c2", 1, 1),
       d1("d1", 1, 2),
       d2("d2", 1, 2);

  n1.set_parent(&c1);
  n2.set_parent(&c1);
  n3.set_parent(&c2);
  
  m1.set_parent(&d1);
  m2.set_parent(&d2);
  m3.set_parent(&d2);
  
  Node::connect_nodes(&n1, &m1);
  Node::connect_nodes(&n1, &m3);
  Node::connect_nodes(&n2, &m1);
  Node::connect_nodes(&n3, &m2);
  Node::connect_nodes(&n3, &m3);
  

  return List::create(
    _["id"]                  = n1.id,
    _["parent"]              = n1.parent->id,
    _["edges"]               = print_node_ids(n1.connections),
    _["n1 parent"]           = n1.get_parent_at_level(1)->id,
    _["n1 l1 cons"]          = print_node_ids(n1.get_connections_to_level(1)),
    _["c1 l0 cons"]          = print_node_ids(c1.get_connections_to_level(0)),
    _["c1 children"]         = print_node_ids(c1.children),
//    _["frac of n1 to d2"]    = n1.connections_to_node(&d2).frac_of_total,
    _["parent_num_kids"]     = c1.get_children_at_level(0).size()
  );
}



/*** R
make_node_and_print()
*/
