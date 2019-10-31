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
    Node(string, bool, int);  // Constructor function takes ID and if node is cluster
    // ==========================================
    // Attributes
    string              id;          // Unique integer id for node
    vector<Node*>       connections; // Nodes that are connected to this node  
    Node*               cluster;     // Cluster that node belongs to
    int                 level;       // What level does this node sit at (0 = data, 1 = cluster, 2 = super-clusters, ...)
    bool                is_cluster;  // Is Node a cluster node
    vector<Node*>       members;     // Nodes that are contained within node (if node is cluster)
    string              type;        // What type of node is this?
    map<Node*, int>     counts_to_clusters; // Pointer keyed map to number of connections to clusters
    
    // ==========================================
    // Methods   
    void            set_cluster(Node*);           // Set current node cluster
    void            add_connection(Node*);        // Add connection to another node
    void            add_member(Node*);            // Add a node to the members map
    vector<Node*>   get_members_at_level(int);    // Get all member nodes of current node at a given level
    vector<Node*>   get_all_connections(int);     // Get all nodes connected to Node at a given level
    void            build_counts_to_clusters();   // Get map of counts to connected clusters
    string          print_counts_to_clusters();   // Print cluster counts for debugging
    static void     connect_nodes(Node*, Node*);  // Static method to connect two nodes to each other with edge
  
};

#endif