#pragma once

#include "Node.h"
#include "vector_helpers.h"

void swap_blocks(Node* child_node,
                 Node* new_block,
                 std::vector<NodeUPtr>& block_holder,
                 const bool remove_empty = true)
{
  Node* old_block          = child_node->get_parent();
  const bool has_old_block = old_block != nullptr;

  child_node->set_parent(new_block);

  // If the old block is now empty and we're removing empty blocks, delete it
  if (has_old_block && remove_empty && old_block->no_children()) {

    const bool delete_successful = delete_from_vector(block_holder, old_block);

    if (!delete_successful)
      LOGIC_ERROR("Tried to delete a node that doesn't exist");
  }
}
