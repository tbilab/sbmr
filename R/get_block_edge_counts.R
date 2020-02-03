#' Get counts between blocks at a level
#'
#' Returns a dataframe with all pairs of blocks at the desired level and the number of edges between them. Unconnected blocks are ommitted for space savings.
#'
#' @seealso \code{\link{get_node_edge_counts}}
#' @inheritParams get_node_to_block_edge_counts
#'
#' @return Dataframe of block pairs (`block_a` and `block_b`) and the number of edges between them (`count`).
#' @export
#'
#' @examples
#' # Initialize a random network and assign random blocks
#' sbm <- sim_basic_block_network(n_blocks = 5) %>%
#'   create_sbm() %>%
#'   initialize_blocks(5)
#'
#' # Gather edge counts for all level 1 block pairs
#' sbm %>% get_block_edge_counts(1)
#'
get_block_edge_counts <- function(sbm, level = 1){
  sbm$get_block_edge_counts(as.integer(level))
}
