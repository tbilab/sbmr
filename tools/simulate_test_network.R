# Code to build the testing networks 
library(bisbmsim)
library(tidyverse)


N_a <- 25  # Number of nodes of the a type
N_b <- 25  # Number of nodes of the b type
K_a <- 3    # How many blocks of a type nodes are there
K_b <- 3    # How many blocks of the b type nodes are there
b_a <- assign_group_membership(N = N_a, K = K_a) # Block membership for each a node
b_b <- assign_group_membership(N = N_b, K = K_b) # Block membership for each b node




# simple_data <- tribble(
#   ~b1,  ~b2,  ~b3,  ~b4,  ~size,
#   1,    1,    0,    0,     5,
#   0,    0,    1,    0,     4,
#   0,    1,    1,    1,     5
# ) %>% 
#   setup_planted_pattern_model(
#     num_noise_nodes = 0,
#     noise_p = 0.03,
#     planted_p_on = 0.95,
#     planted_p_off = 0.01
#   ) %$%
#   draw_from_model(
#     b_a, b_b, Lambda, 
#     binary_connections = TRUE
#   ) %>% 
simple_data <- draw_from_model(
    b_a, b_b, 
    Lambda = generate_random_lambda(K_a = K_a, K_b = K_b)
  ) %>% 
  filter(num_edges > 0) %>% 
  select(a, b) %>% 
  transmute(
    a = paste0('a',a),
    b = paste0('b', b)
  )

simple_data %>% 
  gather(key = 'type', value = 'id') %>% 
  distinct() %>% 
  mutate(type = ifelse(type == 'a', 1, 0)) %>% 
  glue::glue_data(
    "my_SBM.add_node(\"{id}\", {type});"
  )

simple_data %>% 
  glue::glue_data(
    "my_SBM.add_connection(\"{a}\", \"{b}\");"
  )

log_fact_stir <- function(k){
  return( ((k + 0.5) * log(k)) - k + (0.5*log(2*pi)) )
}

log_fact_bad <- function(k){
  return( log(factorial(k)))
}

# tibble(
#   k = 1:5
# ) %>% 
#   mutate(
#     stirling = map_dbl(k, log_fact_stir),
#     normal = map_dbl(k, log_fact_bad)
#   ) %>% 
#   pivot_longer(
#     cols = -k
#   ) %>% 
#   ggplot(aes(x = k, y = value, color = name)) +
#   scale_y_log10() + 
#   geom_line()



