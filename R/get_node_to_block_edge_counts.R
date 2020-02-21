#' Get node's edge counts to blocks
#'
#' For a given node in an SBM, returns a dataframe of each block (or node) that
#' the desired node is connected to and how many connections they have to that
#' block.
#'
#' @family advanced
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
#' # A small simulated network with random block assignment
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 10) %>%
#'   initialize_blocks(3)
#'
#' # Get a random node's edge counts to blocks
#' node_id <- sample(net$nodes$id, 1)
#' net %>% get_node_to_block_edge_counts(node_id, connection_level = 1)
#'
get_node_to_block_edge_counts <- function(sbm, node_id, connection_level = 1L){
  UseMethod("get_node_to_block_edge_counts")
}

get_node_to_block_edge_counts.default <- function(sbm, node_id, connection_level = 1L){
  cat("get_node_to_block_edge_counts generic")
}

#' @export
get_node_to_block_edge_counts.sbm_network <- function(sbm, node_id, connection_level = 1L){

  # Grab level of the node requested for connections.
  node_level <- get_state(sbm) %>%
    dplyr::filter(id == node_id) %>%
    dplyr::pull(level) %>%
    as.integer()

  # Call the exported method from the rcpp wrapper class.
  attr(verify_model(sbm), 'model')$get_node_to_block_edge_counts(node_id,
                                                                 node_level,
                                                                 as.integer(connection_level))
}

