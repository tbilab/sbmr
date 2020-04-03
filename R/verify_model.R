#' Verify model for sbm_network object exists
#'
#' Takes data from sbm_network object and checks S4 `SBM` class model object
#' exists and is usable. If it isn't it is created.
#'
#' @family helpers
#'
#' @param sbm Object of class `sbm_network`.
#' @param show_messages Should function inform of its actions such as when a
#'   model already exists so no changes are made?
#' @param warn_about_random_seed Should the model warn about cases when set seed
#'   is present for cached model?
#'
#' @inherit new_sbm_network return
#'
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
verify_model <- function(sbm, show_messages = FALSE, warn_about_random_seed = TRUE){
  UseMethod("verify_model")
}


#' @export
verify_model.sbm_network <- function(sbm, show_messages = FALSE, warn_about_random_seed = TRUE){
  has_model_already <- not_null(attr(sbm, 'model'))
  has_state_already <- not_null(attr(sbm, "state"))

  if (has_model_already){
    model_is_stale <- tryCatch(
      error = function(err) TRUE,
      {
        attr(sbm, 'model')$get_state()
        FALSE
      })

    # If an error was triggered trying to access the object then we know
    # it is stale and needs to be rebuilt
    if(has_model_already & !model_is_stale){
      if(show_messages) message("Object already has model initialized. No changes made")
      return(sbm)
    }

    if(warn_about_random_seed){
      warning("Random seed was specified but model is being restarted from a saved state.\nThis will harm reproducability if compared to uninterupted use of model.")
    }
  }


  # Load network model with nodes and random seed
  sbm_model <- methods::new(SBM,
                            sbm$nodes$id,
                            sbm$nodes$type,
                            attr(sbm, "node_types"),
                            attr(sbm, "random_seed"))

  # Fill in the edges for model
  sbm_model$add_edges(dplyr::pull(sbm$edges, !!attr(sbm, "from_column")),
                      dplyr::pull(sbm$edges, !!attr(sbm, "to_column")),
                      attr(sbm, 'edge_types')$from,
                      attr(sbm, 'edge_types')$to)

  # If model has cached block structure, reload it here
  if (has_state_already) {
    if(show_messages) message("Reloading saved model state.")

    # Reload using saved state to get model back to working condition
    previous_state <- attr(sbm, "state")

    # Reload state using the s4 method for doing so exposed by rcpp
    sbm_model$update_state(previous_state$id,
                           previous_state$type,
                           previous_state$parent,
                           previous_state$level)
  }

  # Assign sbm_model object to name model in sbm_network object
  attr(sbm, 'model') <- sbm_model

  # Give back sbm_network object
  sbm
}
