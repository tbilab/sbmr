my_nodes <- dplyr::tribble(
  ~id, ~type,
  "a1", "a",
  "a2", "a",
  "b1", "b",
  "b2", "b"
)

my_edges <- dplyr::tribble(
  ~from, ~to,
  "a1", "b1",
  "a1", "b2",
  "a2", "b1"
)

test_that("Warning given when adding a node that has no connections", {

 node_too_many <- dplyr::bind_rows(my_nodes, c(id = "c1", type = "c"))

  expect_warning(
    create_sbm(my_edges, node_too_many),
    "Node(s) c1 are not seen in any of the edges",
    fixed = TRUE
  )
})


test_that("Error thrown when adding an edge with a non-declared node", {

  w_bad_edge <- dplyr::bind_rows(my_edges, c(from = "c1", to = "b1"))

  expect_error(
    create_sbm(w_bad_edge, my_nodes),
    "Passed nodes dataframe is missing node(s) c1 from edges",
    fixed = TRUE
  )
})

test_that("Proper state is reflected from full edge node declaration", {

  expected_state <- dplyr::tribble(
    ~id, ~type, ~parent,  ~level,
    "a1", "a",   "none",      0L,
    "a2", "a",   "none",      0L,
    "b1", "b",   "none",      0L,
    "b2", "b",   "none",      0L
  )

  my_sbm <- create_sbm(my_edges, my_nodes)

  expect_equal(expected_state, my_sbm$get_state())
})

