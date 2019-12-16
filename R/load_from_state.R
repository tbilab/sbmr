#' Load from state
#'
#' Takes model state export as given by SBM$get_state and returns model to
#' specified state. This is useful for resetting model before running various
#' algorithms such as agglomerative merging.
#'
#' @inheritParams add_node
#' @param state SBM state dump dataframe as provided by \code{\link{get_state}}.
#'
#' @return SBM model object modified with state modified to matched passed state
#'   dump. \emph{Note that object is modified in place as well.}
#'
#' @export
#'
#' @examples
#'
#' # A state dump dataframe as one would get from sbmR::get_state()
#' desired_state <- dplyr::tribble(
#'   ~id,  ~parent, ~type, ~level,
#'   "a1",   "a11",   "a",      0,
#'   "b1",   "b11",   "b",      0,
#'  "a11",  "none",   "a",      1,
#'  "b11",  "none",   "b",      1
#' )
#'
#' # Create simple network with no group nodes
#' my_sbm <- create_sbm() %>%
#'   add_node('a1', type = 'a') %>%
#'   add_node('b1', type = 'b')
#'
#' # Load our desired state
#' my_sbm %>% load_from_state(desired_state)
#' # Group nodes have been created and assigned to proper children nodes
#'
load_from_state <- function(sbm, state){
  sbm$load_from_state(
    state$id,
    state$parent,
    state$level,
    state$type)
  sbm
}

