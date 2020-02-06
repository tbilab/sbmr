#' Visualize network stucture
#'
#' Creates a simple interactive network visualization that visually encodes node
#' membership and type (if present) for a passed network.
#'
#' @inheritParams verify_model
#' @param width Desired width for output widget.
#' @param height Desired height for output widget.
#' @param node_color_col Name of column in the `nodes` dataframe that should be
#'   encoded with color of nodes in the plot. If this value is numeric (defined
#'   as more than 10 unique values that are numbers) the network will color
#'   nodes on a linear scale from the min value to the highest value.
#' @param node_shape_col Name of column in the `nodes` dataframe that should be
#'   encoded with shape of nodes in the plot
#'
#' @return NULL
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Simulate a 3 block/cluster network with 15 nodes per block
#' simulated_network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 15)
#'
#' # Visualize network with color encoding the blocks
#' visualize_network(simulated_network)
#'
#' # Visualize network with shape encoding the blocks
#' visualize_network(simulated_network, node_shape_col = 'block', node_color_col = 'type')
#'
visualize_network <- function(sbm,
                              node_color_col = 'block',
                              node_shape_col = 'type',
                              width = "100%",
                              height = NULL){
  UseMethod("visualize_network")
}

visualize_network.default <- function(sbm,
                                      node_color_col = 'block',
                                      node_shape_col = 'type',
                                      width = "100%",
                                      height = NULL){
  cat("visualize_network generic")
}

#' @export
visualize_network.sbm_network <- function(sbm,
                                          node_color_col = 'block',
                                          node_shape_col = 'type',
                                          width = "100%",
                                          height = NULL){

  r2d3::r2d3(
    data = list(
      edges = sbm$edges,
      nodes = sbm$nodes
    ),
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

