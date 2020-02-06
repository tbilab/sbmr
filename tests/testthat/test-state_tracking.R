requireNamespace("lobstr", quietly = TRUE)

test_that("State tracking returns the correct state", {

  edges <- dplyr::tribble(
    ~from, ~to,
    "a1"   , "b1"   ,
    "a1"   , "b2"   ,
    "a1"   , "b3"   ,
    "a2"   , "b1"   ,
    "a2"   , "b4"   ,
    "a3"   , "b1"
  )

  nodes <- dplyr::tribble(
    ~id, ~type,
    "a1", "node",
    "a2", "node",
    "a3", "node",
    "b1", "node",
    "b2", "node",
    "b3", "node",
    "b4", "node"
  )

  net <- new_sbm_network(edges = edges, nodes = nodes)

  expect_equal(
    dplyr::as_tibble(net$model$get_state()),
    dplyr::tribble(
       ~id, ~parent,  ~type, ~level,
      "a1",  "none", "node",      0L,
      "a2",  "none", "node",      0L,
      "a3",  "none", "node",      0L,
      "b1",  "none", "node",      0L,
      "b2",  "none", "node",      0L,
      "b3",  "none", "node",      0L,
      "b4",  "none", "node",      0L
    )
  )

  expect_equal(net$model$get_state(), attr(net, 'state'))
})


test_that("No costly duplication of S4 class is done when assigning s3 class copies.", {

  net <- sim_basic_block_network(n_blocks = 3,
                                     n_nodes_per_block = 40,
                                     setup_model = TRUE) %>%
    initialize_blocks(num_blocks = 5)

  # Take snapshot of model and state before sweep
  pre_sweep_model_address <- lobstr::obj_addr(net$model)

  # Run MCMC sweep
  mcmc_results <- net %>% mcmc_sweep(num_sweeps = 25)

  # Take snapshot after sweep
  post_sweep_model_address <- lobstr::obj_addr(net$model)

  # The addresses and the directly accessed state should stay the same

  # Load up returned sbm_network from results
  post_sweep_returned_sbm_model_address <- lobstr::obj_addr(mcmc_results$sbm_network$model)

  # All objects should be equal
  expect_equal(pre_sweep_model_address, post_sweep_model_address)
  expect_equal(pre_sweep_model_address, post_sweep_returned_sbm_model_address)
  expect_equal(post_sweep_model_address, post_sweep_returned_sbm_model_address)
})

test_that("S3 class attributes are immutable", {

  net <- sim_basic_block_network(n_blocks = 3,
                                     n_nodes_per_block = 40,
                                     setup_model = TRUE) %>%
    initialize_blocks(num_blocks = 5)

  # Take snapshot of model and state before sweep
  pre_sweep_attr_state <- attr(net, 'state')

  # Run MCMC sweep
  mcmc_results <- net %>% mcmc_sweep(num_sweeps = 25)

  # Check snapshot of state after sweep
  post_sweep_attr_state <- attr(net, 'state')

  expect_equal(pre_sweep_attr_state, post_sweep_attr_state)

  # The returned SBM model should have a different (updated state, however)
  returned_net_attr_state <- attr(mcmc_results$sbm_network, 'state')

  expect_false(isTRUE(all.equal(pre_sweep_attr_state,returned_net_attr_state)))
})


