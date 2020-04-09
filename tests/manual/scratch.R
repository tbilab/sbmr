set.seed(42)

# Start with a random network of two blocks with 25 nodes each
net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
  collapse_blocks(sigma = 1.4)

# Choose best result with default heuristic
net <- choose_best_collapse_state(net, verbose = TRUE)

# Score heuristic that fits a nonlinear model to observed values and chooses by
# largest negative residual
nls_score <- function(e, k){
  entropy_model <- nls(e ~ a + b * log(k), start = list(a = max(e), b = -25))
  -residuals(entropy_model)
}

# Choose result using custom heuristic function
my_sbm <- choose_best_collapse_state(net,
                                     heuristic = nls_score,
                                     verbose = TRUE)
