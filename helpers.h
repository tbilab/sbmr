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


// ============================================================================
// Builds an alphabetical pair of node ids for looking up edges between groups.
// Insures order doesn't matter for EdgeCount key pair 
// ============================================================================
// std::pair<string, string> find_edges(string, string);
std::pair<NodePtr, NodePtr> find_edges(NodePtr, NodePtr);

// Single values return a double pair. e.g. find_edges('a') -> ['a','a']
// std::pair<string, string> find_edges(string);
// std::pair<string, string> find_edges(NodePtr);


// ============================================================================
// Normalizes a vector of doubles so all elements sum to 1 
// ============================================================================
std::vector<double> normalize_vector(std::vector<double> const &vec);


// ============================================================================
// Compute's factorial of number by recursion
// ============================================================================
int factorial(int);

double log_factorial(int N);

// Gets average of the last n elements for a paseed vector of integers
inline float avg_last_n(std::vector<int> vec, int n);