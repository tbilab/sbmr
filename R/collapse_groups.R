#' Agglomeratively merge blocks
#'
#' Performs agglomerative merging on network, starting with each block has a
#' single node down to one block per node type. Arguments are level to perform
#' merge at (int) and number of MCMC steps to peform between each collapsing to
#' equilibriate block. See the `agglomerative_merging.Rmd` vignette for more
#' complete discussion of options/behavior.
#'
#' @inheritParams mcmc_sweep
#' @param num_mcmc_sweeps How many MCMC sweeps the model does at each
#'   agglomerative merge step. This allows the model to allow nodes to find
#'   their most natural resting place in a given collapsed state. Larger values
#'   will slow down runtime but can potentially lead for more stable results.
#' @param desired_num_blocks How many blocks should this given merge drop down
#'   to. If the network has more than one node type this number is multiplied by
#'   the total number of types.
#' @param report_all_steps  Should the model state be provided for every merge
#'   step or just the final one? If collapsing is being used to infer
#'   hierarcichal structure in data or inspection is desired this should be set
#'   to `TRUE`, otherwise it will slow down collapsing due to increased data
#'   transfer.
#' @param beta Inverse temperature parameter impacting the acceptance
#'   probabilities of MCMC sweeps. A higher value means model puts more weight
#'   on accepting moves that improve model fit. Only matters if
#'   `num_mcmc_sweeps` is more than zero.
#' @param greedy Should every possible block merger be considered? If `FALSE`,
#'   candidates for mergers are drawn by similarity in edges (just as MCMC
#'   move proposals are). This may lead the model to local minimums by always
#'   pursuing best possible merges.
#' @param num_block_proposals If `greedy = FALSE`, this parameter controls how
#'   many merger proposals are drawn for each block in the model. A larger
#'   number will increase the exploration of merge potentials but may lead the
#'   model to local minimums for the same reason greedy mode does.
#' @param sigma Controls the rate of collapse. At each step of the collapsing
#'   the model will try and remove `current_num_nodes(1 - 1/sigma)` nodes from
#'   the model. So a larger sigma means a faster collapse rate.
#' @param eps Controls randomness of move proposals. Effects both the block
#'   merging and mcmc sweeps. If value is set to value other than null the SBMs
#'   current epsilon value will be overridden.
#'
#' @return Tibble with three columns with rows corresponding to the result of
#'   each merge step:  `entropy`, `num_blocks` left in model, and a list column
#'   of `state` which is the state dump dataframe for model at end of merge.
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network of two blocks with 25 nodes each
#' my_sbm <- sim_basic_block_network(n_blocks = 2, n_nodes_per_block = 25) %>%
#'   create_sbm()
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on network
#' collapse_results <- collapse_blocks(my_sbm, num_mcmc_sweeps = 0)
#'
#' # Investigate number of nodes at each step
#' collapse_results %>%
#'   dplyr::select(-state)
#'
collapse_blocks <- function(
  sbm,
  level = 0,
  num_mcmc_sweeps = 0,
  desired_num_blocks = 1,
  report_all_steps = FALSE,
  beta = 1.5,
  eps = NULL,
  greedy = FALSE,
  num_block_proposals = 5,
  sigma = 2
){

  overide_eps <- !is.null(eps)
  if(overide_eps){
    old_eps <- sbm$EPS
    sbm$EPS <- eps
  }

  # Set free parameters
  sbm$BETA <- beta
  sbm$GREEDY <- greedy
  sbm$N_CHECKS_PER_block <- num_block_proposals
  sbm$SIGMA <- sigma

  collapse_results <- sbm$collapse_blocks(
    as.integer(level),
    as.integer(num_mcmc_sweeps),
    as.integer(desired_num_blocks),
    report_all_steps
  )

  # Reset epsilon value
  if(overide_eps){
    sbm$EPS <- old_eps
  }

  purrr::map_dfr(
    collapse_results,
    ~dplyr::tibble(entropy = .$entropy,
                   num_blocks = .$num_blocks)
  ) %>%
    dplyr::mutate(state = purrr::map(collapse_results, 'state'))
}


