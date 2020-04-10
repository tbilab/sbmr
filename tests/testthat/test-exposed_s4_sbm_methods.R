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
  expect_equal(sbm$n_levels(), 1)

  # Build a level of blocks
  sbm$initialize_blocks(-1)

  # Now we have two levels (node + block)
  expect_equal(sbm$n_levels(), 2)

  # Initializing another level will stack on top
  sbm$initialize_blocks(3)
  expect_equal(sbm$n_levels(), 3)
})

test_that("Counting and adding nodes", {
  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # To start with we should have 7 nodes a1,a2,a3,b1,b2,b3,b4
  expect_equal(sbm$n_nodes_at_level(0), 7)

  # Can't get counts for block level because we don't have it
  expect_error(sbm$n_nodes_at_level(1), "Can't access level 1. Network only has 0 block levels.", fixed = TRUE)
  sbm$initialize_blocks(-1)
  expect_equal(sbm$n_nodes_at_level(1), sbm$n_nodes_at_level(0))

  # Cant get counts for negative levels
  expect_error(sbm$n_nodes_at_level(-1), "Node levels must be positive. Requested level: -1", fixed = TRUE)

  # Can't add a node to network with existing blocks
  expect_error(
    sbm$add_node("test_node", "node", level = 0),
    "Can't add a node to a network with block structure. This invalidates the model state. Remove block structure with reset_blocks() method.",
    fixed = TRUE
  )

  sbm$reset_blocks()
  sbm$add_node("test_node", "node", level = 0)

  # Hopefully we now have an additional block at our lowest level
  expect_equal(sbm$n_nodes_at_level(0), 8)

  # Cant add node twice
  expect_error(
    sbm$add_node("test_node", "node", level = 0),
    "Network already has a node with id test_node",
    fixed = TRUE
  )
})

test_that("Initializing blocks", {
  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # Requesting -1 blocks will get you one block per node
  sbm$initialize_blocks(-1)
  expect_equal(sbm$n_nodes_at_level(1), 7)

  # Initializing blocks again will make metablocks
  expect_equal(sbm$n_levels(), 2)
  sbm$initialize_blocks(-1)
  expect_equal(sbm$n_levels(), 3)
  expect_equal(sbm$n_nodes_at_level(2), 7)

  # We can reset all block structure
  sbm$reset_blocks()
  expect_equal(sbm$n_levels(), 1)

  # We can request a specific number of blocks to be built for our nodes
  sbm$initialize_blocks(3)
  expect_equal(sbm$n_nodes_at_level(1), 3)
})

test_that("State loading and saving works", {
  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # Start with a random state of three nodes
  sbm$initialize_blocks(3)
  three_block_state <- sbm$get_state()

  # Reset the state and make a random state with every block in its own node
  sbm$reset_blocks()
  sbm$initialize_blocks(-1)

  # Make sure the three block state does not equal the latest state
  expect_false(
    isTRUE(dplyr::all_equal(sbm$get_state(), three_block_state))
  )

  # Now reset the state to the three block state
  sbm$update_state(three_block_state$id,
                   three_block_state$type,
                   three_block_state$parent,
                   three_block_state$level)

  # Now the state of the network should match the old three block state
  expect_true(
    isTRUE(dplyr::all_equal(sbm$get_state(), three_block_state))
  )

})

test_that("Getting model entopy", {
  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # Can't calculate entropy when no blocks are initialized
  expect_error(sbm$entropy(0),
               "Can't calculate entropy because there is no block structure for nodes",
               fixed = TRUE)

  sbm$initialize_blocks(2)

  entropy <- sbm$entropy(0)
  expect_true(!is.null(entropy))
  expect_true(!is.na(entropy))
})

