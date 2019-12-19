#' Visualize network stucture
#'
#' Creates a simple interactive network visualization that visually encodes node membership and type (if present) for a passed network.
#'
#' @inheritParams create_sbm
#' @param width Desired width for output widget.
#' @param height Desired height for output widget.
#' @param node_color_col Name of column in the `nodes` dataframe that should be encoded with color of nodes in the plot
#' @param node_shape_col Name of column in the `nodes` dataframe that should be encoded with shape of nodes in the plot
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
#' # Visualize network with color encoding the groups
#' visualize_network(simulated_network)
#'
#' # Visualize network with shape encoding the groups
#' visualize_network(simulated_network, node_shape_col = 'group', node_color_col = 'type')
#'
visualize_network <- function(edges, nodes = NULL, width = "100%", height = NULL, node_color_col = 'group', node_shape_col = 'type') {

  # CHeck if we were passed a list of edges and nodes or the edges and nodes seperately
  if(is_list_class(edges)){
    data_for_d3 <- edges
  } else {
    data_for_d3 <- list(
      edges = edges,
      nodes = nodes
    )
  }

  r2d3::r2d3(
    data = data_for_d3,
    options = list(
      color_col = node_color_col,
      shape_col = node_shape_col
    ),
    container = 'div',
    script = system.file("d3/network_viz.js", package = "sbmR"),
    width = width,
    height = height
  )
}
