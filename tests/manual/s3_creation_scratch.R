# Scratch work for the building the sbm_network s3 class

# library(sbmR)
# library(sloop)
# network <- sim_basic_block_network(
#   n_blocks = 3,
#   n_nodes_per_block = 40
# )
#
# sbm_net <- new_sbm_network(edges = network$edges, nodes = network$nodes)
# sbm_net
#
# x <- sbm_net

network <- sim_basic_block_network(n_blocks = 3,
                                   n_nodes_per_block = 40,
                                   setup_model = TRUE) %>%
  initialize_blocks(n_blocks = 5)

n_blocks(network)

visualize_network(network, node_color_col = 'block', node_shape_col = 'type')

pre_mode_address <- lobstr::obj_addr(attr(sbm_net, 'model'))
pre_sweep_state <- attr(sbm_net, 'model')$state()
pre_sweep_attr_state <- attr(sbm_net, 'state')
mcmc_results <- sbm_net %>% mcmc_sweep(num_sweeps = 25)
# Load up returned sbm_network from results
sbm_net <- mcmc_results$sbm_network

post_mode_address <- lobstr::obj_addr(attr(sbm_net, 'model'))
post_sweep_state <- attr(sbm_net, 'model')$state()
post_sweep_attr_state <- attr(sbm_net, 'state')

dplyr::all_equal(pre_sweep_attr_state,
                 post_sweep_attr_state)

dplyr::all_equal(pre_sweep_state,
                 post_sweep_state)

sbm_net <- sbm_net %>% initialize_model()




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







# Validates that a given object is a proper instance of sbm_network
validate_sbm_network <- function(){

}





class(sbm_network)



