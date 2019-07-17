// [[Rcpp::plugins(cpp11)]]
//=================================
// include guard
//=================================
#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__
using std::string;
using std::vector;
using std::map;


//=================================
// What this file declares
//=================================
class  Node;
struct Edge;

//=================================
// Edge structure for holding pointer to connected 
// node and how many edges between
//=================================
struct Edge {
  Node* node;
  int   count;
};


//=================================
// Main node class declaration
//=================================
class Node {
  public:
    // Node();
    Node(string, bool);   
    // ==========================================
    // Attributes
    string              id;          // Unique integer id for node
    bool                is_type_a;   // Is the node the arbitrary "a" type?
    Node*               cluster;     // What node(cluster) is this node contained in?
    map<string, Node*>  members;     // What nodes are contained within this node?
    map<string, Edge>   edges;       // What nodes is this node connected to and how many times?
    int                 degree;      // How many total connection are there to this node
    
    // ==========================================
    // Methods   
    void            remove_edge(Node*, bool);     // Remove a connection from edge map
    int             num_edges_to_node(Node*);     // How many total edges to another node?
    Node*           get_random_neighbor();        // Find a random neighbor node
    void            add_member(Node*);            // Add a node to the members map
    void            swap_clusters(Node*);         // Swap current cluster with a new one
    vector<string>  neighbor_clusters();          // Ids of every neighbor's cluster
    static void     connect_nodes(Node*, Node*);  // Static method to connect two nodes to each other with edge
    
  private:
    void            add_edge(Node*);              // Add connection to edge map
    
};

#endif