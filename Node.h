// Pre-declare class types
class Node;
class Cluster;

class Node {
private: 
  std::vector<Node *> connections;

public:
  Node(int);   
  int               id;
  bool              is_type_a;
  int               degree;
  Cluster *         cluster;
  void              set_edge(Node *);
  std::vector<int>  get_ids_of_connections();
  Node *            random_neighbor();
  static void       join_nodes(Node *, Node *);
};