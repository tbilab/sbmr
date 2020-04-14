# Testing performance for large networks

library(tidyverse)
library(sbmr)
devtools::load_all()


n_blocks <- 10
num_block_members <- 30

network <- sim_basic_block_network(
  n_blocks = n_blocks,
  n_nodes_per_block = num_block_members,
  propensity_drawer = function(n) rbeta(n, shape1 = 0.3, shape2 = 0.85),
  return_edge_propensities = TRUE)

network$edges %>% nrow()

network$edge_propensities %>%
  mutate(
    block_1 = factor(block_1, levels = paste0('g', 1:n_blocks)),
    block_2 = factor(block_2, levels = paste0('g', 1:n_blocks)),
  ) %>%
  ggplot(aes(x = block_1, y = block_2)) +
  geom_tile(aes(fill = propensity)) +
  scale_fill_gradient(low = "white")

my_sbm <- create_sbm(network)
collapse_results <- my_sbm %>% collapse_run(
  sigma = 4,
  num_final_blocks = 1:25,
  num_mcmc_sweeps = 15,
  parallel = TRUE
)

visualize_collapse_results(collapse_results, heuristic = 'nls_residual')

x = seq(0,1, length.out = 20)
range <- seq(0.01, 1, length.out = 10)

expand_grid(
  shape1 = range,
  shape2 = range
) %>% {
  purrr::map2_dfr(
    .$shape1,
    .$shape2,
    function(s1, s2){
      tibble(x = x, shape1 = s1, shape2 = s2) %>%
        mutate(d = dbeta(x, shape1 = s1, shape2 = s2))
    }
  )
} %>%
  ggplot(aes(x = x, y = d)) +
  geom_line() +
  facet_grid(shape1~shape2) +
  theme_minimal()

