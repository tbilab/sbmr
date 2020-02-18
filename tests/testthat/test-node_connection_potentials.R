test_that("Error thrown when stated connection potentials are violated", {
  edges_tripartite <- dplyr::tribble(
    ~from,  ~to,
     "a1", "b1",
     "a1", "b2",
     "a2", "b1",
     "a1", "c1",
     "a2", "c1",
     "a2", "c2",
     "b1", "c1" # the bad connection
  )

  nodes_tripartite <- dplyr::tribble(
    ~id, ~type,
    "a1", "a",
    "a2", "a",
    "b1", "b",
    "b2", "b",
    "c1", "c",
    "c2", "c"
  )

  # a nodes can connect to b and c but b and c can not connect
  edge_types <- dplyr::tribble(
    ~from,  ~to,
      "a", "b",
      "a", "c"
  )

  expect_error(
    new_sbm_network(edges = edges_tripartite,
                    nodes = nodes_tripartite,
                    edge_types = edge_types)
  )

})
