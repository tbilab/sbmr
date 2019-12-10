#' Simulate simple network
#'
#' Generate an edge dataframe from an \href{https://en.wikipedia.org/wiki/Erd%C5%91s%E2%80%93R%C3%A9nyi_model}{Erdos-Renyi network.} An edge will occur
#' between two nodes with a uniform common probability.
#'
#' @param n_nodes Total number of unique nodes to simulate
#' @param prob_of_connection Probability of any given edges between two nodes
#'   occuring
#'
#' @return An edge dataframe with node ids for connections in the `from` and
#'   `to` columns
#' @export
#'
#' @examples
#' sim_simple_network(n_nodes = 4, prob_of_connection = 0.9)
sim_simple_network <- function(n_nodes = 5, prob_of_connection = 0.7){
  # First generate an array of node ids
  node_ids <- paste0("node_", 1:n_nodes)

  edges <- dplyr::filter(
    tidyr::expand_grid(
      from = node_ids,
      to = node_ids
    ),
    from != to)

  # Retain edges with probability as specified
  dplyr::filter(edges, rbinom(nrow(edges), 1, prob = prob_of_connection) == 1)
}
