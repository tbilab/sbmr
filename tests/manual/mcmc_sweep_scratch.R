library(tidyverse)
library(glue)
library(sbmR)

set.seed(42)

# n_blocks <- 4    # Total number of blocks
# block_size <- 50 # How many nodes in each block
n_blocks <- 3   # Total number of blocks
block_size <- 30 # How many nodes in each block

network <- sim_basic_block_network(
  n_blocks = n_blocks,
  n_nodes_per_block = block_size,
  return_edge_propensities = TRUE
)


# network %>%
#   pluck("nodes") %>%
#   glue_data("my_SBM.add_node(\"{id}\", 0);")
#
# network %>%
#   pluck("edges") %>%
#   glue_data("my_SBM.add_edge(\"{from}\", \"{to}\");")

# visualize_network(network)


my_sbm <- create_sbm(network) %>%
  initialize_blocks(n_blocks)

start_entropy <- my_sbm %>% compute_entropy()
num_sweeps <- 30
sweep_results <- mcmc_sweep(my_sbm, num_sweeps = num_sweeps, track_pairs = FALSE, variable_num_blocks = FALSE, verbose = FALSE)
end_entropy <- my_sbm %>% compute_entropy()

# sweep_info <- read_csv(here::here("tests/manual/sweep_results.txt"))
# sweep_info %>%
#   filter(prob_of_accept < 40) %>%
#   ggplot(aes(x = entropy_delta, y = prob_of_accept, color = move_accepted)) +
#   geom_point()

sweep_info %>%
  filter(prob_of_accept < 4) %>%
  ggplot(aes(x = prob_of_accept)) +
  geom_histogram(bins = 40)


sweep_results$sweep_info %>%
  mutate(sweep = 1:n()) %>%
  pivot_longer(c(entropy_delta, num_nodes_moved)) %>%
  ggplot(aes(x = sweep, y = value)) +
    geom_line() +
    geom_point() +
    facet_grid(name~., scales = "free_y")

node_states <- get_state(my_sbm) %>%
  filter(level == 0)

node_states %>%
  count(parent)

nodes_w_assignments <- node_states %>%
  right_join(network$nodes, by = 'id')%>%
  rename(inferred = parent)

nodes_w_assignments %>%
  group_by(inferred) %>%
  summarise(n_unique_true = length(unique(block))) %>%
  arrange(-n_unique_true)

nodes_w_assignments %>%
  group_by(block) %>%
  summarise(n_unique_assigned = length(unique(inferred))) %>%
  arrange(-n_unique_assigned)


start_entropy <- compute_entropy(my_sbm)
predicted_entropy <- start_entropy

num_sweeps <- 20
true_res <- c(start_entropy)
reported_deltas <- c(0)
predicted_res <- c(start_entropy)

for(sweep in 1:num_sweeps){
  # One sweep
  sweep_results <- mcmc_sweep(my_sbm, num_sweeps = 1, track_pairs = FALSE, variable_num_blocks = FALSE)

  # Extract entropy delta
  delta <- sweep_results$sweep_info$entropy_delta[1]
  reported_deltas <- c(reported_deltas, delta)
  predicted_entropy <- predicted_entropy + (delta)

  # Update values
  true_res <- c(true_res, compute_entropy(my_sbm))
  predicted_res <- c(predicted_res, predicted_entropy)
}

comparison_res <- tibble(
  true = true_res,
  delta = reported_deltas,
  predicted = predicted_res,
  sweep = 0:num_sweeps
) %>%
  mutate(difference = true-predicted)


comparison_res %>%
  mutate(true_delta = true - lag(true),
         true_delta = ifelse(is.na(true_delta), 0, true_delta),
         delta_new = delta) %>%
  pivot_longer(c(delta_new, true_delta)) %>%
  ggplot(aes(x = sweep, y = value, color = name)) +
  geom_point() +
  geom_line()



comparison_res %>%
  pivot_longer(c(true, predicted)) %>%
  ggplot(aes(x = sweep, y = value, color = name)) +
  geom_line()


sweep_results <- mcmc_sweep(my_sbm, num_sweeps = num_sweeps, track_pairs = FALSE, variable_num_blocks = TRUE)

sweep_stats <- sweep_results$sweep_info %>%
  mutate(sweep = 1:n(),
         label = 'eps = 0.1',
         entropy = start_entropy + cumsum(-entropy_delta/2)
  )
# e_delta = -0.061816; P_move = 0.352967; P_rev = 0.421051; P_accept = 0.891756
# exp(-e_delta)*(P_move/P_rev)

sweep_stats  %>%
  pivot_longer(c(entropy_delta, num_nodes_moved)) %>%
  ggplot(aes(x = sweep, y = value)) +
  geom_line() +
  facet_grid(name~., scales = "free_y") +
  labs(
    title = glue::glue('Result of {num_sweeps} MCMC sweeps'),
    subtitle = "Entropy Delta of sweep and number of nodes moved for sweep"
  )

true_final_entropy <- my_sbm %>% compute_entropy()

sweep_stats %>%
  ggplot(aes(x = sweep, y = entropy)) +
  geom_line() +
  geom_hline(yintercept = true_final_entropy)


node_states <- get_state(my_sbm) %>%
  filter(level == 0)

node_states %>%
  count(parent)

nodes_w_assignments <- node_states %>%
  right_join(network$nodes, by = 'id')%>%
  rename(inferred = parent)

nodes_w_assignments %>%
  group_by(inferred) %>%
  summarise(n_unique_true = length(unique(block))) %>%
  arrange(-n_unique_true)

nodes_w_assignments %>%
  group_by(block) %>%
  summarise(n_unique_assigned = length(unique(inferred))) %>%
  arrange(-n_unique_assigned)

visualize_network(edges = network$edges,
                  nodes = nodes_w_assignments,
                  node_color_col = 'block',
                  node_shape_col = 'inferred',
                  width = '100%')



probs <- rbeta(n = 10, 2, 2)

top_vals <- rnorm(n = 10, mean = 5)
bottom_vals <- rnorm(n = 10, mean = 4)


sum(probs*top_vals)/sum(probs*bottom_vals)

sum(top_vals)/sum(bottom_vals)
