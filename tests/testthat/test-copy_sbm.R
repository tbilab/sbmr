test_that("Copy occupies seperate memory location", {

  # Generate a random network to build initial sbm with
  network <- sim_random_network(n_nodes = 10)

  # Generate initial sbm
  initial_sbm <- create_sbm(network)

  # Copy initial sbm
  copied_sbm <- copy_sbm(initial_sbm)

 # Make sure they arent actually the same object in memory
  expect_true(lobstr::obj_addr(initial_sbm) != lobstr::obj_addr(copied_sbm))
})

test_that("Copy without state works", {

  # Generate a random network to build initial sbm with
  network <- sim_random_network(n_nodes = 10)

  # Generate initial sbm
  initial_sbm <- create_sbm(network)

  # Copy initial sbm
  copied_sbm <- copy_sbm(initial_sbm)

  # Make sure that the state output matches (minus the parent info)
  remove_parent <- . %>% dplyr::select(-parent)
  expect_equal(get_state(copied_sbm) %>% remove_parent(),
               get_state(initial_sbm) %>% remove_parent())
})

test_that("Copy with state works", {
  # Generate a random network to build initial sbm with
  network <- sim_random_network(n_nodes = 15)

  # Generate initial sbm and randomly assign nodes to 3 blocks
  initial_sbm <- create_sbm(network) %>%
    initialize_blocks(num_blocks = 3)

  # Copy initial sbm and tell it to match state as well
  copied_sbm <- copy_sbm(initial_sbm, match_state = TRUE)

  # All should be equal now
  expect_equal(get_state(copied_sbm), get_state(initial_sbm))
})

test_that("Copying an empty SBM", {

  initial_sbm <- create_sbm()

  # Copy initial sbm and tell it to match state as well
  # This just tests that there is no error
  expect_true({copy_sbm(initial_sbm); TRUE})
})

test_that("Copying SBM without edges", {

  initial_sbm <- create_sbm() %>%
    add_node('a1') %>%
    add_node('a2')

  # Copy initial sbm and tell it to match state as well
  expect_true({copy_sbm(initial_sbm); TRUE})
})

test_that("Hyperparameters are copied over", {
  eps <- 2
  sigma <- 3
  greedy <- TRUE
  n_checks <- 17

  initial_sbm <- create_sbm(eps = eps)
  initial_sbm$SIGMA <- sigma
  initial_sbm$GREEDY <- greedy
  initial_sbm$N_CHECKS_PER_block <- n_checks

  # Copy initial sbm and tell it to match state as well
  copied_sbm <- copy_sbm(initial_sbm)

  expect_equal(initial_sbm$EPS, eps)
  expect_equal(initial_sbm$SIGMA, sigma)
  expect_equal(initial_sbm$GREEDY, greedy)
  expect_equal(initial_sbm$N_CHECKS_PER_block, n_checks)
})
