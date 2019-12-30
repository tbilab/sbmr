## code to prepare `simulated_network` dataset goes here

# Code to build the testing networks
library(bisbmsim)
library(tidyverse)


N_a <- 25  # Number of nodes of the a type
N_b <- 25  # Number of nodes of the b type
K_a <- 3    # How many blocks of a type nodes are there
K_b <- 3    # How many blocks of the b type nodes are there
b_a <- assign_block_membership(N = N_a, K = K_a) # Block membership for each a node
b_b <- assign_block_membership(N = N_b, K = K_b) # Block membership for each b node


edges <- draw_from_model(
  b_a, b_b,
  Lambda = generate_random_lambda(K_a = K_a, K_b = K_b)
) %>%
  filter(num_edges > 0) %>%
  select(a, b) %>%
  transmute(
    from = paste0('a',a),
    to = paste0('b', b)
  )

nodes <- dplyr::bind_rows(
  tibble(
    id = unique(edges$from),
    type = "a"
  ),
  tibble(
    id = unique(edges$to),
    type = "b"
  )
)

simulated_network <- list(edges = edges, nodes = nodes)


usethis::use_data(simulated_network)


