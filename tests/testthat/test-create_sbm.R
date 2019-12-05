test_that("Errors and warnings for malformed data work", {
  my_edges <- dplyr::tribble(
    ~from, ~to,
    "a1", "b1",
    "a1", "b2",
    "a1", "b3",
    "a2", "b3",
    "a3", "b2"
  )

  my_nodes <- dplyr::tribble(
    ~id, ~type,
    "a1", "a",
    "a2", "a",
    "a3", "a",
    "b1", "b",
    "b2", "b",
    "c1", "c",
    "c2", "c",
    "b3", "b"
  )

  expect_warning(
    SBM(my_edges, my_nodes),
    "Node(s) c1, c2 are not seen in any of the edges",
    fixed = TRUE
  )
})
