print.sbm_network <- function(x){
  N <- attr(sbm_net, "n_nodes")
  E <- attr(sbm_net, "n_edges")
  cat(glue::glue("SBM Network with {N} nodes and {E} edges."))

  if (!is.null(x$model)){
    cat("The network has an initialized model object.")
  }
}
