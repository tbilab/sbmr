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
#' @param num_block_proposals Controls how many merger proposals are drawn for
#'   each block in the model. A larger number will increase the exploration of
#'   merge potentials but may lead the model to local minimums. If the number of
#'   proposals is greater than then number of blocks then all blocks are
#'   searched exhaustively.
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
  eps = NULL,
  num_block_proposals = 5,
  sigma = 2
){

  overide_eps <- !is.null(eps)
  if(overide_eps){
    old_eps <- sbm$EPS
    sbm$EPS <- eps
  }

  # Set free parameters
  sbm$N_CHECKS_PER_BLOCK <- num_block_proposals

  collapse_results <- sbm$collapse_blocks(
    as.integer(level),
    as.integer(num_mcmc_sweeps),
    as.integer(desired_num_blocks),
    as.integer(num_block_proposals),
    sigma,
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


