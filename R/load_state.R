#' Load model state
#'
#' Takes a state dump dataframe and updates the passed model to have that same structure.
#'
#' @param sbm SBM model object
#' @param state_dump Dataframe returned from `get_state(sbm)`
#'
#' @return
#' @export
#'
#' @examples
load_state <- function(sbm, state_dump){
  sbm$load_from_state(
    state_dump$id,
    state_dump$parent,
    state_dump$level,
    state_dump$type)
}
