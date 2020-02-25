#pragma once

#include "../Node.h"

// ============================================================================
// Takes a vector of node ids and returns a string of them pasted together
// ============================================================================
inline std::string print_ids_to_string(std::vector<std::string> node_ids)
{

  // Sort vector of id strings
  std::sort(node_ids.begin(), node_ids.end());

  // Final string that will be filled in
  std::string node_id_string;

  // A rough guess at how big the result will be
  node_id_string.reserve(node_ids.size() * 5);

  // Dump vector of id strings to one big string
  for (const auto& node_id : node_ids) {
    // Append node id to return string.
    node_id_string.append(node_id + ", ");
  }

  // Remove last comma for cleanliness
  node_id_string.erase(node_id_string.end() - 2,
                       node_id_string.end());

  return node_id_string;
}

// ============================================================================
// Grab vector of node ids from a sequential container of nodes
// ============================================================================

// =======================================================
// List Version
inline std::string print_node_ids(NodeList nodes)
{
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (const auto& node : nodes) {
    node_ids.push_back(node->id);
  }

  return print_ids_to_string(node_ids);
}

// Set Version
inline std::string print_node_ids(NodeSet nodes)
{
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (const auto& node : nodes) {
    // Append node id to return string.
    node_ids.push_back(node->id);
  }

  return print_ids_to_string(node_ids);
}

// Vector Version
inline std::string print_node_ids(NodeVec nodes)
{
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (auto node_it = nodes.begin();
       node_it != nodes.end();
       ++node_it) {
    // Append node id to return string.
    node_ids.push_back((*node_it)->id);
  }

  return print_ids_to_string(node_ids);
}

// =======================================================
// Map version
inline std::string print_node_ids(NodeLevel nodes)
{
  // Vector of node ids to be filled in
  std::vector<std::string> node_ids;

  // Add node ids to containing vector
  for (auto node_it = nodes.begin();
       node_it != nodes.end();
       ++node_it) {
    // Append node id to return string.
    node_ids.push_back(node_it->second->id);
  }

  return print_ids_to_string(node_ids);
}
