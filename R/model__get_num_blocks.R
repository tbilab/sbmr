#' Get number of blocks currently in model
#'
#' Returns the number of blocks/clusters in the current model state.
#'
#' @family modeling
#' @inheritParams verify_model
#'
#' @return Number of clusters in current model state
#' @export
#'
#' @examples
#'
#' # Small edge list to build network
#' edges <- dplyr::tribble(
#'   ~from, ~to,
#'   "a1"   , "b1"   ,
#'   "a1"   , "b2"   ,
#'   "a1"   , "b3"   ,
#'   "a2"   , "b1"   ,
#'   "a2"   , "b4"   ,
#'   "a3"   , "b1"
#' )
#'
#' # A small simulated network with no blocks
#' net <- new_sbm_network(edges)
#'
#' get_num_blocks(net)
#'
#' # Add some two random blocks
#' net %>% initialize_blocks(2) %>% get_num_blocks()
#'
#' # Add a block per node
#' net %>% initialize_blocks() %>% get_num_blocks()
#'
get_num_blocks <- function(sbm){
  UseMethod("get_num_blocks")
}

get_num_blocks.default <- function(sbm){
  cat("get_num_blocks generic")
}

#' @export
get_num_blocks.sbm_network <- function(sbm){

  model <- verify_model(sbm) %>%
    attr('model')

  if(model$n_levels() > 1) {
    return(model$n_nodes_at_level(1))
  } else {
    return(0)
  }
}


utils::globalVariables(c("level"))
