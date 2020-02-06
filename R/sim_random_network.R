#' Simulate completely random network
#'
#' Generate an edge dataframe from an \href{https://en.wikipedia.org/wiki/Erd%C5%91s%E2%80%93R%C3%A9nyi_model}{Erdos-Renyi network.} An edge will occur
#' between two nodes with a uniform common probability.
#'
#' @family simulations
#'
#' @param n_nodes Total number of unique nodes to simulate
#' @param prob_of_edge Probability of any given edges between two nodes
#'   occuring
#' @inheritParams sim_sbm_network
#'
#' @return A `sbm_network`/`list` that as `nodes` and `edges` member tibbles.
#' @export
#'
#' @examples
#'
#' # Basic random network
#' sim_random_network(n_nodes = 10, prob_of_edge = 0.5)
#'
#' # Simulate a fully connected network by setting probability of edge to 1
#' sim_random_network(n_nodes = 10, prob_of_edge = 1)
#'
sim_random_network <- function(n_nodes = 5,
                               prob_of_edge = 0.7,
                               allow_self_edges = FALSE,
                               setup_model = FALSE){

  sim_sbm_network(
    block_info = dplyr::tibble(
      block = "node",
      n_nodes = n_nodes
    ),
    edge_propensities = dplyr::tibble(
      block_1 = "node",
      block_2 = "node",
      propensity = prob_of_edge
    ),
    edge_dist = purrr::rbernoulli,
    allow_self_edges = allow_self_edges,
    setup_model = setup_model
  )
}
