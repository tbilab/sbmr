#' Visualize Network
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
