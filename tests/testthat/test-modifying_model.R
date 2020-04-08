test_that("Add Node", {

  # Start with network with 5 nodes.
  net <- sim_random_network(n_nodes = 5,
                            prob_of_edge = 1,
                            setup_model = TRUE,
                            random_seed = 42)

  expect_equal(attr(net, 'n_nodes'), 5)

  # Add a node to network
  expect_message({
      net <- net %>% add_node('new_node', show_messages = TRUE)
    },
    "new_node node not in network but has no specified type. Defaulting to node",
    fixed = TRUE
  )

  # We should have a single node added
  expect_equal(attr(net, 'n_nodes'), 6)


  # Adding node again will let user know nothing happened
  expect_message({
      net <- net %>% add_node('new_node', show_messages = TRUE)
    },
    "new_node node was already in network. No action taken.",
    fixed = TRUE
  )

  # We should have a no nodes added
  expect_equal(attr(net, 'n_nodes'), 6)

  # Adding an new node with a specified type should work with no messages
  expect_silent({
    net <- net %>% add_node('new_test_node', type = "node", show_messages = TRUE)
  })

  # One more node should be added
  expect_equal(attr(net, 'n_nodes'), 7)

  # If we try and add a node that has a type not in the initially nodes passed, an error should sound
  expect_error({
      net <- net %>% add_node('new_node', type = "test_node", show_messages = TRUE)
    },
    glue::glue(
      "new_node node was already in network with type node.",
      "Replacing with type with test_node would invalidate model state.",
      "Try rebuilding model or using different node id"
    ),
    fixed = TRUE
  )
})

test_that("Add edge", {
  # Start with network with 5 nodes all fully connected
  net <- sim_random_network(n_nodes = 5,
                            prob_of_edge = 1,
                            random_seed = 42)

  start_num_edges <- attr(net, 'n_edges')
  # Gives a message about new nodes on default
  expect_message({
    net <- net %>% add_edge('new_from_node', 'new_to_node')
  })

  # Make sure we have two nodes added to our model
  expect_equal(attr(net, 'n_nodes'), 7)

  # And one edge
  expect_equal(attr(net, 'n_edges'), start_num_edges + 1)

  # Messages can be turned off
  # Gives a message about new nodes on default
  expect_silent({
    net <- net %>% add_edge('new_from_node2', 'new_to_node2', show_messages = FALSE)
  })

  pre_addition_edges <- attr(net, 'n_edges')

  # Adding an edge between two existing nodes works without message
  expect_silent({
    net <- net %>% add_edge('new_from_node2', 'new_to_node2')
  })

  expect_equal(attr(net, 'n_edges'), pre_addition_edges + 1)
})

test_that("Randomly initializing blocks in network", {
  # Default value is 1 block per node
  expect_equal(
    sim_random_network(n_nodes = 5,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks() %>%
      n_blocks(),
    5
  )

  # Can also specify number of blocks
  expect_equal(
    sim_random_network(n_nodes = 15,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks(num_blocks = 3) %>%
      n_blocks(),
    3
  )

  # Can't request more blocks than nodes
  expect_error(
    sim_random_network(n_nodes = 5,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks(num_blocks = 10),
    "Can't initialize 10 blocks as there are only 5 nodes"
  )

  # Can't request negative numbers of blocks
  expect_error(
    sim_random_network(n_nodes = 5,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks(num_blocks = -2),
    "Can't initialize -2 blocks."
  )

  # Can build blocks at multiple levels
  net <- sim_random_network(n_nodes = 5,
                            prob_of_edge = 1,
                            random_seed = 42) %>%
    initialize_blocks() %>%
    initialize_blocks()

  model_state <- get_state(net)

  # There should now be 10 total nodes in state
  expect_equal(nrow(model_state), 10)

  # With five total blocks sitting at level 1
  expect_equal(
    sum(model_state$level == 1),
    5
  )
})

