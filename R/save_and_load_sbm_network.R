#' Save sbm_network object
#'
#' Saves an SBM Network object to local disk for use accross sessions without instantiating again. Opened with \code{\link{load_sbm_network}}
#'
#' @inheritParams verify_model
#' @param loc Location on computer of saved model `.rds` file.
#'
#' @return Saved `.rds` file on disk at specified location.
#' @export
#'
#' @examples
#'
#' network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 40)
#'
#' sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)
#'
#' temp <- tempfile()
#' save_sbm_network(sbm_net, temp)
#'
#' loaded_sbm_net <- load_sbm_network(tmp)
#'
save_sbm_network <- function(x, loc){
  UseMethod("save_sbm_network")
}

save_sbm_network.default <- function(x){
  cat("Model saving generic.")
}

#' @export
save_sbm_network.sbm_network <- function(x, loc){
  # Remove the s4 model object
  x$model <- NULL

  # Write object to RDS
  readr::write_rds(x, loc)
}

#' Load sbm_network object
#'
#' Loads an SBM Network object that was saved by \code{\link{save_sbm_network}}
#'
#' @param loc Location on computer of saved model `.rds` file.
#'
#' @return New `sbm_network` object copy in same state as when model was saved.
#' @export
#'
#' @examples
#'
#' network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 40)
#'
#' sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)
#'
#' temp <- tempfile()
#' save_sbm_network(sbm_net, temp)
#'
#' loaded_sbm_net <- load_sbm_network(tmp)
#'
load_sbm_network <- function(loc){
  x <- readr::read_rds(loc)

  initialize_model(x)
}
