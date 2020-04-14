
# Start with a random network
net <- sim_basic_block_network(n_blocks = 2, n_nodes_per_block = 30, random_seed = 42) %>%
  collapse_blocks(num_mcmc_sweeps = 0, sigma = 1.1, report_all_steps = TRUE)

visualize_collapse_results(net, heuristic = "delta_ratio", use_entropy = FALSE)


net <- sim_basic_block_network(n_blocks = 2, n_nodes_per_block = 30, random_seed = 42) %>%
  collapse_run(num_mcmc_sweeps = 0, sigma = 1.3)

visualize_collapse_results(net, heuristic = "delta_ratio", use_entropy = FALSE)
