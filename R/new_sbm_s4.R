#' Build new SBM s4 class
#'
#' Builds an s4 class exported by Rcpp modules. Generally used internally. For making the main `sbm_network` class, use \code{\link{new_sbm_network}} instead.
#'
#' @inheritParams new_sbm_network
#' @inheritParams update_state
#' @param node_types Character vector with unique node types in network. If not provided, this will be deduced from present types in `nodes`.
#'
#' @return An SBM S4 class
#' @export
#'
#' @examples
#'
#' nodes <- dplyr::tribble(~id,     ~type,
#' "a1",    "a",
#' "a2",    "a",
#' "a3",    "a",
#' "b1",    "b",
#' "b2",    "b",
#' "b3",    "b",
#' "b4",    "b")
#'
#' node_types <- c("a", "b")
#'
#' edges <- dplyr::tribble(~a,     ~b,
#'                         "a1",    "b1",
#'                         "a1",    "b2",
#'                         "a1",    "b3",
#'                         "a2",    "b1",
#'                         "a2",    "b4",
#'                         "a3",    "b1")
#'
#' random_seed <- 42
#' new_sbm_s4(nodes = nodes,
#'            edges = edges,
#'            node_types = node_types,
#'            random_seed = 42)
#'
new_sbm_s4 <- function(nodes,
                       edges,
                       allowed_edge_types = NULL,
                       node_types = NULL,
                       random_seed = NULL,
                       state = NULL){

  # I use NULL to represent unpassed values because that's how they will be given from the s3 class.
  if(is.null(allowed_edge_types)){
    allowed_edge_types <- dplyr::tibble(a = character(), b = character())
  }

  if(is.null(node_types)){
    node_types <- unique(nodes$type)
  }

  if(is.null(random_seed)){
    random_seed <- ceiling(runif(1, 0, 1e6))
  }

  # Load network model with nodes and random seed
  sbm_model <- methods::new(SBM,
                            nodes$id,
                            nodes$type,
                            node_types,
                            random_seed)

  # Fill in the edges for model
  sbm_model$add_edges(edges$a,
                      edges$b,
                      allowed_edge_types$a,
                      allowed_edge_types$b)

  if(!is.null(state)){
    sbm_model$update_state(state$id,
                           state$type,
                           state$parent,
                           state$level)
  }

  sbm_model
}
