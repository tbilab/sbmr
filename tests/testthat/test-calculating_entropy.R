test_that("computing entropy", {
  # Start with nodes 3 nodes attached to 2 blocks
  net <- sim_random_network(n_nodes = 25,
                            prob_of_edge = 0.5,
                            random_seed = 42) %>%
    initialize_blocks(10)

  first_entropy <- net %>% entropy()

  start_state <- state(net)

  # Choose random node
  random_node <- start_state %>%
    dplyr::filter(level == 0) %>%
    dplyr::sample_n(1)

  # Find a _new_ parent for our random node
  random_new_parent <- start_state %>%
    dplyr::filter(parent != random_node$parent) %>%
    dplyr::sample_n(1) %>%
    dplyr::pull(parent)

  # Assign node its new parent by modifying state
  net <- net %>%
    update_state(dplyr::mutate(start_state,
                              parent = ifelse(id == random_node$id, random_new_parent, parent)))

  # Record entropy again
  second_entropy <- net %>% entropy()

  # Entropy should have changed
  expect_false(first_entropy == second_entropy)
})

