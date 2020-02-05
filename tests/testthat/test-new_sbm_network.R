test_that("Correct numbers of nodes and edges are returned", {
  network <- sim_basic_block_network(
    n_blocks = 3,
    n_nodes_per_block = 40
  )

  sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)

  expect_equal(attr(sbm_net, "n_nodes"), 3*40)
  expect_equal(attr(sbm_net, "n_edges"), nrow(network$edges))
})

test_that("Throws messagse for mismatched bipartite edges and nodes", {
  edges <- dplyr::tribble(
    ~from, ~to,
    "a1"   , "b1"   ,
    "a1"   , "b2"   ,
    "a1"   , "b3"   ,
    "a2"   , "b1"   ,
    "a2"   , "b4"   ,
    "a3"   , "b1"
  )

  nodes <- dplyr::tribble(
    ~id, ~type,
    "a1", "node",
    "a2", "node",
    "a3", "node",
    "b1", "node",
    "b2", "node",
    "b3", "node",
    "b4", "node"
  )

  expect_warning(
    new_sbm_network(edges = edges, nodes = nodes, bipartite_edges = TRUE, show_warnings = TRUE),
    "bipartite_edges setting ignored due to nodes dataframe being provided.",
    fixed = TRUE
  )

})

test_that("Throws messages for mismatched bipartite edges and nodes", {
  edges <- dplyr::tribble(
    ~from, ~to,
    "a1"   , "b1"   ,
    "a1"   , "b2"   ,
    "a1"   , "b3"   ,
    "a2"   , "b1"   ,
    "b1"   , "b4"   ,
    "a3"   , "b1"
  )

  expect_error(
    new_sbm_network(edges = edges, bipartite_edges = TRUE),
    "Bipartite edge structure was requested but some nodes appeared in both from and two columns of supplied edges.",
    fixed = TRUE
  )
})

test_that("Default node type can change", {
  edges <- dplyr::tribble(
    ~from, ~to,
    "a1"   , "b1"   ,
    "a1"   , "b2"   ,
    "a1"   , "b3"   ,
    "a2"   , "b1"   ,
    "b1"   , "b4"   ,
    "a3"   , "b1"
  )

  expect_equal(
    new_sbm_network(edges = edges, default_node_type = "my_node_type")$nodes$type[1],
    "my_node_type")
})


test_that("Model is instantiated if requested", {
  network <- sim_basic_block_network(
    n_blocks = 3,
    n_nodes_per_block = 40
  )

  sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)
  testthat::expect_false(is.null(sbm_net$model))
})

test_that("Model is not instantiated if requested", {
  network <- sim_basic_block_network(
    n_blocks = 3,
    n_nodes_per_block = 40
  )

  sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes, setup_model = FALSE)
  testthat::expect_true(is.null(sbm_net$model))
})


