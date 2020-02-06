test_that("Correct edge counts returned", {

  net <- sim_basic_block_network(n_blocks = 5) %>%
    initialize_blocks(5)

  total_num_edges <- attr(net, 'n_edges')

  # Get ids of all the nodes and blocks generated
  sbm_state <- get_state(net)
  node_ids <- sbm_state %>% dplyr::filter(level == 0) %>% dplyr::pull(id)
  block_ids <- sbm_state %>% dplyr::filter(level == 1) %>% dplyr::pull(id)

  # Loop over all nodes and make sure they are not connected to any nodes they
  # shouldnt be and that they have reasonable edge counts
  node_ids %>% purrr::walk(function(node_id){
    nodes_connections <- net %>% get_node_to_block_edge_counts(node_id)

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
