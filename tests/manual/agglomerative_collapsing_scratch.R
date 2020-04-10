library(tidyverse)
library(glue)
library(sbmR)

set.seed(42)

n_blocks <- 4    # Total number of blocks
block_size <- 50 # How many nodes in each block

network <- sim_basic_block_network(
  n_blocks = n_blocks,
  n_nodes_per_block = block_size,
  return_edge_propensities = TRUE
)

my_sbm <- create_sbm(network)

# start_entropy <- entropy(my_sbm)

# start_entropy <- my_sbm %>% initialize_blocks() %>% entropy()
single_collapse <- my_sbm %>%
  collapse_blocks(report_all_steps = TRUE,
                  sigma = 1.1,
                  num_mcmc_sweeps = 0,
                  desired_n_blocks = 2)

visualize_collapse_results(single_collapse)

my_sbm <- choose_best_collapse_state(my_sbm, single_collapse, use_entropy_value_for_score = TRUE, heuristic = "trend_deviation", verbose = TRUE)

single_collapse %>%
  pivot_longer(c(entropy, entropy_delta)) %>%
  ggplot(aes(x = n_blocks, y = value)) +
  geom_line() +
  geom_point() +
  facet_grid(rows = vars(name), scales = "free_y")


distance_between_line_and_pt <- function(x1,y1,x2,y2,px,py){
  abs((y2 - y1)*px - (x2 - x1)*py + x2*y1 - y2*x1)/sqrt( (y2-y1)^2 + (x2-x1)^2 )
}


single_collapse %>%
  arrange(n_blocks) %>%
  filter(n_blocks < 25) %>%
  select(value = entropy, k = n_blocks) %>%
  mutate(
    distance_to_line = distance_between_line_and_pt(lag(k), lag(value),
                                                    lead(k), lead(value),
                                                    k, value)
  ) %>%
  pivot_longer(c(value, distance_to_line)) %>%
  # pivot_longer(c(value, predicted_value)) %>%
  ggplot(aes(x = k, y = value)) +
  geom_line(aes(color = name)) +
  facet_grid(rows = vars(name), scales = "free_y") +
  geom_vline(xintercept = n_blocks, color = 'orangered')
  # xlim(0, 25)

single_collapse %>%
  select(-entropy_delta) %>%
  visualize_collapse_results(heuristic = 'nls_residual') +


my_sbm <- choose_best_collapse_state(my_sbm, single_collapse, heuristic = 'dev_from_rolling_mean', verbose = TRUE)


window_size <- 2

single_collapse %>%
  arrange(n_blocks) %>%
  filter(n_blocks < 25) %>%
  mutate(
    rolling_avg = rolling_mean(entropy, window = window_size),
    rolling_avg_before = dplyr::lag(rolling_avg, 1),
    rolling_avg_after = dplyr::lead(rolling_avg, window_size),
    neighbor_ratio = rolling_avg/rolling_avg_after,
    score = neighbor_ratio
  ) %>%
  pivot_longer(c(entropy, score)) %>%
  ggplot(aes(x = n_blocks, y = value)) +
  geom_line() +
  geom_point() +
  geom_vline(xintercept = n_blocks, color = 'orangered') +
  facet_grid(name~., scales = "free_y")


single_collapse %>%
  filter(n_blocks == n_blocks) %>%
  pluck('state', 1) %>%
  filter(level == 0) %>%
  right_join(network$nodes, by = 'id')%>%
  rename(inferred = parent) %>% {
    table(.$inferred, .$block)
  }


collapse_results <- my_sbm %>%
  collapse_run(num_block_proposals = 15,
               sigma = 2,
               num_mcmc_sweeps = 5,
               num_final_blocks = 1:15,
               parallel = TRUE)


visualize_collapse_results(collapse_results, heuristic = "delta_ratio") +
  geom_vline(xintercept = n_blocks, color = 'orangered')


my_sbm <- choose_best_collapse_state(my_sbm, collapse_results, heuristic =function(e,b){as.numeric(b == n_blocks)}, verbose = TRUE)

my_sbm %>%
  get_state() %>%
  filter(level == 0) %>%
  right_join(network$nodes, by = 'id')%>%
  rename(inferred = parent) %>% {
    table(.$inferred, .$block)
  }

# network$nodes %>% pull(block) %>% unique()

