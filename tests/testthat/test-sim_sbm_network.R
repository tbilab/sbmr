block_info <- dplyr::tribble(
 ~block, ~n_nodes,
    "a",       10,
    "b",       12,
    "c",       15
)

edge_propensities <- dplyr::tribble(
~block_1, ~block_2, ~propensity,
     "a",      "a",         0.7,
     "a",      "b",         0.2,
     "a",      "c",         0.1,
     "b",      "b",         0.9,
     "b",      "c",         0.4,
     "c",      "c",         0.4,
)



test_that("Expected number of nodes returned", {
  expect_equal(
    attr(sim_sbm_network(block_info, edge_propensities, random_seed = 42), 'n_nodes'),
    sum(block_info$n_nodes)
  )
})

test_that("Partite structure can be reflected by zeroing out or ommitting block combos", {
  block_info <- dplyr::tribble(
    ~block, ~n_nodes,
    "a1",       15,
    "a2",       11,
    "b1",       12,
    "b2",       15
  )

  edge_propensities <- dplyr::tribble(
    ~block_1, ~block_2, ~propensity,
    "a1",         "b1",         5,
    "a1",         "b2",         3,
    "a2",         "b1",         1,
    "a2",         "b2",         7,
  )

  simulated_edges <- sim_sbm_network(block_info, edge_propensities, random_seed = 42)$edges

  # There should be no edges between any nodes that have the same type as
  # encoded in letter before block name
  get_type <- . %>%
    stringr::str_remove("[1|2]_[0-9]+")

  # There should be no edges between nodes of the same type
  expect_false(
    any(get_type(simulated_edges$from) == get_type(simulated_edges$to))
  )
})

test_that("Higher propensity block combos should be reflected with more edges", {
  block_info <- dplyr::tribble(
    ~block, ~n_nodes,
    "a",       50,
    "b",       50,
    "c",       50
  )

  edge_propensities <- dplyr::tribble(
    ~block_1, ~block_2, ~propensity,
    "a",      "a",         0.9,
    "a",      "b",         0.5,
    "a",      "c",         0.3,
    "b",      "b",         0.7,
    "b",      "c",         0.1,
    "c",      "c",         0.4,
  )

  # Helper function to collapse blocks into order-independent form and remove block from generated node name
  sorted_block_collapse <- function(block_1, block_2){
    purrr::map2_chr(
      block_1,
      block_2,
      ~paste(sort(c(.x, .y)), collapse = "-")
    )
  }

  # Helper function to extract block from generated node name
  get_block <- . %>% stringr::str_remove("_[0-9]+")


  # First get the observed ordering of least likely to connect to most likely to
  # connect pairs of blocks
  # Note that these are zero censored values so true mean will be different than real Lambda
  observed_pair_order <- sim_sbm_network(block_info, edge_propensities, random_seed = 42)$edges %>%
    dplyr::mutate(
      blocks = sorted_block_collapse(get_block(from),get_block(to))
    ) %>%
    dplyr::group_by(blocks) %>%
    dplyr::summarise(
      avg_num_cons = sum(edges)/dplyr::n()
    ) %>%
    dplyr::arrange(avg_num_cons) %>%
    dplyr::pull(blocks)

  # Next get the same from our defining edge propensities
  true_pair_order <- edge_propensities %>%
    dplyr::transmute(
      blocks = sorted_block_collapse(block_1, block_2),
      propensity
    ) %>%
    dplyr::arrange(propensity) %>%
    dplyr::pull(blocks)

  # Check that order is the same
  expect_equal(
    observed_pair_order,
    true_pair_order
  )

})
