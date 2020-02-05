#' Initialize model for sbm_network object
#'
#' Takes data from sbm_network object and uses it to initialize an `SBM` class
#' model object.
#'
#' @param x Object of class `sbm_network`.
#' @param warn_if_overwriting Should function warn if it is initializing a model
#'   over an existing one?
#'
#' @return
#' @export
#'
#' @examples
#' network <- sim_basic_block_network(
#'   n_blocks = 3,
#'   n_nodes_per_block = 40
#' )
#'
#' sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)
#' sbm_net
#'
#' sbm_net <- sbm_net %>% initialize_model()
#' sbm_net
#'
initialize_model <- function(x, warn_if_overwriting = TRUE){
  UseMethod("initialize_model")
}

initialize_model.default <- function(x){
  cat("Default initialize model generic.")
}

#' @export
initialize_model.sbm_network <- function(x, warn_if_overwriting = TRUE){

  has_model_already <- !is.null(x$model)
  if (warn_if_overwriting & has_model_already) {
    warning("Object already has model object. Overwriting.")
  }

  # Instantiate instance of sbm class
  sbm <- new(SBM)

  # Fill in all the needed nodes
  for(i in 1:attr(x, "n_nodes")){
    sbm$add_node(x$nodes$id[i], x$nodes$type[i], 0L)
  }

  # Fill in the edges
  from_nodes <- dplyr::pull(x$edges, !!attr(x, "from_column"))
  to_nodes <- dplyr::pull(x$edges, !!attr(x, "to_column"))
  for(i in 1:attr(x, "n_edges")){
    sbm$add_edge(
      from_nodes[i],
      to_nodes[i]
    )
  }

  # Assign sbm object to name model in sbm_network object
  x$model <- sbm

  # Give back sbm_network object
  x
}
