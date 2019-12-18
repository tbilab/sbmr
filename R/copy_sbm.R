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

  # Make a new copy by exporting nodes and edges from old
  new_sbm <- create_sbm(
    nodes = dplyr::filter(old_sbm_state, level == 0),
    edges = old_sbm$get_edges(),
    eps = old_sbm$EPS
  )

  # Update state of new model if needed
  if(match_state){
    load_from_state(new_sbm, old_sbm_state)
  }

  new_sbm
}
