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
//class  Node;

//=================================
// Main node class declaration
//=================================
class Node {
  public:
    // Node();
    Node(string, bool);  // Constructor function takes ID and if node is cluster
    // ==========================================
    // Attributes
    string              id;          // Unique integer id for node
    vector<Node*>       connections; // Nodes that are connected to this node  
    Node*               cluster;     // Cluster that node belongs to 
    bool                is_cluster;  // Is Node a cluster node
    vector<Node*>       members;     // Nodes that are contained within node (if node is cluster)
    string              type;        // What type of node is this?
    
    // ==========================================
    // Methods   
    void            set_cluster(Node*);           // Set current node cluster
    void            add_connection(Node*);        // Add connection to another node
    void            add_member(Node*);            // Add a node to the members map
    static void     connect_nodes(Node*, Node*);  // Static method to connect two nodes to each other with edge
    //int             num_edges_to_node(Node*);     // Get total edges to another node
    //void            remove_edge(Node*, bool);     // Remove a connection from edge map
    //Node*           get_random_neighbor();        // Find a random neighbor node
    //void            swap_clusters(Node*);         // Swap current cluster with a new one
    //vector<string>  neighbor_clusters();          // Ids of every neighbor's cluster
    
};

#endif