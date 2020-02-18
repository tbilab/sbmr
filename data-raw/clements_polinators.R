## code to prepare `clements_polinators` dataset goes here

# Original data for network was downloaded from https://www.nceas.ucsb.edu/interactionweb/html/clements_1923.html on
# February 18, 2020

library(tidyverse)

polinator_wide <- read_csv('data-raw/clements_polinators.csv')

clements_polinators <- polinator_wide %>%
  pivot_longer(-polinator, names_to = "flower") %>%
  filter(value == 1) %>%
  select(-value)

usethis::use_data(clements_polinators)
