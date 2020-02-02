test_that("Correct edge counts returned", {
  simulated_network <- sim_basic_block_network(n_blocks = 5)
  sbm <- simulated_network %>%
    create_sbm() %>%
    initialize_blocks(5)

  total_num_edges <- nrow(simulated_network$edges)

  # Get ids of all the nodes generated
  sbm_state <- get_state(sbm)
  node_ids <- sbm_state$id[sbm_state$level == 0]

  # And get ids of all the blocks generated
  block_ids <- sbm_state$id[sbm_state$level == 1]

  # Loop over all nodes and make sure they are not connected to any nodes they
  # shouldnt be and that they have reasonable edge counts
  node_ids %>% purrr::walk(function(node_id){
    nodes_connections <- sbm %>% get_node_edge_counts_at_level(node_id)

    expect_true(
      all(nodes_connections$id %in% block_ids)
    )

    expect_true(
      all(nodes_connections$counts >= 0)
    )

    expect_true(
      all(nodes_connections$counts <= total_num_edges)
    )
  })
})
