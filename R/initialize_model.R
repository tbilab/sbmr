#' Initialize model for sbm_network object
#'
#' Takes data from sbm_network object and uses it to initialize an `SBM` class
#' model object.
#'
#' @param x Object of class `sbm_network`.
#' @param show_messages Should function inform of its actions such as when a
#'   model already exists so no changes are made?
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
initialize_model <- function(x, show_messages = TRUE){
  UseMethod("initialize_model")
}

initialize_model.default <- function(x){
  cat("Default initialize model generic.")
}

#' @export
initialize_model.sbm_network <- function(x, show_messages = TRUE){

  has_model_already <- !is.null(x$model)
  has_state_already <- !is.null(attr(x, "state"))

  if (has_model_already){
    model_is_stale <- tryCatch(
      error = function(err) TRUE,
      {
        x$model$get_state()
        FALSE
      })

    # If an error was triggered trying to access the object then we know
    # it is stale and needs to be rebuilt
    if(has_model_already & !model_is_stale){
      if(show_messages) message("Object already has model initialized. No changes made")
      return(x)
    }
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

  if (has_state_already) {
    if(show_messages) message("Reloading saved model state.")

    # Reload using saved state to get model back to working condition
    previous_state <- attr(x, "state")

    # Reload state using the s4 method for doing so exposed by rcpp
    sbm$load_from_state(previous_state$id,
                        previous_state$parent,
                        previous_state$level,
                        previous_state$type)
  } else {
    if(show_messages) message("New SBM model object initialized.")
    # Update object state with newly created state
    attr(x, "state") <- sbm$get_state()
  }

  # Assign sbm object to name model in sbm_network object
  x$model <- sbm

  # Give back sbm_network object
  x
}
