interblock_edge_counts <- function(sbm, level = 1){
  UseMethod("interblock_edge_counts")
}

#' @export
interblock_edge_counts.sbm_network <- function(sbm, level = 1){
  sbm <- verify_model(sbm)
  attr(sbm, "model")$get_interblock_edge_counts(level)
}
