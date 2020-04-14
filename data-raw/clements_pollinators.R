## code to prepare `clements_polinators` dataset goes here

# Original data for network was downloaded from https://www.nceas.ucsb.edu/interactionweb/html/clements_1923.html on
# February 18, 2020
requireNamespace("usethis", quietly = TRUE)
library(tidyverse)

pollinator_wide <- read_csv('data-raw/clements_pollinators.csv')

clements_pollinators <- pollinator_wide %>%
  pivot_longer(-pollinator, names_to = "flower") %>%
  filter(value == 1) %>%
  select(-value)

usethis::use_data(clements_pollinators,overwrite = TRUE)
