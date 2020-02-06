test_that("Add Node", {

  # Start with network with 5 nodes.
  net <- sim_random_network(n_nodes = 5, prob_of_edge = 1, setup_model = TRUE)

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
    net <- net %>% add_node('new_test_node', type = "test_node", show_messages = TRUE)
  })

  # One more node should be added
  expect_equal(attr(net, 'n_nodes'), 7)

  # If we try and add a node that has the same name as a previous node but a
  # different type, a error should sound
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
  net <- sim_random_network(n_nodes = 5, prob_of_edge = 1)

  start_num_edges <- attr(net, 'n_edges')
  net <- net %>% add_edge('new_from_node', 'new_to_node')

  # Make sure we have two nodes added to our model
  expect_equal(attr(net, 'n_nodes'), 7)

  # And one edge
  expect_equal(attr(net, 'n_edges'), start_num_edges + 1)
})


#
# test_that("Set node parent", {
#   # Start with network with couple nodes, one at block level
#   my_sbm <- create_sbm() %>%
#     add_node('node_1') %>%
#     add_node('node_11', level = 1) %>%
#     set_node_parent(child_id = 'node_1', parent_id = 'node_11')
#
#   #  Grab parent of node 1
#   get_node_1_parent <- . %>%
#     get_state() %>%
#     dplyr::filter(id == 'node_1') %>%
#     purrr::pluck('parent')
#
#   # Make sure that node 1's parent is what it should be
#   expect_equal(get_node_1_parent(my_sbm), 'node_11')
#
#   # Add another node and set it as node 1s parent and make sure it works
#   my_sbm %>%
#     add_node('node_12', level = 1) %>%
#     set_node_parent(child_id = 'node_1', parent_id = 'node_12')
#
#   # Make sure that node 1's parent is what it should be
#   expect_equal(get_node_1_parent(my_sbm), 'node_12')
#
#   # Try and assign a parent node that is same level as node 1
#   # Again, this should throw an error!
#   expect_error(
#     my_sbm %>%
#       add_node('node_2') %>%
#       set_node_parent(child_id = 'node_1', parent_id = 'node_2'),
#     "Can't find node node_2 at level 1",
#     fixed = TRUE
#   )
# })
#
#
# test_that("Initializing a single block per node", {
#   # Default parameters should create a single block per node
#   my_sbm <- create_sbm() %>%
#     add_node('node_1') %>%
#     add_node('node_2') %>%
#     add_node('node_3') %>%
#     initialize_blocks()
#
#   expect_equal(get_num_blocks(my_sbm),3)
#
# })
#
#
# test_that("Randomly assigning initial blocks", {
#   n_samples <- 10 # due to stochastic sampling need to try a few times and make sure average works as expected
#   n_blocks <- 3;
#   n_nodes_each_type <- 10;
#   max_n_types <- 5
#
#   get_num_initialized_blocks <- function(my_nodes){
#     # Default parameters should create a single block per node
#     create_sbm(nodes = my_nodes) %>%
#       initialize_blocks(num_blocks = n_blocks) %>%
#       get_state() %>%
#       dplyr::filter(level == 1) %>%
#       nrow()
#   }
#
#   # Loop over a range of number of types and make sure desired number of blocks is made
#   1:max_n_types %>%
#     purrr::walk(function(n_types){
#       types <- letters[1:n_types]
#
#       # Setup some nodes
#       my_nodes <- dplyr::tibble(
#         type = rep(types, each = n_nodes_each_type),
#         id = paste0(type, 1:(n_nodes_each_type*n_types))
#       )
#
#       num_blocks_per_sample <- 1:n_samples %>%
#         purrr::map_int(~get_num_initialized_blocks(my_nodes))
#
#       expect_true(any(num_blocks_per_sample == n_blocks*n_types))
#     })
# })
#
#
# test_that("Loading from state dump", {
#
#   # Start with nodes 3 nodes attached to 2 blocks
#   my_sbm <- create_sbm() %>%
#     add_node('node_1') %>%
#     add_node('node_2') %>%
#     add_node('node_3') %>%
#     add_node('node_11', level = 1) %>%
#     add_node('node_12', level = 1) %>%
#     set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
#     set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
#     set_node_parent(child_id = 'node_3', parent_id = 'node_12')
#
#   # Grab state
#   initial_state <- my_sbm %>% get_state()
#
#   # Modify state by adding a new block and attaching node 3 to it
#   my_sbm %>%
#     add_node('node_13', level = 1) %>%
#     set_node_parent('node_3', parent_id = 'node_13')
#
#   intermediate_state <- my_sbm %>% get_state()
#
#   # Make sure initial and intermediate are not equal
#   testthat::expect_false(isTRUE(all.equal(intermediate_state, initial_state)))
#
#   # Now restore model state using initial snapshot
#   my_sbm %>% load_from_state(initial_state)
#   final_state <- my_sbm %>% get_state()
#
#   expect_equal(initial_state, final_state)
# })
#
#
# test_that("computing entropy", {
#   # Start with nodes 3 nodes attached to 2 blocks
#   my_sbm <- create_sbm() %>%
#     add_node('node_1') %>%
#     add_node('node_2') %>%
#     add_node('node_3') %>%
#     add_node('node_4') %>%
#     add_node('node_11', level = 1) %>%
#     add_node('node_12', level = 1) %>%
#     add_node('node_13', level = 1) %>%
#     add_edge('node_1', 'node_2') %>%
#     add_edge('node_1', 'node_3') %>%
#     add_edge('node_1', 'node_4') %>%
#     add_edge('node_2', 'node_3') %>%
#     add_edge('node_2', 'node_4') %>%
#     add_edge('node_3', 'node_4') %>%
#     add_edge('node_4', 'node_1') %>%
#     set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
#     set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
#     set_node_parent(child_id = 'node_3', parent_id = 'node_12') %>%
#     set_node_parent(child_id = 'node_4', parent_id = 'node_13')
#
#   first_entropy <- my_sbm %>% get_entropy()
#
#   # Change parentage of a node
#   my_sbm %>% set_node_parent(child_id = 'node_2', parent_id = 'node_12')
#
#   # Record entropy again
#   second_entropy <- my_sbm %>% get_entropy()
#
#   # Entropy should have changed
#   expect_false(first_entropy == second_entropy)
# })
#
