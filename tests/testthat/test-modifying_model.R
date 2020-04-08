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


test_that("Set node parent", {
  # Start with bipartite network with 6 nodes
  net <- dplyr::tribble(
      ~a_node, ~b_node,
      "a1"   , "b1"   ,
      "a1"   , "b2"   ,
      "a1"   , "b3"   ,
      "a2"   , "b1"   ,
      "a3"   , "b1"
    ) %>%
  new_sbm_network(bipartite_edges = TRUE,
                  edges_from_col = a_node,
                  edges_to_col = b_node,
                  random_seed = 42)


  # Make a parent node and assign it to a1
  net <- set_node_parent(net, child_id = 'a1', parent_id = 'a1_parent')

  new_state <- get_state(net)
  # Now we should have a state with 6 total nodes...
  expect_equal(nrow(new_state), 7)

  # One of which has the id of node_1_parent...
  index_of_new_parent <- which(new_state$id == 'a1_parent')

  # Check new parent exists
  expect_equal(length(index_of_new_parent), 1)

  # Make sure it has the proper level
  expect_equal(new_state$level[index_of_new_parent], 1)

  # Make sure it has the proper type
  expect_equal(new_state$type[index_of_new_parent], 'a_node')

  # The node a1 should have a parent of a1_parent
  expect_equal(new_state$parent[new_state$id == "a1"], 'a1_parent')

  # Now we can add a parent to the parent
  net <- set_node_parent(net, child_id = 'a1_parent', parent_id = 'a1_grandparent')

  new_new_state <- get_state(net)
  index_of_new_grandparent <- which(new_new_state$id == 'a1_grandparent')
  # Make sure grandparent has proper level
  expect_equal(new_new_state$level[index_of_new_grandparent], 2)
})


test_that("Randomly initializing blocks in network", {
  # Default value is 1 block per node
  expect_equal(
    sim_random_network(n_nodes = 5,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks() %>%
      get_num_blocks(),
    5
  )

  # Can also specify number of blocks
  expect_equal(
    sim_random_network(n_nodes = 15,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks(num_blocks = 3) %>%
      get_num_blocks(),
    3
  )

  # Can't request more blocks than nodes
  expect_error(
    sim_random_network(n_nodes = 5,
                       prob_of_edge = 1,
                       random_seed = 42) %>%
      initialize_blocks(num_blocks = 10),
    "Network only has 5 nodes at level 0. Can't initialize 10 blocks"
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

