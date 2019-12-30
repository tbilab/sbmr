# Helper function to collapse blocks into order-independent form and remove block from generated node name
sorted_block_collapse <- function(block_1, block_2){
  purrr::map2_chr(
    block_1,
    block_2,
    ~paste(sort(c(.x, .y)), collapse = "-")
  )
}

test_that("Mean of propensity distribution is reflected in draws", {


  simulated <- sim_basic_block_network(n_blocks = 4, n_nodes_per_block = 10)
  sim_nodes <- simulated$nodes
  sim_edges <- simulated$edges
  n_total_nodes <- 4*10

  sim_edges %>%
    dplyr::mutate(node_pair = sorted_block_collapse(from, to)) %>%
    dplyr::count(node_pair)

  # Helper function to extract block from generated node name
  get_block <- . %>% stringr::str_remove("_[0-9]+")

  sim_edges %>%
    dplyr::mutate(from_block = get_block(from))

  expect_equal(2 * 2, 4)
})
