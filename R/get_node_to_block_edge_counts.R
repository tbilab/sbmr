#' Get node's edge counts to blocks
#'
#' For a given node in an SBM, returns a dataframe of each block (or node) that
#' the desired node is connected to and how many connections they have to that
#' block.
#'
#' @seealso \code{\link{get_block_edge_counts}}
#' @inheritParams add_node
#' @param node_id String identifying the node that edge counts are desired for
#' @param connection_level Level of blocks to get connections to. E.g. `1` = node block level, `0` = connections to other nodes.
#'
#' @return Dataframe with two columns: `id` for the id of the node/block connected to, and `count` for the number of connections between.
#' @export
#'
#' @examples
#'
#' # Initialize a random network and assign random blocks
#' sbm <- sim_basic_block_network(n_blocks = 5) %>%
#' create_sbm() %>%
#' initialize_blocks(5)
#'
#' # Gather edge counts for a node in the network
#' sbm %>% get_node_to_block_edge_counts("g1_1")
#'
get_node_to_block_edge_counts <- function(sbm, node_id, connection_level = 1L){
  set_generic("get_node_to_block_edge_counts")
}

get_node_to_block_edge_counts.default <- function(sbm, node_id, connection_level = 1L){
  cat("get_node_to_block_edge_counts generic")
}

#' @export
get_node_to_block_edge_counts.sbm_network <- function(sbm, node_id, connection_level = 1L){
  # Call the exported method from the rcpp wrapper class.
  sbm$model$get_node_to_block_edge_counts(node_id,
                                    as.integer(node_level),
                                    as.integer(connection_level))
}

