#' Run a single MCMC sweep over nodes
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
#' @param beta Inverse temperature parameter for determining move acceptance
#'   probability.
#'
#' @return List with array of all nodes that were moved in the
#'   sweep (`nodes_moved`) and the sweeps impact on the model's entropy
#'   (`entropy_delta`).
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
