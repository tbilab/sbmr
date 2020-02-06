#' Get number of blocks currently in model
#'
#' Returns the number of blocks/clusters in the currenet model state.
#'
#' @family modeling
#' @inheritParams verify_model
#'
#' @return Number of clusters in current model state
#' @export
#'
#' @examples
#'network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 40, setup_model = TRUE) %>%
#'   initialize_blocks(num_blocks = 5)
#'
#' # Returns the number of blocks currently in model. Note this is not neccesarily the true number.
#' get_num_blocks(network)
#'
get_num_blocks <- function(sbm){
  UseMethod("get_num_blocks")
}

get_num_blocks.default <- function(sbm){
  cat("get_num_blocks generic")
}

#' @export
get_num_blocks.sbm_network <- function(sbm){
  verify_model(sbm) %>%
    get_state() %>%
    dplyr::filter(level == 1) %>%
    nrow()
}
