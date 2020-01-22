# Reads in io captured report of MCMC sweeps and makes plots to investigate behavior.
library(tidyverse)

sweep_info <- read_csv(here::here("tests/manual/mcmc_output.txt"))


sweep_info %>%
  filter(prob_of_accept > 5, entropy_delta < -50)

sweep_info %>%
  # filter(prob_of_accept < 40) %>%
  ggplot(aes(x = entropy_delta, y = prob_of_accept, color = move_accepted)) +
  geom_point()

sweep_info %>%
  # filter(prob_of_accept < 4) %>%
  ggplot(aes(x = prob_of_accept)) +
  geom_histogram(bins = 40)
#
