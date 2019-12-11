test_that("Proper number of connections happens", {

  n_edges <- 5
  prob_of_connection <- 0.5
  n_samples <- 50


  sim_avg_con_count <- function(self_connections){
    1:n_samples %>%
      purrr::map_int(~{
        nrow(sim_simple_network(n_edges, prob_of_connection, allow_self_connections = self_connections))
      }) %>%
      mean()
  }

  expected_num_edges_wo_self_cons <- n_edges*prob_of_connection*(n_edges-1)
  expected_num_edges_w_self_cons <- n_edges*prob_of_connection*n_edges

  edges_per_w_self_cons <- sim_avg_con_count(TRUE)
  edges_per_wo_self_cons <- sim_avg_con_count(FALSE)

  expect_equal(edges_per_w_self_cons, expected_num_edges_w_self_cons, tolerance = 0.1)
  expect_equal(edges_per_wo_self_cons, expected_num_edges_wo_self_cons, tolerance = 0.1)
})
