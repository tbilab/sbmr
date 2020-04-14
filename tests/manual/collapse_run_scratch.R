# library(tidyverse)
library(sbmr)

block_info <- dplyr::tribble(
  ~block, ~n_nodes,
  "a",       30,
  "b",       30,
)

edge_props <- dplyr::tribble(
  ~block_1, ~block_2, ~propensity,
  "a",      "a",         0.5,
  "a",      "b",         0.2,
  "b",      "b",         0.001,
)


for(i in 1:100){
  print(i)
  set.seed(42)
  sim_sbm_network(block_info,
                  edge_props,
                  edge_dist = purrr::rbernoulli,
                  random_seed = 42) %>%
   collapse_run(num_final_blocks = 2:5, num_mcmc_sweeps = 3)
}

