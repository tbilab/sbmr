#' Assign blocks for all nodes
#'
#' Initializes blocks for a given level of network. When `n_blocks = -1` (the
#' default) this will simply give every node its own block. Otherwise it will
#' randomly draw each nodes block from `num_block` possible parent nodes for
#' each node type. E.g. if network is bipartite and `n_blocks == 3` a total of
#' `3*2 = 6` blocks will be added and assigned randomly to nodes. Because of
#' random assignment of parents there may sometimes be less blocks than
#' `n_blocks * <num_types>` due to some block nodes never being assigned to a
#' child node.
#'
#' @family advanced
#'
#' @inheritParams add_node
#' @param n_blocks Total number of blocks to initialize for each unique node
#'   type. If set to `-1`, then a single block is made for every node.
#'
#'
#' @inherit new_sbm_network return
#'
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
#' # A small simulated network
#' net <- new_sbm_network(edges)
#'
#' # Default values of function will give every node its own block
#' net %>%
#'   initialize_blocks() %>%
#'   n_blocks()
#'
#' # You can also decide to have a given number of blocks randomly assigned Here
#' # four blocks result because two random blocks are made for each of the two
#' # types
#' net %>%
#'   initialize_blocks(n_blocks = 2) %>%
#'   n_blocks()
#'
#' # If you have a polypartite network each set of node types will get their own
#' # set of `n_blocks` randomly assigned
#' new_sbm_network(edges, bipartite_edges = TRUE) %>%
#'   initialize_blocks(n_blocks = 2) %>%
#'   n_blocks()
#'
#'
initialize_blocks <- function(sbm, n_blocks = NULL){
  UseMethod("initialize_blocks")
}

initialize_blocks.default <- function(sbm, n_blocks = NULL){
  cat("Block initialization generic.")
}

#' @export
initialize_blocks.sbm_network <- function(sbm, n_blocks = NULL){
  # Make sure we have an SBM object to work with
  sbm <- verify_model(sbm)

  one_block_per_node <- is.null(n_blocks)

  # Check all the possible error conditions
  if(one_block_per_node){
    n_blocks <- -1
  } else if(n_blocks < 1){
    stop(glue::glue("Can't initialize {n_blocks} blocks."))
  }

  # Send message to RCPP class to initialize proper number of nodes
  attr(sbm, 'model')$initialize_blocks(as.integer(n_blocks))

  # Update state attribute
  attr(sbm, "state") <- attr(sbm, 'model')$state()

  sbm
}
