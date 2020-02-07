#' Assign blocks for all nodes
#'
#' Initializes blocks for a given level of network. When `num_blocks = -1` (the
#' default) this will simply give every node its own block. Otherwise it will
#' randomly draw each nodes block from `num_block` possible parent nodes for
#' each node type. E.g. if network is bipartite and `num_blocks == 3` a total of
#' `3*2 = 6` blocks will be added and assigned randomly to nodes. Because of
#' random assignment of parents there may sometimes be less blocks than
#' `num_blocks * <num_types>` due to some block nodes never being assigned to a
#' child node.
#'
#' @family advanced
#'
#' @inheritParams set_node_parent
#' @param num_blocks Total number of blocks to initialize for each unique node
#'   type. If set to `-1`, then a single block is made for every node.
#'
#' @return SBM model object modified with new block nodes and node hierarchy
#'   modified. \emph{Note that object is modified in place as well.}
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
#'   get_num_blocks()
#'
#' # You can also decide to have a given number of blocks randomly assigned Here
#' # four blocks result because two random blocks are made for each of the two
#' # types
#' net %>%
#'   initialize_blocks(num_blocks = 2) %>%
#'   get_num_blocks()
#'
#' # If you have a polypartite network each set of node types will get their own
#' # set of `num_blocks` randomly assigned
#' new_sbm_network(edges, bipartite_edges = TRUE) %>%
#'   initialize_blocks(num_blocks = 2) %>%
#'   get_num_blocks()
#'
#'
initialize_blocks <- function(sbm, num_blocks = NULL, level = 0){
  UseMethod("initialize_blocks")
}

initialize_blocks.default <- function(sbm, num_blocks = NULL, level = 0){
  cat("Block initialization generic.")
}

#' @export
initialize_blocks.sbm_network <- function(sbm, num_blocks = NULL, level = 0){
  # Make sure we have an SBM object to work with
  sbm <- verify_model(sbm)

  one_block_per_node <- is.null(num_blocks)
  num_nodes_at_level <- sum(attr(sbm, 'state')$level == level)

  # Check all the possible error conditions
  if(one_block_per_node){
    num_blocks <- -1
  } else
  if(num_blocks < 1){
    stop(glue::glue("Can't initialize {num_blocks} blocks."))
  } else
  if(num_blocks > num_nodes_at_level){
    stop(glue::glue("Network only has {num_nodes_at_level} nodes at level {level}. ",
                    "Can't initialize {num_blocks} blocks."))
  }

  # Send message to RCPP class to initialize proper number of nodes
  attr(sbm, 'model')$initialize_blocks(as.integer(num_blocks), as.integer(level))

  # Update state attribute
  attr(sbm, "state") <- attr(sbm, 'model')$get_state()

  sbm
}
