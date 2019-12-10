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
#'
#' @return Total number of nodes that were changed in the previous sweep
#' @export
#'
#' @examples
mcmc_sweep <- function(sbm, level = 0, variable_num_groups = TRUE){
  sbm$mcmc_sweep(as.integer(level), variable_num_groups)
}


#' Collapse Groups
#'
#' Performs agglomerative merging on network, starting with each group has a
#' single node down to one group per node type. Arguments are level to perform
#' merge at (int) and number of MCMC steps to peform between each collapsing to
#' equilibriate group. Returns
#'
#' @inheritParams mcmc_sweep
#' @param num_mcmc_sweeps Number of MCMC sweeps to run in between merge steps to
#'   allow model to find best partitioning with given number of groups. Set to
#'   zero for fast but more likely incorrect structure
#'
#' @return List with `entropy` and model `state` after each merge.
#' @export
#'
#' @examples
collapse_groups <- function(sbm, level = 0, num_mcmc_sweeps = 10){
  sbm$collapse_groups(as.integer(level), as.integer(num_mcmc_sweeps))
}




