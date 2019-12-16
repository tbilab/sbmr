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
