# This file contains simple wrapper functions that call the exposed methods from the C++ module.
# The reason this is done is to allow for cleaner documentation but the user can still directly
# call the methods with SBM$METHOD_NAME(...) instead of METHOD_NAME(SBM, ...).


#' Add node to network
#'
#' Add a node to the network. Takes the node id (string), the node type
#' (string), and the node level (int).
#'
#' @param sbm SBM model object as created by \code{\link{create_sbm()}}.
#' @param id Unique identifying name for node.
#' @param type Type of node (string). This is used to distinguish multipartite networks. E.g. "person", or "publication", etc..
#' @param level Level in node hierarchy. Level `0` indicates node is at the data level, levels greater than `0` are group nodes.
#'
#' @return SBM model object modified with node added. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#'
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2')
#'
#' get_state(my_sbm)
#'
add_node <- function(sbm, id, type = "node", level = 0){
  sbm$add_node(id, type, as.integer(level))
  sbm
}


#' Add connection/ edge
#'
#' Connects two nodes in network (at level 0) by their ids (string).
#'
#' @inheritParams add_node
#' @param a_node Id of first node in connection
#' @param b_node Id of second node in connection
#'
#' @return SBM model object modified with connection added. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#'
#' # Create SBM and add two nodes
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2')
#'
#' # Add connection between two nodes
#' my_sbm %>% add_connection('node_1', 'node_2')
#'
add_connection <- function(sbm, a_node, b_node){
  sbm$add_connection(a_node, b_node)
  sbm
}



#' Set node parent
#'
#' Sets the parent node (or group) for a given node. Takes child node's id
#' (string), parent node's id (string), and the level of child node (int).
#'
#' @inheritParams add_node
#' @param child_id Id of node for which parent is being added
#' @param parent_id Id of parent node
#' @param level Level of child node. This will almost always stay at default
#'   value of `0`.
#'
#' @return SBM model object modified with node hierarchy modified. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#'
#' # Create a model with two nodes, one at the group level (1)
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_11', level = 1)
#'
#' # Assign node 11 as node 1's parent
#' my_sbm %>%
#'   set_node_parent(child_id = 'node_1', parent_id = 'node_11')
#'
set_node_parent <- function(sbm, child_id, parent_id, level = 0){
  sbm$set_node_parent(child_id, parent_id, as.integer(level))
  sbm
}

#' Initialize groups
#'
#' Initializes groups for a given level of network. When `num_groups = -1` (the
#' default) this will simply give every node its own group. Otherwise it will
#' randomly draw each nodes group from `num_group` possible parent nodes for
#' each node type. E.g. if network is bipartite and `num_groups == 3` a total of
#' `3*2 = 6` groups will be added and assigned randomly to nodes. Because of
#' random assignment of parents there may sometimes be less groups than
#' `num_groups * <num_types>` due to some group nodes never being assigned to a
#' child node.
#'
#' @inheritParams set_node_parent
#' @param num_groups Total number of groups to initialize for each unique node
#'   type. If set to `-1`, then a single group is made for every node.
#'
#' @return SBM model object modified with new group nodes and node hierarchy
#'   modified. \emph{Note that object is modified in place as well.}
#' @export
#'
#' @examples
#' # Helper function to get number of groups in a model
#' get_num_groups <- function(sbm){sum(get_state(sbm)$level == 1)}
#'
#' # Initialize a simple bipartite network with 6 total nodes (3 of each type)
#' my_nodes <- dplyr::tibble(
#'   id = c("a1", "a2", "a3", "b1", "b2", "b3"),
#'   type = c("a", "a", "a", "b", "b", "b")
#' )
#' my_sbm <- create_sbm(nodes = my_nodes)
#'
#' # Default values of function will give every node its own group
#' my_sbm %>% initialize_groups()
#' get_num_groups(my_sbm)
#'
#' # You can also decide to have a given number of groups randomly assigned Here
#' # four groups result because two random groups are made for each of the two
#' # types
#' my_sbm %>% initialize_groups(num_groups = 2)
#' get_num_groups(my_sbm)
#'
initialize_groups <- function(sbm, num_groups = -1, level = 0){
  if(num_groups < -1) stop(paste("Can't initialize", num_groups, "groups."))
  sbm$initialize_groups(as.integer(num_groups), as.integer(level))
  sbm
}


