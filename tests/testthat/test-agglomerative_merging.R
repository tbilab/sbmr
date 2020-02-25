test_that("Agglomerative merging with MCMC works", {

  n_blocks <- 2
  n_nodes_per_block <- 30

  net <- sim_basic_block_network(n_blocks = n_blocks,
                                 n_nodes_per_block = n_nodes_per_block,
                                 random_seed = 42) %>%
    collapse_blocks(num_mcmc_sweeps = 1, report_all_steps = TRUE)

  blocks_per_collapse <- net$collapse_results$num_blocks

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
      collapse_blocks(num_mcmc_sweeps = 0, report_all_steps = TRUE)

  blocks_per_collapse <- net$collapse_results$num_blocks

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
                    report_all_steps = FALSE)

  expect_equal(nrow(net$collapse_results), 1)
})

#
#
# test_that("Collapse run works in sequential mode", {
#   set.seed(7)
#   library(tidyverse)
#
#   # net <- sim_basic_block_network(n_blocks          = 2,
#   #                                n_nodes_per_block = 30,
#   #                                return_edge_propensities = TRUE,
#   #                                random_seed       = 42)
#
#   block_info <- dplyr::tribble(
#     ~block, ~n_nodes,
#     "a",       30,
#     "b",       30,
#   )
#
#   edge_props <- tribble(
#     ~block_1, ~block_2, ~propensity,
#          "a",      "a",         0.5,
#          "a",      "b",         0.2,
#          "b",      "b",         0.001,
#   )
#
#   net <- sim_sbm_network(block_info,
#                   edge_props,
#                   edge_dist = purrr::rbernoulli,
#                   random_seed = 42)
# #
# #   net$edges %>%
# #     mutate(i = row_number()) %>%
# #     pivot_longer(c(from, to)) %>%
# #     left_join(net$nodes, by = c("value" = "id")) %>%
# #     group_by(i) %>%
# #     summarise(pair = sort(block) %>% paste(collapse = "-")) %>%
# #     count(pair)
# #
#
#   # visualize_network(net)
#   for(i in 1:100){
#     new_sbm_network(edges = net$edges,
#                     nodes = net$nodes,
#                     random_seed = 42) %>%
#       collapse_run(num_final_blocks = 1:5, num_mcmc_sweeps = 3)
#   }
#
#
#
#
#   expect_equal(nrow(net$collapse_results), 5)
# })
#
#
#
#
