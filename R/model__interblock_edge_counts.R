#' Get counts between blocks at a level
#'
#' Returns a dataframe with all pairs of blocks at the desired level and the number of edges between them. Unconnected blocks are ommitted for space savings.
#'
#' @family modeling
#'
#' @seealso \code{\link{get_node_to_block_edge_counts}}
#' @inheritParams mcmc_sweep
#' @inheritParams get_node_to_block_edge_counts
#'
#' @return Dataframe of block pairs (`block_a` and `block_b`) and the number of edges between them (`count`).
#' @export
#'
#' @examples
#'
#' # A small simulated network with random block assignment
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 10) %>%
#'   initialize_blocks(3)
#'
#' # Get counts of connections from each given block to the others
#' net %>% interblock_edge_counts()
#'
interblock_edge_counts <- function(sbm, level = 1){
  UseMethod("interblock_edge_counts")
}

#' @export
interblock_edge_counts.sbm_network <- function(sbm, level = 1){
  sbm <- verify_model(sbm)
  attr(sbm, "model")$interblock_edge_counts(level)
}
