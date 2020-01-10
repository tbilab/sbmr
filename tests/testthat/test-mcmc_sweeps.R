# Helper to get number of blocks from a given state
get_num_blocks <- function(sbm){
  sbm %>%
    get_state() %>%
    dplyr::filter(level == 1) %>%
    nrow()
}

test_that("MCMC Sweeps function as expected", {
  n_nodes <- 10
  n_blocks <- 5
  n_sweeps <- 10

  # Start with a random network
  my_sbm <- create_sbm(sim_random_network(n_nodes = n_nodes)) %>%
    initialize_blocks(num_blocks = n_blocks)

  original_num_blocks <- get_num_blocks(my_sbm)

  sweep_and_check_n_blocks <- function(i, variable_num_blocks, sbm){
    sweep <- mcmc_sweep(sbm, variable_num_blocks = variable_num_blocks)
    sweep$total_num_blocks <- get_num_blocks(sbm)
    sweep
  }

  # Run MCMC sweeps that do not allow block numbers to change
  n_blocks_stays_same_results <- 1:n_sweeps %>%
    purrr::map(sweep_and_check_n_blocks, variable_num_blocks = FALSE, sbm = my_sbm)

  # Every step should result in the same number of blocks in model
  n_blocks_stays_same_results %>%
    purrr::map_int('total_num_blocks') %>%
    magrittr::equals(original_num_blocks) %>%
    all() %>%
    expect_true()


  # Now let the model change number of blocks and see if it ever does
  n_blocks_changes <- 1:n_sweeps %>%
    purrr::map(sweep_and_check_n_blocks, variable_num_blocks = TRUE, sbm = my_sbm)

  # Expect at least one block change
  n_blocks_changes %>%
    purrr::map_int('total_num_blocks') %>%
    magrittr::equals(original_num_blocks) %>%
    magrittr::not() %>%
    any() %>%
    expect_true()
})


test_that("Pair tracking can be enabled and disabled",{
  num_sweeps <- 3

  # Start with a random network
  my_sbm <- create_sbm(sim_random_network(n_nodes = 30)) %>%
    initialize_blocks(num_blocks = 5)

  # Run a few sweeps where pair tracking is enabled
  pair_tracking_sweeps <- mcmc_sweep(my_sbm, num_sweeps = num_sweeps, track_pairs = TRUE)

  expect_true(
    exists('pairing_counts', where=pair_tracking_sweeps)
  )

  # Run a few sweeps without pair tracking
  no_pair_tracking_sweeps <- mcmc_sweep(my_sbm, num_sweeps = num_sweeps, track_pairs = FALSE)
  expect_false(
    exists('pairing_counts', where=no_pair_tracking_sweeps)
  )

})