#' Get state
#'
#' Exports the current state of the network as dataframe with each node as a row
#' and columns for node id, parent id, node type, and node level.
#'
#' @inheritParams add_node
#'
#' @return A dataframe describing the current state of the network: \describe{
#'   \item{id}{id of node}
#'   \item{parent}{id of node's parent. `none` if node has no parent.}
#'   \item{type}{type of node}
#'   \item{level}{level of node in network hierarchy}
#' }
#'
#' @export
#'
#' @examples
#'
#' # Build a bipartite network with 1 node and one group for each type
#' my_sbm <- create_sbm() %>%
#'   add_node('a1', type = 'a') %>%
#'   add_node('a11', type = 'a', level = 1) %>%
#'   add_node('b1', type = 'b') %>%
#'   add_node('b11', type = 'b', level = 1) %>%
#'   set_node_parent('a1', 'a11') %>%
#'   set_node_parent('b1', 'b11')
#'
#' my_sbm %>% get_state()
#'
get_state <- function(sbm){
  sbm$get_state()
}



#' Load from state
#'
#' Takes model state export as given by SBM$get_state and returns model to
#' specified state. This is useful for resetting model before running various
#' algorithms such as agglomerative merging.
#'
#' @inheritParams add_node
#' @param state SBM state dump dataframe as provided by \code{\link{get_state}}.
#'
#' @return SBM model object modified with state modified to matched passed state
#'   dump. \emph{Note that object is modified in place as well.}
#'
#' @export
#'
#' @examples
#'
#' # A state dump dataframe as one would get from sbmR::get_state()
#' desired_state <- dplyr::tribble(
#'   ~id,  ~parent, ~type, ~level,
#'   "a1",   "a11",   "a",      0,
#'   "b1",   "b11",   "b",      0,
#'  "a11",  "none",   "a",      1,
#'  "b11",  "none",   "b",      1
#' )
#'
#' # Create simple network with no group nodes
#' my_sbm <- create_sbm() %>%
#'   add_node('a1', type = 'a') %>%
#'   add_node('b1', type = 'b')
#'
#' # Load our desired state
#' my_sbm %>% load_from_state(desired_state)
#' # Group nodes have been created and assigned to proper children nodes
#'
load_from_state <- function(sbm, state){
  sbm$load_from_state(
    state$id,
    state$parent,
    state$level,
    state$type)
  sbm
}


#' Compute Entropy
#'
#' Computes the (degree-corrected) entropy for the network at the specified
#' level (int).
#'
#' @inheritParams add_node
#' @param level Level of nodes for which group entropy is accessed. E.g. the
#'   default value of `0` will get entropy of model as described by the groups
#'   in level `1`.
#'
#' @return Entropy value (numeric).
#' @export
#'
#' @examples
#'
#' # Build basic network with 3 nodes and two groups
#' my_sbm <- create_sbm() %>%
#'   add_node('node_1') %>%
#'   add_node('node_2') %>%
#'   add_node('node_3') %>%
#'   add_node('node_11', level = 1) %>%
#'   add_node('node_12', level = 1) %>%
#'   add_connection('node_1', 'node_2') %>%
#'   add_connection('node_1', 'node_3') %>%
#'   add_connection('node_2', 'node_3') %>%
#'   set_node_parent(child_id = 'node_1', parent_id = 'node_11') %>%
#'   set_node_parent(child_id = 'node_2', parent_id = 'node_11') %>%
#'   set_node_parent(child_id = 'node_3', parent_id = 'node_12')
#'
#' # Compute entropy of network
#' compute_entropy(my_sbm)
#'
compute_entropy <- function(sbm, level = 0){
  sbm$compute_entropy(as.integer(level))
}


