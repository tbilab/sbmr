test_that("computing entropy", {
  # Start with nodes 3 nodes attached to 2 blocks
  net <- sim_random_network(n_nodes = 25, prob_of_edge = 0.5) %>%
    initialize_blocks(10)

  first_entropy <- net %>% get_entropy()

  start_state <- get_state(net)

  # Choose random node
  random_node <- start_state %>%
    dplyr::filter(level == 0) %>%
    dplyr::sample_n(1)

  # Find a _new_ parent for our random node
  random_new_parent <- start_state %>%
    dplyr::filter(level == 1, id != random_node$parent) %>%
    dplyr::sample_n(1)

  # Assign node its new parent
  net <- net %>% set_node_parent(child_id = random_node$id, parent_id = random_new_parent$id)

  # Record entropy again
  second_entropy <- net %>% get_entropy()

  # Entropy should have changed
  expect_false(first_entropy == second_entropy)
})

