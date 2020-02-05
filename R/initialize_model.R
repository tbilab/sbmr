initialize_model <- function(x){
  UseMethod("initialize_model")
}

initialize_model.default <- function(x){
  cat("Default initialize model generic.")
}

initialize_model.sbm_network <- function(sbm_net){

  # Instantiate instance of sbm class
  sbm <- new(SBM)

  # Fill in all the needed nodes
  for(i in 1:attr(sbm_net, "n_nodes")){
    sbm$add_node(sbm_net$nodes$id[i], sbm_net$nodes$type[i], 0L)
  }

  # Fill in the edges
  from_nodes <- dplyr::pull(sbm_net$edges, !!attr(sbm_net, "from_column"))
  to_nodes <- dplyr::pull(sbm_net$edges, !!attr(sbm_net, "to_column"))
  for(i in 1:attr(sbm_net, "n_edges")){
    sbm$add_edge(
      from_nodes[i],
      to_nodes[i]
    )
  }

  # Assign sbm object to name model in sbm_network object
  sbm_net$model <- sbm

  # Give back sbm_network object
  sbm_net
}
