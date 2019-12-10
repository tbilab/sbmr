requireNamespace("lobstr", quietly = TRUE)

test_that("Assignment and mutation keep pointing to same object", {

  # Start with network with 5 nodes.
  my_sbm <- create_sbm() %>%
    add_node('node_1') %>%
    add_node('node_2')

  expect_equal(nrow(get_state(my_sbm)), 2)

  # Add a new node and assign returned SBM object to new variable
  my_sbm2 <- my_sbm %>%
    add_node('node_3')

  expect_equal(nrow(get_state(my_sbm2)), 3)
  expect_equal(nrow(get_state(my_sbm)), 3)

  # Make sure we're looking at the exact same object to avoid painful copy costs
  expect_equal(lobstr::obj_addr(my_sbm), lobstr::obj_addr(my_sbm2))
 })

test_that("Add Node", {
  # Start with network with 5 nodes.
  my_sbm <- create_sbm(edges = sim_simple_network(n_nodes = 5, prob_of_connection = 1))
  expect_equal(my_sbm %>% get_state() %>% nrow(), 5)

  # Add a node using piped in place testing
  my_sbm %>% add_node('node_6', type = 'test_node')
  expect_equal(my_sbm %>% get_state() %>% nrow(), 6)

  # Add node using piped and assigned formula
  my_sbm2 <- my_sbm %>% add_node('node_7', type = 'test_node')
  expect_equal(my_sbm2 %>% get_state() %>% nrow(), 7)

  # Add a node at a group level
  my_sbm %>% add_node('node_11', level = 1, type = 'test_node')
  expect_equal(sum(get_state(my_sbm)$level), 1)

  # Add another node at the metagroup level
  my_sbm %>% add_node('node_21', level = 2, type = 'test_node')
  expect_equal(sum(get_state(my_sbm)$level), 3)
})


test_that("Add connection", {
  # Start with network with couple nodes
  my_sbm <- create_sbm() %>%
    add_node('node_1') %>%
    add_node('node_2')

  expect_equal(my_sbm %>% get_state() %>% nrow(), 2)

  # Add connection between those two nodes
  my_sbm %>% add_connection('node_1', 'node_2')

  # This should thrown a nice helpful error message, but it doesn't.
  # my_sbm %>% add_connection('node_4', 'node_2')
})


test_that("Set node parent", {
  # Start with network with couple nodes, one at group level
  my_sbm <- create_sbm() %>%
    add_node('node_1') %>%
    add_node('node_11', level = 1) %>%
    set_node_parent(child_id = 'node_1', parent_id = 'node_11')

  #  Grab parent of node 1
  get_node_1_parent <- . %>%
    get_state() %>%
    dplyr::filter(id == 'node_1') %>%
    purrr::pluck('parent')

  # Make sure that node 1's parent is what it should be
  expect_equal(get_node_1_parent(my_sbm), 'node_11')

  # Add another node and set it as node 1s parent and make sure it works
  my_sbm %>%
    add_node('node_12', level = 1) %>%
    set_node_parent(child_id = 'node_1', parent_id = 'node_12')

  # Make sure that node 1's parent is what it should be
  expect_equal(get_node_1_parent(my_sbm), 'node_12')

  # Try and assign a parent node that is same level as node 1
  # Again, this should throw an error!
  # my_sbm %>%
  #   add_node('node_2') %>%
  #   set_node_parent(child_id = 'node_1', parent_id = 'node_2')
  #
})
