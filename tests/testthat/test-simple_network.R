test_that("Proper number of connections happens", {

  n_edges <- 5
  prob_of_connection <- 0.5
  n_samples <- 50

  expected_num_edges <- n_edges*prob_of_connection*(n_edges-1)

  edges_per_sample <- purrr::map_int(1:n_samples, ~{nrow(sim_simple_network(n_edges, prob_of_connection))})

  expect_equal(mean(edges_per_sample), expected_num_edges, tolerance = 0.1)
})