table(nodes_w_assignments$inferred, nodes_w_assignments$block)




true_delta <- start_entropy - true_entropy

entropy_delta/true_delta

true_delta

collapse_results %>%
  mutate(
    score = entropy - lag(entropy)
  ) %>%
  pivot_longer(c(score, entropy)) %>%
  ggplot(aes(x = n_blocks, y = value)) +
  geom_line() +
  facet_grid(name~., scales = "free_y") +
  geom_vline(xintercept = n_blocks, color = 'orangered')

collapse_results %>%
  mutate(
    entropy = entropy/first(entropy)
  ) %>%
  visualize_collapse_results(heuristic = "dev") +
  geom_vline(xintercept = n_blocks, color = 'orangered')

collapse_results %>%
  visualize_collapse_results(heuristic = 'nls_residual') +
  geom_vline(xintercept = n_blocks, color = 'orangered') +
  geom_vline(xintercept = 2*n_blocks, color = 'orangered')

my_sbm <- choose_best_collapse_state(my_sbm, collapse_results, heuristic =function(e,b){as.numeric(b == n_blocks)}, verbose = TRUE)

nodes_w_assignments <- my_sbm %>%
  get_state() %>%
  filter(level == 0) %>%
  right_join(network$nodes, by = 'id')%>%
  rename(inferred = parent)

# network$nodes %>% pull(block) %>% unique()

table(nodes_w_assignments$inferred, nodes_w_assignments$block)

nodes_w_assignments %>%
  group_by(inferred) %>%
  summarise(n_unique_true = length(unique(block))) %>%
  arrange(-n_unique_true)

nodes_w_assignments %>%
  group_by(block) %>%
  summarise(n_unique_assigned = length(unique(inferred))) %>%
  arrange(-n_unique_assigned)


my_sbm %>%
  initialize_blocks(n_blocks)

num_sweeps <- 2

sweep_results <- mcmc_sweep(my_sbm, num_sweeps = num_sweeps, track_pairs = FALSE, variable_n_blocks = TRUE)

sweep_results$sweep_info %>%
  mutate(sweep = 1:n(),
         label = 'eps = 0.1'
         # entropy_delta = cumsum(entropy_delta)
         ) %>%
  # filter(sweep > 25) %>%
  pivot_longer(entropy_delta:num_nodes_moved) %>%
  ggplot(aes(x = sweep, y = value)) +
  geom_line() +
  facet_grid(name~., scales = "free_y") +
  labs(
    title = glue::glue('Result of {num_sweeps} MCMC sweeps'),
    subtitle = "Entropy Delta of sweep and number of nodes moved for sweep"
  )
#
# my_sbm %>%
#   get_state() %>%
#   filter(level == 0) %>%
#   right_join(network$nodes, by = 'id')%>%
#   rename(inferred = parent) %>%
#   group_by(inferred) %>%
#   summarise(n_unique_true = length(unique(block))) %>%
#   arrange(-n_unique_true)
#
#
#
# e_delta = -4.926308; P_pre = 0.599292; P_post = 0.822462
# e_delta = -75.862098; P_pre = 35.979662; P_post = -1606.925421
#
# exp(-e_delta)*(P_post/P_pre)
#
# e_delta <- -71.744486
# P_pre <- 119.900000
# P_post <-  46.900000
#
#
# sweep_results$sweep_info %>% arrange(entropy_delta)
# # collapse_results <- my_sbm %>%
# #   collapse_run(num_block_proposals = 15,
# #                sigma = 6,
# #                num_mcmc_sweeps = 15,
# #                num_final_blocks = 1:15,
# #                parallel = TRUE)
# #
# # visualize_collapse_results(collapse_results, heuristic = 'dev_from_rolling_mean') +
# #   geom_vline(xintercept = n_blocks, color = 'orangered')
#
# tibble(
#   x = seq(-2, 2, length.out = 30),
#   y = exp(x)
# ) %>%
#   ggplot(aes( x = x, y = y) ) +
#   geom_line() +
#   geom_hline(yintercept = 1)
#
#
# my_sbm %>% entropy()
#
# plot_sweep_results(sweep_df)

tibble(
  x = seq(0, 1, length.out = 130),
  y = log(x)
) %>%
  ggplot(aes(x = x, y = y)) + geom_line()
