#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>    // std::sort
#include "Node.h"


// =======================================================
// Takes a vector of node ids and returns a single string of them pasted together
// =======================================================
std::string print_ids_to_string(std::vector<std::string>);


// =======================================================
// Grab vector of node ids from a sequential container of nodes
// =======================================================

// =======================================================
// List Version
std::string print_node_ids(std::list<Node*>);

// =======================================================
// Vector Version
std::string print_node_ids(std::vector<Node*>);

// =======================================================
// Map version
std::string print_node_ids(std::map<std::string, Node*>);
