#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>    // std::sort

#include "Node.h"


#include <memory>
typedef std::shared_ptr<Node> NodePtr;


// =======================================================
// Takes a vector of node ids and returns a single string of them pasted together
// =======================================================
std::string print_ids_to_string(std::vector<std::string>);


// =======================================================
// Grab vector of node ids from a sequential container of nodes
// =======================================================

// =======================================================
// List Version
std::string print_node_ids(std::list<NodePtr>);

std::string print_node_ids(std::unordered_set<NodePtr>);

// =======================================================
// Vector Version
std::string print_node_ids(std::vector<NodePtr>);

// =======================================================
// Map version
std::string print_node_ids(std::map<std::string, NodePtr>);


// Helper function that insures order doesn't matter for EdgeCount key pair
std::pair<string, string> find_edges(string, string);
std::pair<string, string> find_edges(NodePtr, NodePtr);
std::pair<string, string> find_edges(string);
std::pair<string, string> find_edges(NodePtr);

std::vector<double> normalize_vector(std::vector<double> const &vec);


int factorial(int);