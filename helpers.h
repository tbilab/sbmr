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
