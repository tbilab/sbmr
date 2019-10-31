// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <random>
#include <queue> 
#include "Node.h" 

using namespace Rcpp;
using std::string;
using std::vector;
using std::map;
using std::unordered_set;


// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
Node::Node(string node_id, int level):
  id(node_id),
  level(level),
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
vector<Node*> Node::get_children_at_level(int desired_level) {
  vector<Node*>                   children_nodes;
  unordered_set<Node*>::iterator  child_it;
  Node*                           current_node;
  bool                            at_desired_level;
  std::queue<Node*>               children_queue;

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
  
  // How many levels up do we need to go?
  int level_delta = level_of_parent - level;
  
  // First we need to make sure that the requested level is not less than that
  // of the current node.
  if (level_delta < 0) {
    throw "Requested parent level lower than current node level.";
  }
  
  // Start with this node as current node
  Node* current_node = this;
  
  // Traverse up parents until we've reached just below where we want to go
  for(int i = 0; i < level_delta; i++){
    current_node = current_node->parent;
  }
  
  // Return the final node, aka the parent at desired level
  return current_node;
}


//vector<Node*> Node::get_all_connections(int desired_level) {
//  
//  vector<Node*> connected_nodes;
//  
//  // Grab all the level 0 (or data) member of this node
//  vector<Node*> member_nodes = this->get_members_at_level(0);
//  
//  
//  std::queue<Node*> members_to_process;
//  std::queue<Node*> connections_to_process; 
//  
//  vector<Node*>::iterator member_iterator;
//  
//  // Start by placing the current node into member queue
//  members_to_process.push(this);
//  
//  // While the member queue is not empty, pop off a node reference
//  // check if that node has member nodes (aka it is a cluster node)
//  // if node has member nodes, add those nodes to the queue
//  // if the node doesn't have member nodes (aka it is a level 0 node)
//  // then send it to the 
//  
//
//  // Start by placing all members of this node into the process queue.
//  for(member_iterator = members.begin(); member_iterator != members.end(); ++ member_iterator){
//    members_to_process.push(*member_iterator);
//  }
//  
//  // Work through the process queue, adding more nodes when nodes have members
//  // If the node looked at is at level 0, aka data, add it to the connections to
//  // process queue
//  
//  
//  // Start processing through the connections from all level-0 children, 
//  // Continuing to add to the queue until we have reached the desired connection
//  // level, then add nodes to the connected_nodes return vector
//  
//  // Return the vector of these connected nodes at the desired level. 
//  
//  
//  return connected_nodes;
//}

// =======================================================
// Build a map of cluster -> # connections for current node
// =======================================================
//void Node::build_counts_to_clusters() {
//  
//  // Start by flushing counts to clusters map incase it was previously set
//  counts_to_clusters.clear();
//  vector<Node*> nodes_to_scan;
//  vector<Node*>::iterator connection_it;
//  Node* connected_cluster;
//  
//  // If Node is a cluster, gather nodes to scan from its members
//  if(is_cluster) {
//    vector<Node*>::iterator member_it; // Iterator for member nodes
//    vector<Node*> member_connections; // Pointer to member's connections vector
//    
//    // First loop over each member of this cluster
//    for(member_it = members.begin(); member_it != members.end(); ++member_it){
//      // Grab pointer to member connections vector. First we de-reference the
//      // pointer to each member, which is itself a pointer to a node. We then
//      // enter the node itself to get its connections with ->
//      member_connections = (*member_it)->connections; 
//      
//      // Loop over the member connections vector
//      for(connection_it = member_connections.begin(); connection_it != member_connections.end(); ++connection_it) {
//        // Convert the current connected node to a real pointer from iterator
//        // and send to the nodes-to-scan vector
//        nodes_to_scan.push_back(*connection_it);
//      }
//    } // end member loop
//  } else {
//    // If the current node is just a normal node we don't need to run into its
//    // members before looping over connections
//    for(connection_it = connections.begin(); connection_it != connections.end(); ++connection_it) {
//      nodes_to_scan.push_back(*connection_it);
//    }
//  } // end else statement
//  
//  // Finally, loop over all the connections and record the cluster membership
//  for(connection_it = nodes_to_scan.begin(); connection_it != nodes_to_scan.end(); ++connection_it) {
//    connected_cluster = (*connection_it)->cluster;
//    counts_to_clusters[connected_cluster] += 1;
//  }
//  
//}


// =======================================================
// Print cluster connections, for debugging
// =======================================================
//string Node::print_counts_to_clusters(){
//  string all_connections;
//  map<Node*, int>::iterator connection_it;
//  string cluster_id;
//  int connection_count;
//  
//  for(connection_it = counts_to_clusters.begin(); connection_it != counts_to_clusters.end(); ++connection_it) {
//    cluster_id = (connection_it->first)->id;
//    connection_count = connection_it->second;
//    all_connections.append(cluster_id + ":" + std::to_string(connection_count) + ",");
//  }
//  
//  return all_connections;
//}   

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
  Node n1("n1", 0),
       n2("n2", 0),
       n3("n3", 0),
       c1("c1", 1),
       c2("c2", 1),
       c11("c11", 2);
  
  n1.set_parent(&c1);
  n2.set_parent(&c1);
  n3.set_parent(&c2);
  c1.set_parent(&c11);
  
  Node::connect_nodes(&n1, &n2);
  Node::connect_nodes(&n1, &n3);
  
//  n1.build_counts_to_clusters();
  
  return List::create(
    _["id"]                  = n1.id,
    _["parent"]              = n1.parent->id,
    _["num_edges"]           = n1.connections.size(),
    _["n1 parent"]           = n1.get_parent_at_level(1)->id,
    _["n1 parent*2"]         = n1.get_parent_at_level(2)->id,
    _["c1 children"]         = c1.children.size(),
    _["parent_num_kids"]     = c1.get_children_at_level(0).size()
  );
}



/*** R
make_node_and_print()
*/
