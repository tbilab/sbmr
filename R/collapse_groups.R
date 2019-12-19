#' Agglomeratively merge groups
#'
#' Performs agglomerative merging on network, starting with each group has a
#' single node down to one group per node type. Arguments are level to perform
#' merge at (int) and number of MCMC steps to peform between each collapsing to
#' equilibriate group. See the `agglomerative_merging.Rmd` vignette for more
#' complete discussion of options/behavior.
#'
#' @inheritParams mcmc_sweep
#' @param num_mcmc_sweeps How many MCMC sweeps the model does at each
#'   agglomerative merge step. This allows the model to allow nodes to find
#'   their most natural resting place in a given collapsed state. Larger values
#'   will slow down runtime but can potentially lead for more stable results.
#' @param desired_num_groups How many groups should this given merge drop down
#'   to. If the network has more than one node type this number is multiplied by
#'   the total number of types.
#' @param report_all_steps  Should the model state be provided for every merge
#'   step or just the final one? If collapsing is being used to infer
#'   hierarcichal structure in data or inspection is desired this should be set
#'   to `TRUE`, otherwise it will slow down collapsing due to increased data
#'   transfer.
#' @param beta Inverse temperature parameter impacting the acceptance
#'   probabilities of MCMC sweeps. A higher value means model puts more weight
#'   on accepting moves that improve model fit. Only matters if `num_mcmc_sweeps
#'   > 0`.
#' @param greedy Should every possible group merger be considered? If `FALSE`,
#'   candidates for mergers are drawn by similarity in connections (just as MCMC
#'   move proposals are). This may lead the model to local minimums by always
#'   pursuing best possible merges.
#' @param num_group_proposals If `greedy = FALSE`, this parameter controls how
#'   many merger proposals are drawn for each group in the model. A larger
#'   number will increase the exploration of merge potentials but may lead the
#'   model to local minimums for the same reason greedy mode does.
#' @param sigma Controls the rate of collapse. At each step of the collapsing
#'   the model will try and remove `current_num_nodes(1 - 1/sigma)` nodes from
#'   the model. So a larger sigma means a faster collapse rate.
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
  num_mcmc_sweeps = 0,
  desired_num_groups = 1,
  report_all_steps = FALSE,
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

  collapse_results <- sbm$collapse_groups(
    as.integer(level),
    as.integer(num_mcmc_sweeps),
    as.integer(desired_num_groups),
    report_all_steps
  )

  purrr::map_dfr(
    collapse_results,
    ~dplyr::tibble(entropy = .$entropy,
                   num_groups = .$num_groups)
  ) %>%
    dplyr::mutate(state = purrr::map(collapse_results, 'state'))
}


