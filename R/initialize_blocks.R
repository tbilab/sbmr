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
#' @inheritParams set_node_parent
#' @param num_blocks Total number of blocks to initialize for each unique node
#'   type. If set to `-1`, then a single block is made for every node.
#'
#' @return SBM model object modified with new block nodes and node hierarchy
#'   modified. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#' # Helper function to get number of blocks in a model
#' get_num_blocks <- function(sbm){sum(get_state(sbm)$level == 1)}
#'
#' # Initialize a simple bipartite network with 6 total nodes (3 of each type)
#' my_nodes <- dplyr::tibble(
#'   id = c("a1", "a2", "a3", "b1", "b2", "b3"),
#'   type = c("a", "a", "a", "b", "b", "b")
#' )
#' my_sbm <- create_sbm(nodes = my_nodes)
#'
#' # Default values of function will give every node its own block
#' my_sbm %>% initialize_blocks()
#' get_num_blocks(my_sbm)
#'
#' # You can also decide to have a given number of blocks randomly assigned Here
#' # four blocks result because two random blocks are made for each of the two
#' # types
#' my_sbm %>% initialize_blocks(num_blocks = 2)
#' get_num_blocks(my_sbm)
#'
initialize_blocks <- function(sbm, num_blocks = -1, level = 0){
  UseMethod("initialize_blocks")
}

initialize_blocks.default <- function(sbm, num_blocks = -1, level = 0){
  cat("Block initialization generic.")
}

#' @export
initialize_blocks.sbm_network <- function(sbm, num_blocks = -1, level = 0){
  if(num_blocks < -1) stop(paste("Can't initialize", num_blocks, "blocks."))
  sbm <- verify_model(sbm)

  sbm$model$initialize_blocks(as.integer(num_blocks), as.integer(level))

  # Update state attribute
  attr(x, "state") <- sbm$model$get_state()

  sbm
}
