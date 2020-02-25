test_that("Random seeds make reproducable SBMS works", {

  seed <- 42

  # Lets first generate some network data to use
  # Dont initialize model as we just want the data here.
  # network_data <- sim_basic_block_network(n_blocks = num_blocks, n_nodes_per_block = 30, setup_model = FALSE)
  network_data <- sim_random_network(n_nodes = 100, prob_of_edge = 0.5, setup_model = FALSE)

  initialize_sweep_snapshot <- . %>%
    initialize_blocks(5) %>%
    mcmc_sweep(num_sweeps = 15) %>%
    get_sweep_results() %>%
    purrr::pluck('nodes_moved')

  # Initialize a an SBM model from this network data and give it a seed
  # First we will initialize some random blocks into the network and then we
  # will run a few mcmc sweeps and finally collect the model state
  sbm_1_state <- new_sbm_network(edges = network_data$edges,
                                 nodes = network_data$nodes,
                                 random_seed = seed) %>%
    initialize_sweep_snapshot()


  # Next we can repeat the same process but with a new model with the same random seed
  sbm_2_state <- new_sbm_network(edges = network_data$edges,
                                 nodes = network_data$nodes,
                                 random_seed = seed) %>%
    initialize_sweep_snapshot()

  # The resultant state dataframes should be identical to eachother
  expect_equal(sbm_1_state, sbm_2_state)

  # Another sbm that was created with a different seed should not be the same
  sbm_3_state <- new_sbm_network(edges = network_data$edges,
                                 nodes = network_data$nodes,
                                 random_seed = 312) %>%
    initialize_sweep_snapshot()

  sbm_4_state <- new_sbm_network(edges = network_data$edges,
                                 nodes = network_data$nodes) %>%
    initialize_sweep_snapshot()

  all.equal(sbm_1_state, sbm_4_state) %>%
    isTRUE() %>%
    expect_false()
})
