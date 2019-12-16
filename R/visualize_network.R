#' Visualize network stucture
#'
#' Creates a simple interactive network visualization that visually encodes node membership and type (if present) for a passed network.
#'
#' @inheritParams create_sbm
#' @param width Desired width for output widget.
#' @param height Desired height for output widget.
#'
#' @return NULL
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Simulate a 3 group/cluster network with 15 nodes per group
#' simulated_network <- sim_basic_block_network(n_groups = 3, n_nodes_per_group = 15)
#'
#' # Visualize network
#' visualize_network(simulated_network)
#'
visualize_network <- function(edges, nodes = NULL, width = NULL, height = NULL) {


  if(!is_list_class(edges)){
    data_for_d3 <- list(
      edges = edges$edges,
      nodes = nodes$nodes
    )
  } else {
    data_for_d3 <- edges
  }


  r2d3::r2d3(
    data = data_for_d3,
    script = system.file("d3/network_viz.js", package = "sbmR"),
    width = width,
    height = height
  )
}
