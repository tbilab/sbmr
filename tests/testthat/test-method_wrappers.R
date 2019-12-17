requireNamespace("lobstr", quietly = TRUE)

# Helper to get number of groups from a given state
get_num_groups <- function(sbm){
  sbm %>%
    get_state() %>%
    dplyr::filter(level == 1) %>%
    nrow()
}

test_that("Get state returns the actual state", {
  my_nodes <- dplyr::tribble(
    ~id, ~type,
    "a1", "a",
    "a2", "a",
    "b1", "b",
    "b2", "b"
  )

  expected_state <- my_nodes %>%
    dplyr::mutate(
      level = 0L,
      parent = 'none'
    )

  my_sbm <- create_sbm(nodes = my_nodes)
  expect_equal(dplyr::as_tibble(get_state(my_sbm)), expected_state)
})


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
  my_sbm <- create_sbm(edges = sim_random_network(n_nodes = 5, prob_of_connection = 1))
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
  expect_error(
    add_connection(my_sbm, 'node_4', 'node_2'),
    "Can't find node node_4 at level 0",
    fixed = TRUE
  )
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
  expect_error(
    my_sbm %>%
      add_node('node_2') %>%
      set_node_parent(child_id = 'node_1', parent_id = 'node_2'),
    "Can't find node node_2 at level 1",
    fixed = TRUE
  )
})


test_that("Initializing a single group per node", {
  # Default parameters should create a single group per node
  my_sbm <- create_sbm() %>%
    add_node('node_1') %>%
    add_node('node_2') %>%
    add_node('node_3') %>%
    initialize_groups()

  expect_equal(get_num_groups(my_sbm),3)

})


test_that("Randomly assigning initial groups", {
  n_samples <- 10 # due to stochastic sampling need to try a few times and make sure average works as expected
  n_groups <- 3;
  n_nodes_each_type <- 10;
  max_n_types <- 5

  get_num_initialized_groups <- function(my_nodes){
    # Default parameters should create a single group per node
    create_sbm(nodes = my_nodes) %>%
      initialize_groups(num_groups = n_groups) %>%
      get_state() %>%
      dplyr::filter(level == 1) %>%
      nrow()
  }

  # Loop over a range of number of types and make sure desired number of groups is made
  1:max_n_types %>%
    purrr::walk(function(n_types){
      types <- letters[1:n_types]

      # Setup some nodes
      my_nodes <- dplyr::tibble(
        type = rep(types, each = n_nodes_each_type),
        id = paste0(type, 1:(n_nodes_each_type*n_types))
      )

      num_groups_per_sample <- 1:n_samples %>%
        purrr::map_int(~get_num_initialized_groups(my_nodes))

      expect_true(any(num_groups_per_sample == n_groups*n_types))
    })
})


test_that("Loading from state dump", {

  # Start with nodes 3 nodes attached to 2 groups
  my_sbm <- create_sbm() %>%
    add_node('node_1') %>%
    add_node('node_2') %>%
    add_node('node_3') %>%
    add_node('node_11', level = 1) %>%
    add_node('node_12', level = 1) %>%
    set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
    set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
    set_node_parent(child_id = 'node_3', parent_id = 'node_12')

  # Grab state
  initial_state <- my_sbm %>% get_state()

  # Modify state by adding a new group and attaching node 3 to it
  my_sbm %>%
    add_node('node_13', level = 1) %>%
    set_node_parent('node_3', parent_id = 'node_13')

  intermediate_state <- my_sbm %>% get_state()

  # Make sure initial and intermediate are not equal
  testthat::expect_false(isTRUE(all.equal(intermediate_state, initial_state)))

  # Now restore model state using initial snapshot
  my_sbm %>% load_from_state(initial_state)
  final_state <- my_sbm %>% get_state()

  expect_equal(initial_state, final_state)
})


test_that("computing entropy", {
  # Start with nodes 3 nodes attached to 2 groups
  my_sbm <- create_sbm() %>%
    add_node('node_1') %>%
    add_node('node_2') %>%
    add_node('node_3') %>%
    add_node('node_4') %>%
    add_node('node_11', level = 1) %>%
    add_node('node_12', level = 1) %>%
    add_node('node_13', level = 1) %>%
    add_connection('node_1', 'node_2') %>%
    add_connection('node_1', 'node_3') %>%
    add_connection('node_1', 'node_4') %>%
    add_connection('node_2', 'node_3') %>%
    add_connection('node_2', 'node_4') %>%
    add_connection('node_3', 'node_4') %>%
    add_connection('node_4', 'node_1') %>%
    set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
    set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
    set_node_parent(child_id = 'node_3', parent_id = 'node_12') %>%
    set_node_parent(child_id = 'node_4', parent_id = 'node_13')

  first_entropy <- my_sbm %>% compute_entropy()

  # Change parentage of a node
  my_sbm %>% set_node_parent(child_id = 'node_2', parent_id = 'node_12')

  # Record entropy again
  second_entropy <- my_sbm %>% compute_entropy()

  # Entropy should have changed
  expect_false(first_entropy == second_entropy)
})


test_that("MCMC Sweeps function as expected", {
  n_nodes <- 10
  n_groups <- 5
  n_sweeps <- 10

  # Start with a random network
  my_sbm <- create_sbm(sim_random_network(n_nodes = n_nodes)) %>%
    initialize_groups(num_groups = n_groups)

  original_num_groups <- get_num_groups(my_sbm)

  sweep_and_check_n_groups <- function(i, variable_num_groups, sbm){
    sweep <- mcmc_sweep(sbm, variable_num_groups = variable_num_groups)
    sweep$total_num_groups <- get_num_groups(sbm)
    sweep
  }

  # Run MCMC sweeps that do not allow group numbers to change
  n_groups_stays_same_results <- 1:n_sweeps %>%
    purrr::map(sweep_and_check_n_groups, variable_num_groups = FALSE, sbm = my_sbm)

  # Every step should result in the same number of groups in model
  n_groups_stays_same_results %>%
    purrr::map_int('total_num_groups') %>%
    magrittr::equals(original_num_groups) %>%
    all() %>%
    expect_true()


  # Now let the model change number of groups and see if it ever does
  n_groups_changes <- 1:n_sweeps %>%
    purrr::map(sweep_and_check_n_groups, variable_num_groups = TRUE, sbm = my_sbm)

  # Expect at least one group change
  n_groups_changes %>%
    purrr::map_int('total_num_groups') %>%
    magrittr::equals(original_num_groups) %>%
    magrittr::not() %>%
    any() %>%
    expect_true()
})

