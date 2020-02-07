#' Update SBM model state
#'
#' Updates the state of the internal S4 class along with the state holder of the s3 class.
#'
#' @family advanced
#'
#' @inheritParams verify_model
#' @param state_df A state dataframe with `id`, `parent`, `level`, and `type` columns for all nodes in network (along with block nodes).
#'
#' @return Updated `sbm_network` object
#' @export
#'
#' @examples
update_state <- function(sbm, state_df){

  UseMethod("update_state")
}

update_state.default <- function(sbm, state_df){
  cat("update_state generic")
}

#' @export
update_state.sbm_network <- function(sbm, state_df){

  attr(sbm, 'state') <- state_df

  sbm$model$load_from_state(state_df$id,
                            state_df$parent,
                            state_df$level,
                            state_df$type)

  sbm
}
