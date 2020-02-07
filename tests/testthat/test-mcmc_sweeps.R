# Helper to get number of blocks from a given state
library(dplyr)

test_that("MCMC Sweeps function as expected", {
  n_blocks <- 5
  n_sweeps <- 10

  # Start with a random network
  net <- sim_random_network(n_nodes = 10) %>%
    initialize_blocks(num_blocks = n_blocks)

  sweep_and_check_n_blocks <- function(i, variable_num_blocks, sbm){
    net %>%
      mcmc_sweep(variable_num_blocks = variable_num_blocks) %>%
      get_num_blocks()
  }

  # Run MCMC sweeps that do not allow block numbers to change
  n_blocks_stays_same_results <- 1:n_sweeps %>%
    purrr::map_int(sweep_and_check_n_blocks, variable_num_blocks = FALSE, sbm = my_sbm)

  # Every step should result in the same number of blocks in model
  n_blocks_stays_same_results %>%
    magrittr::equals(n_blocks) %>%
    all() %>%
    expect_true()


  # Now let the model change number of blocks and see if it ever does
  n_blocks_changes <- 1:n_sweeps %>%
    purrr::map_int(sweep_and_check_n_blocks, variable_num_blocks = TRUE, sbm = my_sbm)

  # Expect at least one block change
  n_blocks_changes %>%
    magrittr::equals(n_blocks) %>%
    magrittr::not() %>%
    any() %>%
    expect_true()
})


test_that("Pair tracking can be enabled and disabled",{
  num_sweeps <- 3

  # Start with a random network
  net <- sim_random_network(n_nodes = 30) %>%
    initialize_blocks(num_blocks = 5)

  # Run a few sweeps where pair tracking is enabled
  pair_tracking_sweeps <- mcmc_sweep(net, num_sweeps = num_sweeps, track_pairs = TRUE)$mcmc_sweeps

  expect_true(
    exists('pairing_counts', where = pair_tracking_sweeps)
  )

  # Run a few sweeps without pair tracking
  no_pair_tracking_sweeps <- mcmc_sweep(net, num_sweeps = num_sweeps, track_pairs = FALSE)$mcmc_sweeps
  expect_false(
    exists('pairing_counts', where=no_pair_tracking_sweeps)
  )

})


test_that("No nodes should have pairs for more steps than number of sweeps",{
  num_sweeps <- 5
  num_trails <- 20

  # Start with a random network
  net <- sim_random_network(n_nodes = 30)

  for(i in num_trails){

    # Run a few sweeps where pair tracking is enabled
    pair_connections <- mcmc_sweep(initialize_blocks(net,5),
                                   num_sweeps = num_sweeps,
                                   track_pairs = TRUE)$mcmc_sweeps$pairing_counts


    # Make sure that none of the times connected exceeds the number of sweeps requested
    expect_false(
      any(pair_connections$times_connected > num_sweeps)
    )
  }

})
