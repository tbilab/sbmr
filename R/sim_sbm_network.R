#' Simulate network using stochastic block model
#'
#' Simulates nodes and edges from a network where edge connections are derived
#' from the Stochastic Block Model. A dataframe describing the different groups
#' present and the number of nodes within them, a dataframe that provides the
#' `propensity` of connection between two edges, and a distribution function
#' who's main parameter the `propensity` value defines are needed.
#'
#' @param group_info A dataframe/tibble with two columns: `group`: the id of the
#'   group, and `n_nodes`: the number of nodes to simulate from that group.
#' @param connection_propensities A dataframe with 3 columns: `group_1`: the id
#'   of the from group, `group_2`: the id of the to group, and `propensity`: the
#'   parameter for `edge_dist` that controls if and or how many edges should
#'   occur between the pair of groups on average.
#' @param edge_dist A distribution function that has two inputs: the first is
#'   number of samples to draw and the second the `propensity` value from
#'   `connection_propensities`. For instance, the default value is `rpois` which
#'   takes two arguments: `n` and `lambda`. In this scenario the `propensity` of
#'   connections between two nodes is provided to `lambda` and thus is the
#'   average number of connections for each pair of nodes between two given
#'   groups.
#' @param allow_self_connections Should nodes be allowed to have connections to
#'   themselves?
#' @param keep_connection_counts Should the connection counts stay on returned
#'   edges? If edges distribution is a binary yes or no then you will likely
#'   want to set this to `TRUE`.
#'
#' @return A list with a `nodes` dataframe (containing a node's `id` and `group`
#'   membership) and a `edges` dataframe (containing `from` and `to` nodes along
#'   with the total number of `connections` as drawn from `edge_dist`.)
#' @export
#'
#' @examples
#' group_info <- dplyr::tribble(
#'   ~group, ~n_nodes,
#'      "a",       10,
#'      "b",       12,
#'      "c",       15
#' )
#'
#' connection_propensities <- dplyr::tribble(
#'  ~group_1, ~group_2, ~propensity,
#'       "a",      "a",         0.7,
#'       "a",      "b",         0.2,
#'       "a",      "c",         0.1,
#'       "b",      "b",         0.9,
#'       "b",      "c",         0.4,
#'       "c",      "c",         0.4,
#' )
#'
#' sim_sbm_network(group_info, connection_propensities, edge_dist = purrr::rbernoulli)
#'
sim_sbm_network <- function(
  group_info,
  connection_propensities,
  edge_dist = rpois,
  allow_self_connections = FALSE,
  keep_connection_counts = TRUE){

  # Generate all the node names and their groups
  nodes <- purrr::map2_dfr(
    group_info$group,
    group_info$n_nodes,
    ~dplyr::tibble(
      id = paste0(.x, "_", 1:.y),
      group = .x
    )
  )

  # Collapses two rows of groups into a sorted single string to make joining
  # edges with their respective propensity easier due to order not mattering
  sorted_group_collapse <- function(group_1, group_2){
    purrr::map2_chr(
      group_1,
      group_2,
      ~paste(sort(c(.x, .y)), collapse = "-")
   )
  }

  # Make dataframe of all the node name combinations and join with propensities
  # to get propensity of each given connection. Then draw from provided
  # distribution and return edge list with pairs of nodes returned if the draw
  # returned greater than 0 connections

  # Get all possible combinations of node-node pairs by their indices in
  # the nodes dataframe...
  edge_pairs_inds <- get_combination_indices(length(nodes$id), repeats = allow_self_connections)

  # Extract indices from combination matrix and use to build an edge tibble
  node_1_inds <- edge_pairs_inds$a
  node_2_inds <- edge_pairs_inds$b

  edges <- dplyr::tibble(
      node_1 = nodes$id[node_1_inds],
      node_2 = nodes$id[node_2_inds],
      groups = sorted_group_collapse(nodes$group[node_1_inds],
                                     nodes$group[node_2_inds])
    ) %>%
    dplyr::full_join(
      dplyr::transmute(
        connection_propensities,
        groups = sorted_group_collapse(group_1, group_2),
        propensity = propensity
      ),
      by = "groups"
    ) %>%
    dplyr::filter(!is.na(propensity)) %>% # Remove node combos that were not specified
    dplyr::mutate(connections = as.numeric(edge_dist(dplyr::n(), propensity))) %>%
    dplyr::filter(connections > 0) %>%
    dplyr::select(
      from = node_1,
      to = node_2,
      connections
    )

  if (!keep_connection_counts){
    edges <- edges %>%
      dplyr::select(-connections)
  }

  list(nodes = nodes, edges = edges)
}
