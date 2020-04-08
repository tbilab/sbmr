test_that("Agglomerative merging with MCMC works", {

  n_blocks <- 2
  n_nodes_per_block <- 30

  net <- sim_basic_block_network(n_blocks = n_blocks,
                                 n_nodes_per_block = n_nodes_per_block,
                                 random_seed = 42) %>%
    collapse_blocks(num_mcmc_sweeps = 1, sigma = 1.4, report_all_steps = TRUE)

  blocks_per_collapse <- net$collapse_results$n_blocks

  # Make sure that there are always fewer blocks after each step
  for(i in 2:length(blocks_per_collapse)){
    expect_lt(blocks_per_collapse[i], blocks_per_collapse[i - 1])
  }
})


test_that("Agglomerative merging without MCMC works", {

  # Start with a random network
  net <- sim_basic_block_network(n_blocks = 2,
                                 n_nodes_per_block = 30,
                                 random_seed = 42) %>%
      collapse_blocks(num_mcmc_sweeps = 0, sigma = 1.3, report_all_steps = TRUE)

  blocks_per_collapse <- net$collapse_results$n_blocks

  # Make sure that there are always fewer blocks after each step
  for(i in 2:length(blocks_per_collapse)){
    expect_lt(blocks_per_collapse[i], blocks_per_collapse[i - 1])
  }

})


test_that("Requesting just the final merge step returns just the final merge step", {

  net <- sim_basic_block_network(n_blocks = 2,
                                 n_nodes_per_block = 30,
                                 random_seed = 42) %>%
    collapse_blocks(desired_num_blocks = 4,
                    num_mcmc_sweeps = 0,
                    sigma = 1.4,
                    report_all_steps = FALSE)

  expect_equal(nrow(net$collapse_results), 1)
})




