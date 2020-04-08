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

  # Assign sbm_model object to name model in sbm_network object
  attr(sbm, 'model') <- new_sbm_s4(nodes = sbm$nodes,
                                   edges = dplyr::rename(sbm$edges, a = !!attr(sbm, "from_column"), b = !!attr(sbm, "to_column")),
                                   node_types = attr(sbm, "node_types"),
                                   allowed_edge_types = attr(sbm, "allowed_edge_types"),
                                   state =  attr(sbm, "state"),
                                   random_seed = attr(sbm, "random_seed"))

  # Give back sbm_network object
  sbm
}
