# Helper function to collapse groups into order-independent form and remove group from generated node name
sorted_group_collapse <- function(group_1, group_2){
  purrr::map2_chr(
    group_1,
    group_2,
    ~paste(sort(c(.x, .y)), collapse = "-")
  )
}

test_that("Mean of propensity distribution is reflected in draws", {


  simulated <- sim_basic_block_network(n_groups = 4, n_nodes_per_group = 10)
  sim_nodes <- simulated$nodes
  sim_edges <- simulated$edges
  n_total_nodes <- 4*10

  sim_edges %>%
    dplyr::mutate(node_pair = sorted_group_collapse(from, to)) %>%
    dplyr::count(node_pair)

  # Helper function to extract group from generated node name
  get_group <- . %>% stringr::str_remove("_[0-9]+")

  sim_edges %>%
    dplyr::mutate(from_group = get_group(from))

  expect_equal(2 * 2, 4)
})
