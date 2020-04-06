# Build small object from simple edge dataframe
edges <- dplyr::tribble(
  ~a_node, ~b_node,
  "a1"   , "b1"   ,
  "a1"   , "b2"   ,
  "a1"   , "b3"   ,
  "a2"   , "b1"   ,
  "a2"   , "b4"   ,
  "a3"   , "b1"
)

test_that("Level counting and initialization", {
  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # No block structure to start
  expect_equal(sbm$num_levels(), 1)

  # Build a level of blocks
  sbm$initialize_blocks(-1)

  # Now we have two levels (node + block)
  expect_equal(sbm$num_levels(), 2)

  # Initializing another level will stack on top
  sbm$initialize_blocks(3)
  expect_equal(sbm$num_levels(), 3)
})


test_that("Counting and adding nodes", {
  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # To start with we should have 7 nodes a1,a2,a3,b1,b2,b3,b4
  expect_equal(sbm$num_nodes_at_level(0), 7)

  # Can't get counts for block level because we don't have it
  expect_error(sbm$num_nodes_at_level(1), "Can't access level 1. Network only has 0 block levels.", fixed = TRUE)
  sbm$initialize_blocks(-1)
  expect_equal(sbm$num_nodes_at_level(1), sbm$num_nodes_at_level(0))

  # Cant get counts for negative levels
  expect_error(sbm$num_nodes_at_level(-1), "Node levels must be positive. Requested level: -1", fixed = TRUE)

  # Can't add a node to network with existing blocks
  expect_error(
    sbm$add_node("test_node", "node", level = 0),
    "Can't add a node to a network with block structure. This invalidates the model state. Remove block structure with reset_blocks() method.",
    fixed = TRUE
  )

  sbm$reset_blocks()
  sbm$add_node("test_node", "node", level = 0)

  # Hopefully we now have an additional block at our lowest level
  expect_equal(sbm$num_nodes_at_level(0), 8)
})
