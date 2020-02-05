# Scratch work for the building the sbm_network s3 class

library(sbmR)
library(sloop)
set.seed(42) # seed for reproducability

n_blocks <- 3    # Four total groups
group_size <- 40 # W/ 50 nodes in each

network <- sim_basic_block_network(
  n_blocks = n_blocks,
  n_nodes_per_block = group_size,
  return_edge_propensities = TRUE
)


# my_sbm <- create_sbm(network)

new_sbm_network(edges = network$edges)

# Validates that a given object is a proper instance of sbm_network
validate_sbm_network <- function(){

}





class(sbm_network)



