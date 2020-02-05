# Scratch work for the building the sbm_network s3 class

library(sbmR)
# library(sloop)
network <- sim_basic_block_network(
  n_blocks = 3,
  n_nodes_per_block = 40
)

sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)
sbm_net

sbm_net <- sbm_net %>% initialize_model()
sbm_net

sbm_net$model


small_edges <- dplyr::tribble(
  ~a_node, ~b_node,
  "a1"   , "b1"   ,
  "a1"   , "b2"   ,
  "a1"   , "b3"   ,
  "a2"   , "b1"   ,
  "a2"   , "b4"   ,
  "a3"   , "b1"
)

sbm_net <- new_sbm_network(small_edges, edges_from_col = a_node, edges_to_col = b_node)
sbm_net
sbm_net <- sbm_net %>% initialize_model()
sbm_net


check_for_model_obj.sbm_network <- function(sbm_net, build_if_missing = TRUE){
  #  Check if the model object just hasnt been created at all
  missing_model <- is.null(sbm_net$model)

  if(missing_model){
    if (!build_if_missing) stop("SBM Network is missing model object.")
  }

}



# Validates that a given object is a proper instance of sbm_network
validate_sbm_network <- function(){

}





class(sbm_network)



