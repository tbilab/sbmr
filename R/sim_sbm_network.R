#' Simulate network using stochastic block model
#'
#' Simulates nodes and edges from a network where edge edges are derived
#' from the Stochastic Block Model. A dataframe describing the different blocks
#' present and the number of nodes within them, a dataframe that provides the
#' `propensity` of edge between two edges, and a distribution function
#' who's main parameter the `propensity` value defines are needed.
#'
#' @param block_info A dataframe/tibble with two columns: `block`: the id of the
#'   block, and `n_nodes`: the number of nodes to simulate from that block.
#' @param edge_propensities A dataframe with 3 columns: `block_1`: the id
#'   of the from block, `block_2`: the id of the to block, and `propensity`: the
#'   parameter for `edge_dist` that controls if and or how many edges should
#'   occur between the pair of blocks on average.
#' @param edge_dist A distribution function that has two inputs: the first is
#'   number of samples to draw and the second the `propensity` value from
#'   `edge_propensities`. For instance, the default value is `rpois` which
#'   takes two arguments: `n` and `lambda`. In this scenario the `propensity` of
#'   edges between two nodes is provided to `lambda` and thus is the
#'   average number of edges for each pair of nodes between two given
#'   blocks.
#' @param allow_self_edges Should nodes be allowed to have edges to
#'   themselves?
#' @param keep_edge_counts Should the edge counts stay on returned
#'   edges? If edges distribution is a binary yes or no then you will likely
#'   want to set this to `TRUE`.
#'
#' @return A list with a `nodes` dataframe (containing a node's `id` and `block`
#'   membership) and a `edges` dataframe (containing `from` and `to` nodes along
#'   with the total number of `edges` as drawn from `edge_dist`.)
#' @export
#'
#' @examples
#' block_info <- dplyr::tribble(
#'   ~block, ~n_nodes,
#'      "a",       10,
#'      "b",       12,
#'      "c",       15
#' )
#'
#' edge_propensities <- dplyr::tribble(
#'  ~block_1, ~block_2, ~propensity,
#'       "a",      "a",         0.7,
#'       "a",      "b",         0.2,
#'       "a",      "c",         0.1,
#'       "b",      "b",         0.9,
#'       "b",      "c",         0.4,
#'       "c",      "c",         0.4,
#' )
#'
#' sim_sbm_network(block_info, edge_propensities, edge_dist = purrr::rbernoulli)
#'
sim_sbm_network <- function(
  block_info,
  edge_propensities,
  edge_dist = rpois,
  allow_self_edges = FALSE,
  keep_edge_counts = TRUE){

  # Generate all the node names and their blocks
  nodes <- purrr::map2_dfr(
    block_info$block,
    block_info$n_nodes,
    ~dplyr::tibble(
      id = paste0(.x, "_", 1:.y),
      block = .x
    )
  )

  # Collapses two rows of blocks into a sorted single string to make joining
  # edges with their respective propensity easier due to order not mattering
  sorted_block_collapse <- function(block_1, block_2){
    purrr::map2_chr(
      block_1,
      block_2,
      ~paste(sort(c(.x, .y)), collapse = "-")
   )
  }

  # Make dataframe of all the node name combinations and join with propensities
  # to get propensity of each given edge. Then draw from provided
  # distribution and return edge list with pairs of nodes returned if the draw
  # returned greater than 0 edges

  # Get all possible combinations of node-node pairs by their indices in
  # the nodes dataframe...
  edge_pairs_inds <- get_combination_indices(length(nodes$id), repeats = allow_self_edges)

  # Extract indices from combination matrix and use to build an edge tibble
  node_1_inds <- edge_pairs_inds$a
  node_2_inds <- edge_pairs_inds$b

  edges <- dplyr::tibble(
      node_1 = nodes$id[node_1_inds],
      node_2 = nodes$id[node_2_inds],
      blocks = sorted_block_collapse(nodes$block[node_1_inds],
                                     nodes$block[node_2_inds])
    ) %>%
    dplyr::full_join(
      dplyr::transmute(
        edge_propensities,
        blocks = sorted_block_collapse(block_1, block_2),
        propensity = propensity
      ),
      by = "blocks"
    ) %>%
    dplyr::filter(!is.na(propensity)) %>% # Remove node combos that were not specified
    dplyr::mutate(edges = as.numeric(edge_dist(dplyr::n(), propensity))) %>%
    dplyr::filter(edges > 0) %>%
    dplyr::select(
      from = node_1,
      to = node_2,
      edges
    )

  if (!keep_edge_counts){
    edges <- edges %>%
      dplyr::select(-edges)
  }

  list(nodes = nodes, edges = edges)
}