test_that("Interblock edge counts", {

  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # Give each node their own block
  sbm$initialize_blocks(-1)

  sbm_state <- sbm$get_state()

  # Note blocks for a1 and b1
  a1_block <- sbm_state$parent[sbm_state$id == "a1"]
  b1_block <- sbm_state$parent[sbm_state$id == "b1"]

  # Get the interblock edge counts
  block_edge_counts <- sbm$interblock_edge_counts(1)

  # Get number of edges between a1 and a2s blocks
  get_a1_to_b1_block_counts <- . %>%
    dplyr::filter(block_a == a1_block & block_b == b1_block |
                  block_a == b1_block & block_b == a1_block  ) %>%
    dplyr::pull("n_edges")

  original_count <- get_a1_to_b1_block_counts(block_edge_counts)

  # Attempting to add an edge to the network with blocks will give an error.
  expect_error(sbm$add_edge("a1", "b1"),
               "Block structure present in network. Adding an edge invalidates the model state. Remove block structure with reset_blocks() method.",
               fixed = TRUE)

  # We can follow the instructions and proceed again
  # Add an edge between a1 and b1 to network
  sbm$reset_blocks()
  sbm$add_edge("a1", "b1")
  sbm$initialize_blocks(-1)
  a1_block <- sbm_state$parent[sbm_state$id == "a1"]
  b1_block <- sbm_state$parent[sbm_state$id == "b1"]

  # Recheck the block edge counts, the value should have incremented up by 1
  expect_equal(get_a1_to_b1_block_counts(sbm$interblock_edge_counts(1)),
               original_count + 1)

})

test_that("Node to block edge counts", {

  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node) %>%
    attr("model")

  # Give each node their own block
  sbm$initialize_blocks(-1)

  # Get the interblock edge counts
  # When every block has its own node, each node's results have as many rows as that node has degrees
  expect_equal(nrow(sbm$node_to_block_edge_counts("a1", 1)), 3)
  expect_equal(nrow(sbm$node_to_block_edge_counts("a2", 1)), 2)
  expect_equal(nrow(sbm$node_to_block_edge_counts("a3", 1)), 1)

  expect_equal(nrow(sbm$node_to_block_edge_counts("b1", 1)), 3)
  expect_equal(nrow(sbm$node_to_block_edge_counts("b2", 1)), 1)
  expect_equal(nrow(sbm$node_to_block_edge_counts("b3", 1)), 1)
  expect_equal(nrow(sbm$node_to_block_edge_counts("b4", 1)), 1)

})

test_that("MCMC sweeps, no pair tracking", {

  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node, bipartite_edges = TRUE) %>%
    attr("model")

  # Give each node their own block
  sbm$initialize_blocks(-1)

  n_sweeps <- 100

  sweeps <- sbm$mcmc_sweep(n_sweeps, # n_sweeps
                           0.01,     # eps
                           TRUE,     # variable_n_blocks
                           FALSE,    # track_pairs
                           0,        # level
                           FALSE)    # verbose

  expect_equal(nrow(sweeps$sweep_info), n_sweeps)
  expect_null(sweeps$pairing_counts)
})

test_that("MCMC sweeps, w/ pair tracking", {

  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node, bipartite_edges = TRUE) %>%
    attr("model")

  # Give each node their own block
  sbm$initialize_blocks(-1)

  n_sweeps <- 100

  sweeps <- sbm$mcmc_sweep(n_sweeps, # num_sweeps
                           0.01,     # eps
                           TRUE,     # variable_n_blocks
                           TRUE,     # track_pairs
                           0,        # level
                           FALSE)    # verbose

  expect_equal(nrow(sweeps$sweep_info), n_sweeps)
  expect_equal(nrow(sweeps$pairing_counts), (4*3/2) + (3*2/2))
})

test_that("Agglomerative merging", {

  sbm <- new_sbm_network(edges, edges_from_column = a_node, edges_to_column = b_node, bipartite_edges = TRUE) %>%
    attr("model")

  final_n_blocks <- 2
  # Run block collapsing with one block per merge
  collapse_results <- sbm$collapse_blocks(0,    # node_level,
                                          final_n_blocks,    # B_end,
                                          3,    # n_checks_per_block,
                                          0,    # n_mcmc_sweeps,
                                          0.9,  # sigma,
                                          0.01, # eps,
                                          TRUE, # report_all_steps = true,
                                          TRUE) # allow_exhaustive = true

  # Reporting all steps should, return as many list elements as there are possible steps
  expect_equal(length(collapse_results$step_states),
               sbm$n_nodes_at_level(0) - final_n_blocks)

  just_final <- sbm$collapse_blocks(0,                # node_level,
                                    final_n_blocks, # B_end,
                                    3,                # n_checks_per_block,
                                    0,                # n_mcmc_sweeps,
                                    0.9,              # sigma,
                                    0.01,             # eps,
                                    FALSE,            # report_all_steps = true,
                                    TRUE)             # allow_exhaustive = true

  # Just reporting the final step should... just report the final step
  expect_equal(length(just_final$step_states),1)
})

