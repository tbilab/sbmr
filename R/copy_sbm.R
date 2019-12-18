#' Copy existing SBM object
#'
#' @param old_sbm Old SBM created with \link{\code{create_sbm}}
#' @param match_state Should copy also match the current grouping state of old model?
#'
#' @return A new deep copy of the original SBM with parameters preserved.
#' @export
#'
#' @examples
copy_sbm <- function(old_sbm, match_state = FALSE){

  old_sbm_state <- old_sbm$get_state()

  old_nodes <- dplyr::filter(old_sbm_state, level == 0)
  has_nodes <- nrow(old_nodes) > 0
  old_edges <- old_sbm$get_edges()
  has_edges <- nrow(old_edges) > 0

  # Make a new copy by exporting nodes and edges from old
  new_sbm <- create_sbm(
    nodes = if(has_nodes) old_nodes else NULL,
    edges = if(has_edges) old_edges else NULL,
    eps = old_sbm$EPS
  )

  # Set free parameters
  new_sbm$BETA <- old_sbm$BETA
  new_sbm$SIGMA <- old_sbm$SIGMA
  new_sbm$N_CHECKS_PER_GROUP <- old_sbm$N_CHECKS_PER_GROUP
  new_sbm$GREEDY <- old_sbm$GREEDY

  # Update state of new model if needed
  if(match_state){
    load_from_state(new_sbm, old_sbm_state)
  }

  new_sbm
}
