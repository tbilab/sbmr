
test_that("Agglomerative merging with MCMC works", {

  n_blocks <- 2
  n_nodes_per_block <- 30

  simulated_network <- sim_basic_block_network(n_blocks = n_blocks,
                                               n_nodes_per_block = n_nodes_per_block,
                                               return_connection_propensities = TRUE)

  # Start with a random network
  my_sbm <- create_sbm(simulated_network)
  collapse_results <- collapse_blocks(my_sbm, num_mcmc_sweeps = 1, report_all_steps = TRUE)

  blocks_per_collapse <- collapse_results$num_blocks

  # Make sure that there are always fewer blocks after each step
  for(i in 2:length(blocks_per_collapse)){
    expect_lt(blocks_per_collapse[i], blocks_per_collapse[i - 1])
  }
})


test_that("Agglomerative merging without MCMC works", {

  n_blocks <- 2
  n_nodes_per_block <- 30

  simulated_network <- sim_basic_block_network(n_blocks = n_blocks,
                                               n_nodes_per_block = n_nodes_per_block,
                                               return_connection_propensities = TRUE)

  # Start with a random network
  my_sbm <- create_sbm(simulated_network)
  collapse_results <- collapse_blocks(my_sbm, num_mcmc_sweeps = 0, report_all_steps = TRUE)

  blocks_per_collapse <- collapse_results$num_blocks

  # Make sure that there are always fewer blocks after each step
  for(i in 2:length(blocks_per_collapse)){
    expect_lt(blocks_per_collapse[i], blocks_per_collapse[i - 1])
  }

})


test_that("Requesting just the final merge step returns just the final merge step", {

  # Start with a random network
  my_sbm <- create_sbm(
    sim_basic_block_network(n_blocks = 4, n_nodes_per_block = 30)
  )

  collapse_results <- my_sbm %>%
    collapse_blocks(desired_num_blocks = 4,
                    num_mcmc_sweeps = 0,
                    report_all_steps = FALSE)

  expect_equal(nrow(collapse_results), 1)
})


test_that("Collapse run works in sequential mode", {

  # Start with a random network
  my_sbm <- create_sbm(
    sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 30)
  )

  num_expected_results <- 5

  collapse_results <- my_sbm %>%
    collapse_run(num_final_blocks = 1:5,
                 num_mcmc_sweeps = 3)

  expect_equal(nrow(collapse_results), num_expected_results)
})




