#' Agglomeratively merge groups
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
#' until the desired number remains. If `exhaustive = TRUE` then this parameter
#' will do nothing
#'
#' @return Tibble with three columns with rows corresponding to the result of
#'   each merge step:  `entropy`, `num_groups` left in model, and a list column
#'   of `state` which is the state dump dataframe for model at end of merge.
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
#'   select(-state)
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
  collapse_results <- sbm$collapse_groups(as.integer(level),
                     as.integer(num_mcmc_sweeps),
                     as.integer(desired_num_groups),
                     exhaustive)

  purrr::map_dfr(
    collapse_results,
    ~dplyr::tibble(entropy = .$entropy,
                   num_groups = .$num_groups)
  ) %>%
    dplyr::mutate(state = purrr::map(collapse_results, 'state'))
}


