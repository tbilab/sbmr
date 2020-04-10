set.seed(42)

# Start with a random network of two blocks with 25 nodes each
net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
  collapse_run(num_final_blocks = 1:5, sigma = 1.5)

# # Collapse runs can be done in parallel
# \dontrun{
#   net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#     collapse_run(num_final_blocks = 1:5, sigma = 1.5, parallel = TRUE)
# }

# We can look directly at the collapse results
net %>% get_collapse_results()

# We can visualize the collapse results
net %>% visualize_collapse_results()
net %>% visualize_collapse_results(heuristic = "delta_ratio")

# We can choose best result with default heuristic
net <- choose_best_collapse_state(net, verbose = TRUE)
