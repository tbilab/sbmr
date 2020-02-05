test_that("Proper number of edges happens", {

  n_nodes <- 5
  prob_of_edge <- 0.5
  n_samples <- 100

  sim_avg_con_count <- function(self_edges){
    1:n_samples %>%
      purrr::map_int(~{
        nrow(sim_random_network(n_nodes, prob_of_edge, allow_self_edges = self_edges)$edges)
      }) %>%
      mean()
  }


  # Calculate the expected number of edges. This is divided by two because we
  # only draw unique combos of blocks so if we have edges for nodes a->b we wont
  # also have b->a.
  expected_num_edges_w_self_cons <- choose(n_nodes + 1, 2) * prob_of_edge
  edges_per_w_self_cons <- sim_avg_con_count(TRUE)
  expect_equal(edges_per_w_self_cons, expected_num_edges_w_self_cons, tolerance = 0.1)


  expected_num_edges_wo_self_cons <- choose(n_nodes, 2) * prob_of_edge
  edges_per_wo_self_cons <- sim_avg_con_count(FALSE)
  expect_equal(edges_per_wo_self_cons, expected_num_edges_wo_self_cons, tolerance = 0.1)
})
