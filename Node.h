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
struct Edge_Search;

//=================================
// Edge structure for holding pointer to connected 
// node and how many edges between
//=================================
struct Edge {
  Node* node;
  int   count;
};

struct Edge_Search {
  Edge edge;
  bool exists; 
};

//=================================
// Main node class declaration
//=================================
class Node {
  private:
    Edge_Search      find_edge(Node*); 
  
  public:
    Node(int, bool);   
    // ==========================================
    // Attributes
    int              id;          // Unique integer id for node
    bool             is_type_a;   // Is the node the arbitrary "a" type?
    Node*            cluster;     // What node(cluster) is this node contained in?
    map<int, Node*>  members;     // What nodes are contained within this node?
    map<int, Edge>   edges;       // What nodes is this node connected to and how many times?
    int              degree;      // How many total connection are there to this node
    
    // ==========================================
    // Methods
    void          add_edge(Node*);          // Add connection to edge map
    void          remove_edge(Node*, bool); // Remove a connection from edge map
    int           num_edges_to_node(Node*); // How many total edges to another node?
    Node*         get_random_neighbor();    // Find a random neighbor node
    void          add_member(Node*);        // Add a node to the members map
    void          swap_clusters(Node*);     // Swap current cluster with a new one
    vector<Edge>  num_edges_to_clusters();  // Get how many edges to all represented neighbor clusters
};

#endif