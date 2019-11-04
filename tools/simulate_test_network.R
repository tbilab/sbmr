# Code to build the testing networks 
library(bisbmsim)
library(tidyverse)

simple_data <- tribble(
  ~b1,  ~b2,  ~b3,  ~b4,  ~size,
  1,    1,    0,    0,     5,
  0,    0,    1,    0,     4,
  0,    1,    1,    1,     5
) %>% 
  setup_planted_pattern_model(
    num_noise_nodes = 0,
    noise_p = 0.03,
    planted_p_on = 0.95,
    planted_p_off = 0.01
  ) %$%
  draw_from_model(
    b_a, b_b, Lambda, 
    binary_connections = TRUE
  ) %>% 
  filter(num_edges > 0) %>% 
  select(a, b)

simple_data %>% 
  gather(key = 'type', value = 'id') %>% 
  distinct() %>% 
  mutate(type = ifelse(type == 'a', 1, 0)) %>% 
  glue::glue_data(
    "my_SBM.get_node_by_id(\"{id}\", {type});"
  )

simple_data %>% 
  glue::glue_data(
    "my_SBM.add_connection(\"{a}\", \"{b}\");"
  )