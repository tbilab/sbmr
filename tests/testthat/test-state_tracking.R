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
    dplyr::as_tibble(attr(net, 'model')$get_state()),
    dplyr::tribble(
       ~id, ~parent,  ~type, ~level,
      "a1",  "none", "node",     0L,
      "a2",  "none", "node",     0L,
      "a3",  "none", "node",     0L,
      "b1",  "none", "node",     0L,
      "b2",  "none", "node",     0L,
      "b3",  "none", "node",     0L,
      "b4",  "none", "node",     0L
    )
  )

  expect_equal(attr(net, 'model')$get_state(), attr(net, 'state'))
})


test_that("State updating method", {

  edges <- dplyr::tribble(
    ~from,   ~to,
     "a1",  "b1",
     "a1",  "b2",
     "a1",  "b3",
     "a2",  "b1",
     "a2",  "b4",
     "a3",  "b1"
  )

  nodes <- dplyr::tribble(
    ~id,   ~type,
    "a1", "node",
    "a2", "node",
    "a3", "node",
    "b1", "node",
    "b2", "node",
    "b3", "node",
    "b4", "node"
  )

  new_state <- dplyr::tribble(
     ~id,          ~parent,   ~type, ~level,
    "a1",       "a_parent",  "node",     0L,
    "a2",       "a_parent",  "node",     0L,
    "a3",       "a_parent",  "node",     0L,
    "b1",       "b_parent",  "node",     0L,
    "b2",       "b_parent",  "node",     0L,
    "b3",       "b_parent",  "node",     0L,
    "b4",       "b_parent",  "node",     0L,
    "a_parent", "none",      "node",     1L,
    "b_parent", "none",      "node",     1L,
  )


  net <- new_sbm_network(edges = edges, nodes = nodes)

  # Update state using update_state() method
  net <- net %>% update_state(new_state)

  # Now the states should be identical
  expect_true( dplyr::all_equal(new_state, attr(net, 'state')))
  expect_true( dplyr::all_equal(new_state, attr(net, 'model')$get_state()))


  # Now to updating using a integer-typed dataframe
  net <- new_sbm_network(edges = edges, nodes = nodes)

  # Update state using update_state() method
  net <- net %>% update_state(new_state)

  # Now the states should be identical
  expect_true( dplyr::all_equal(new_state, attr(net, 'state')))
  expect_true( dplyr::all_equal(new_state, attr(net, 'model')$get_state()))
})

test_that("No costly duplication of S4 class is done when assigning s3 class copies.", {

  net <- sim_basic_block_network(n_blocks = 3,
                                 n_nodes_per_block = 40,
                                 setup_model = TRUE) %>%
    initialize_blocks(num_blocks = 5)

  # Take snapshot of model and state before sweep
  pre_sweep_model_address <- lobstr::obj_addr(attr(net, 'model'))

  # Run MCMC sweep
  net_after_sweeps <- net %>% mcmc_sweep(num_sweeps = 25)

  # Take snapshot after sweep
  post_sweep_model_address <- lobstr::obj_addr(attr(net_after_sweeps,'model'))

  # Addresses should be equal
  expect_equal(pre_sweep_model_address, post_sweep_model_address)
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


