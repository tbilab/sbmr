#' Verify model for sbm_network object exists
#'
#' Takes data from sbm_network object and checks S4 `SBM` class model object
#' exists and is usable. If it isn't it is created.
#'
#' @family helpers
#'
#' @param x Object of class `sbm_network`.
#' @param show_messages Should function inform of its actions such as when a
#'   model already exists so no changes are made?
#' @param warn_about_random_seed Should the model warn about cases when set seed
#'   is present for cached model?
#'
#' @inherit new_sbm_network return
#' @export
#'
#' @examples
#'
#' # Setup network without model initialized
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 20, setup_model = FALSE)
#'
#' net
#'
#' # Run verify model and the model should now be initialized
#' net <- net %>% verify_model()
#' net
#'
verify_model <- function(x, show_messages = FALSE, warn_about_random_seed = TRUE){
  UseMethod("verify_model")
}


#' @export
verify_model.sbm_network <- function(x, show_messages = FALSE, warn_about_random_seed = TRUE){
  has_model_already <- not_null(attr(x, 'model'))
  has_state_already <- not_null(attr(x, "state"))
  has_random_seed <- not_null(attr(x, 'random_seed'))

  if (has_model_already){
    model_is_stale <- tryCatch(
      error = function(err) TRUE,
      {
        attr(x, 'model')$get_state()
        FALSE
      })

    # If an error was triggered trying to access the object then we know
    # it is stale and needs to be rebuilt
    if(has_model_already & !model_is_stale){
      if(show_messages) message("Object already has model initialized. No changes made")
      return(x)
    }

    if(has_random_seed & warn_about_random_seed){
      warning("Random seed was specified but model is being restarted from a saved state.\nThis will harm reproducability if compared to uninterupted use of model.")
    }
  }

  if(has_random_seed){
    # Instantiate instance of sbm class with random seed
    sbm_model <- new(SBM, as.integer(attr(x, 'random_seed')))
  } else {
    # Instantiate instance of sbm class with no random seed
    sbm_model <- new(SBM)
  }


  # Fill in all the needed nodes
  # bind the integer types to nodes before sending them to model
  purrr::walk2(x$nodes$id, x$nodes$type, function(id, type){ sbm_model$add_node(id, type, 0L) })

  # If the model has a allowed node pairs list, let model know before adding edges
  allowed_pairs <- attr(x, 'edge_types')
  if(not_null(allowed_pairs)){
    sbm_model$add_edge_types(dplyr::pull(allowed_pairs, !!attr(x, "from_column")),
                             dplyr::pull(allowed_pairs, !!attr(x, "to_column")))
  }

  # Fill in the edges
  from_nodes <- dplyr::pull(x$edges, !!attr(x, "from_column"))
  to_nodes <- dplyr::pull(x$edges, !!attr(x, "to_column"))
  for(i in 1:attr(x, "n_edges")){
    sbm_model$add_edge(from_nodes[i],
                       to_nodes[i])
  }

  if (has_state_already) {
    if(show_messages) message("Reloading saved model state.")

    # Reload using saved state to get model back to working condition
    previous_state <- attr(x, "state")

    # Reload state using the s4 method for doing so exposed by rcpp
    sbm_model$set_state(previous_state$id,
                              previous_state$parent,
                              previous_state$level,
                              previous_state$type)
  } else {
    if(show_messages) message("New SBM model object initialized.")
    # Update object state with newly created state
    attr(x, "state") <- sbm_model$get_state()
  }

  # Assign sbm_model object to name model in sbm_network object
  attr(x, 'model') <- sbm_model

  # Give back sbm_network object
  x
}
