#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>    // std::sort
#include <memory>
#include <numeric>

#include "Node.h"



// ============================================================================
// Takes a vector of node ids and returns a string of them pasted together
// ============================================================================
std::string print_ids_to_string(std::vector<std::string>);


// ============================================================================
// Grab vector of node ids from a sequential container of nodes
// ============================================================================
// List Version
std::string print_node_ids(std::list<NodePtr>);
// Set Version
std::string print_node_ids(std::unordered_set<NodePtr>);
// Vector Version
std::string print_node_ids(std::vector<NodePtr>);
// Map version
std::string print_node_ids(std::map<std::string, NodePtr>);



// Gets average of the last n elements for a paseed vector of integers
inline float avg_last_n(std::vector<int> vec, int n);