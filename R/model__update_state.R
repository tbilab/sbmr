#' Update SBM model state
#'
#' Updates the state of the internal S4 class along with the state holder of the s3 class.
#'
#' @family advanced
#'
#' @inheritParams verify_model
#' @param state_df A state dataframe with `id`, `parent`, `level`, and `type` columns for all nodes in network (along with block nodes).
#'
#' @inherit new_sbm_network return
#' @export
#'
#' @examples
#'
#' edges <- dplyr::tribble(
#'   ~from,   ~to,
#'   "a1",  "b1",
#'   "a1",  "b2",
#'   "a1",  "b3",
#'   "a2",  "b1",
#'   "a2",  "b4",
#'   "a3",  "b1"
#' )
#'
#' nodes <- dplyr::tribble(
#'   ~id,   ~type,
#'   "a1", "node",
#'   "a2", "node",
#'   "a3", "node",
#'   "b1", "node",
#'   "b2", "node",
#'   "b3", "node",
#'   "b4", "node"
#' )
#'
#' new_state <- dplyr::tribble(
#'   ~id,          ~parent,   ~type,  ~level,
#'   "a1",       "a_parent",  "node",      0L,
#'   "a2",       "a_parent",  "node",      0L,
#'   "a3",       "a_parent",  "node",      0L,
#'   "b1",       "b_parent",  "node",      0L,
#'   "b2",       "b_parent",  "node",      0L,
#'   "b3",       "b_parent",  "node",      0L,
#'   "b4",       "b_parent",  "node",      0L,
#'   "a_parent", "none",      "node",      1L,
#'   "b_parent", "none",      "node",      1L,
#' )
#'
#' net <- new_sbm_network(edges = edges, nodes = nodes)
#'
#' # before updating state the state should not equal the desired new state
#' dplyr::all_equal(new_state, attr(net, 'state'))
#' dplyr::all_equal(new_state, attr(net, 'model')$get_state())
#'
#' # Update state using update_state() method
#' net <- net %>% update_state(new_state)
#'
#' # Now the states should be identical
#' dplyr::all_equal(new_state, attr(net, 'state'))
#' dplyr::all_equal(new_state, attr(net, 'model')$get_state())
#'
update_state <- function(sbm, state_df){

  UseMethod("update_state")
}

update_state.default <- function(sbm, state_df){
  cat("update_state generic")
}

#' @export
update_state.sbm_network <- function(sbm, state_df){
  attr(sbm, 'state') <- state_df

  attr(sbm, 'model')$update_state(state_df$id,
                                  state_df$type,
                                  state_df$parent,
                                  state_df$level)

  sbm
}
