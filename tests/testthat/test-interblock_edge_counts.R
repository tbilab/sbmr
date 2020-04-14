test_that("Correct edge counts returned", {

  net <- sim_basic_block_network(n_blocks = 5, random_seed = 42) %>%
    initialize_blocks(5)

  total_num_edges <- attr(net, 'n_edges')

  # Get ids of all the nodes and blocks generated
  sbm_state <- state(net)
  node_ids <- sbm_state$id
  block_ids <- sbm_state$parent %>% unique()

  block_to_block_edges <- net %>% interblock_edge_counts()

  # Make sure we have proper block ids
  expect_true(all(block_ids %in% block_to_block_edges$block_a))
  expect_true(all(block_ids %in% block_to_block_edges$block_b))

  # Make sure the size is as expected
  expect_true(nrow(block_to_block_edges) <= 5*4/2 + 5)

  # Every edge should be accounted for
  expect_equal(total_num_edges,
               sum(block_to_block_edges$n_edges))
})