#' MCMC Sweep
#'
#' Runs a single Metropolis Hastings MCMC sweep across all nodes at specified
#' level (For algorithm details see
#' \href{https://arxiv.org/abs/1705.10225}{Piexoto, 2018}). Each node is given a
#' chance to move groups or stay in current group and all nodes are processed in
#' random order. Takes the level that the sweep should take place on (int) and
#' if new groups groups can be proposed and empty groups removed (boolean).
#'
#' @inheritParams add_node
#' @param level Level of nodes who's groups will have their group membership run
#'   through MCMC proposal-accept routine.
#' @param variable_num_groups Should the model allow new groups to be created or
#'   empty groups removed while sweeping or should number of groups remain
#'   constant?
#' @param beta Inverse temperature parameter for determining move acceptance probability.
#'
#' @return Total number of nodes that were changed in the previous sweep
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network and assign randomly to 4 groups
#' n_groups <- 3
#' my_sbm <- create_sbm(sim_basic_block_network(n_groups = n_groups, n_nodes_per_group = 15)) %>%
#'   initialize_groups(num_groups = n_groups)
#'
#' # Calculate entropy with random groups
#' compute_entropy(my_sbm)
#'
#' # Run 4 MCMC sweeps
#' for(i in 1:4) my_sbm %>% mcmc_sweep(variable_num_groups = FALSE)
#'
#' # Calculate entropy after sweeps
#' compute_entropy(my_sbm)
#'
mcmc_sweep <- function(sbm, level = 0, variable_num_groups = TRUE, beta = 1.5){
  sbm$BETA <- beta
  sbm$mcmc_sweep(as.integer(level), variable_num_groups)
}


#' Collapse Groups
#'
#' Performs agglomerative merging on network, starting with each group has a
#' single node down to one group per node type. Arguments are level to perform
#' merge at (int) and number of MCMC steps to peform between each collapsing to
#' equilibriate group. See the `agglomerative_merging.Rmd` vignette for more
#' complete discussion of options/behavior.
#'
#' @inheritParams mcmc_sweep
#' @param num_mcmc_sweeps Number of MCMC sweeps to run in between merge steps to
#'   allow model to find best partitioning with given number of groups. Set to
#'   zero for fast but more likely incorrect structure
#' @param desired_num_groups How many groups should be left at the end of the
#'   collapsing? Ignored if `exhaustive = TRUE`.
#' @param exhaustive Should collapsing exhaust all possible number of groups?
#'   I.e. should network be collapsed one group at a time down to one group per
#'   node type?
#' @param beta Inverse temperature parameter for determining move acceptance
#'   probability. Only applicable if `num_mcmc_sweeps > 0`.
#' @param greedy Should all possible moves be considered for merging or should a
#'   set number of proposals be drawn?
#' @param num_group_proposals If `greedy = FALSE`, how many move proposals
#'   should each node produce for merge options?
#' @param sigma Controls how fast collapse of network happens. For instance if
#'   set to `2` then half (`1/2`) of the nodes will be removed at each step
#'   until the desired number remains. If `exhaustive = TRUE` then this
#'   parameter will do nothing
#'
#' @return List with `entropy` and model `state` after each merge.
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network of two groups with 25 nodes each
#' my_sbm <- sim_basic_block_network(n_groups = 2, n_nodes_per_group = 25) %>%
#'   create_sbm()
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on network
#' collapse_results <- collapse_groups(my_sbm, num_mcmc_sweeps = 0)
#'
#' # Investigate number of nodes at each step
#' collapse_results %>%
#'   purrr::map('state') %>%
#'   purrr::map_int(~dplyr::filter(., level == 1) %>% nrow())
#'
collapse_groups <- function(
  sbm,
  level = 0,
  num_mcmc_sweeps = 10,
  desired_num_groups = 1,
  exhaustive = TRUE,
  beta = 1.5,
  greedy = FALSE,
  num_group_proposals = 5,
  sigma = 2
){

  # Set free parameters
  sbm$BETA <- beta
  sbm$GREEDY <- greedy
  sbm$N_CHECKS_PER_GROUP <- num_group_proposals
  sbm$SIGMA <- sigma

  # The C++ function arguments
  # collapse_groups(const int node_level,
  #                 const int num_mcmc_steps,
  #                 int desired_num_groups,
  #                 const bool exhaustive)
  sbm$collapse_groups(as.integer(level),
                      as.integer(num_mcmc_sweeps),
                      as.integer(desired_num_groups),
                      exhaustive)
}




