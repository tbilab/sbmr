#' Copy existing SBM object
#'
#' @param old_sbm Old SBM created with \link{\code{create_sbm}}
#' @param match_state Should copy also match the current blocking state of old model?
#'
#' @return A new deep copy of the original SBM.
#' @export
#'
#' @examples
#'
#' # Generate a random network to build initial sbm with
#' network <- sim_random_network(n_nodes = 10)
#'
#' # Generate initial sbm
#' initial_sbm <- create_sbm(network)
#'
#' # Copy initial sbm
#' copied_sbm <- copy_sbm(initial_sbm)
#'
#' # Make sure that the state output matches (minus the parent info)
#' remove_parent <- . %>% dplyr::select(-parent)
#'
#' # Two sbm states will be the same
#' all.equal(get_state(copied_sbm),
#'           get_state(initial_sbm))
#'
copy_sbm <- function(old_sbm, match_state = FALSE){

  old_sbm_state <- old_sbm$get_state()

  old_nodes <- dplyr::filter(old_sbm_state, level == 0)
  has_nodes <- nrow(old_nodes) > 0
  old_edges <- old_sbm$get_edges()
  has_edges <- nrow(old_edges) > 0

  # Make a new copy by exporting nodes and edges from old
  new_sbm <- create_sbm(
    nodes = if(has_nodes) old_nodes else NULL,
    edges = if(has_edges) old_edges else NULL
  )

  # Update state of new model if needed
  if(match_state){
    load_from_state(new_sbm, old_sbm_state)
  }

  new_sbm
}
