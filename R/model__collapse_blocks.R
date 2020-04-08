#' Agglomeratively merge blocks
#'
#' Performs agglomerative merging on network, starting with each block has a
#' single node down to one block per node type. Arguments are level to perform
#' merge at (int) and number of MCMC steps to peform between each collapsing to
#' equilibriate block. See the `agglomerative_merging.Rmd` vignette for more
#' complete discussion of options/behavior.
#'
#' @family modeling
#'
#' @inheritParams mcmc_sweep
#' @param num_mcmc_sweeps How many MCMC sweeps the model does at each
#'   agglomerative merge step. This allows the model to allow nodes to find
#'   their most natural resting place in a given collapsed state. Larger values
#'   will slow down runtime but can potentially lead for more stable results.
#' @param desired_num_blocks How many blocks should this given merge drop down
#'   to. If the network has more than one node type this number is multiplied by
#'   the total number of types.
#' @param num_block_proposals Controls how many merger proposals are drawn for
#'   each block in the model. A larger number will increase the exploration of
#'   merge potentials but may lead the model to local minimums. If the number of
#'   proposals is greater than then number of blocks then all blocks are
#'   searched exhaustively.
#' @param sigma Controls the rate of collapse. At each step of the collapsing
#'   the model will try and remove `current_num_nodes(1 - 1/sigma)` nodes from
#'   the model. So a larger sigma means a faster collapse rate.
#' @param allow_exhaustive If the number of proposals for a blocks merges is
#'   less than the number of proposals needed to check all possible merge
#'   combinations, should the model check all possible combinations?
#' @param report_all_steps  Should the model state be provided for every merge
#'   step or just the final one? If collapsing is being used to infer
#'   hierarcichal structure in data or inspection is desired this should be set
#'   to `TRUE`, otherwise it will slow down collapsing due to increased data
#'   transfer.
#'
#' @inherit new_sbm_network return
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network of two blocks with 25 nodes each
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#'   collapse_blocks(sigma = 1.2)
#'
#' # We can look directly at the collapse results
#' net %>% get_collapse_results()
#'
#' # We can visualize the collapse results
#' net %>% visualize_collapse_results()
#'
#' # We can choose best result with default heuristic
#' net <- choose_best_collapse_state(net, verbose = TRUE)
#'
#' # Setting sigma to a higher value means faster collapsing at the cost of less accuracy
#' net %>%
#'   collapse_blocks(sigma = 2) %>%
#'   visualize_collapse_results()
#'
collapse_blocks <- function(sbm,
                            desired_num_blocks = 1,
                            num_mcmc_sweeps = 0,
                            sigma = 2,
                            eps = 0.1,
                            num_block_proposals = 5,
                            level = 0,
                            allow_exhaustive = TRUE,
                            report_all_steps = TRUE){
  UseMethod("collapse_blocks")
}


#' @export
collapse_blocks.sbm_network <- function(sbm,
                                        desired_num_blocks = 1,
                                        num_mcmc_sweeps = 0,
                                        sigma = 2,
                                        eps = 0.1,
                                        num_block_proposals = 5,
                                        level = 0,
                                        allow_exhaustive = TRUE,
                                        report_all_steps = TRUE){
  # We call verify_model here in case this is being called in another thread using
  # the collapse_run function. In that case the pointer to the s4 class will be stale
  # and we will need to re-create the model class.
  sbm <- verify_model(sbm)

  collapse_results <- attr(sbm, 'model')$collapse_blocks(
    as.integer(level),
    as.integer(desired_num_blocks),
    as.integer(num_block_proposals),
    as.integer(num_mcmc_sweeps),
    sigma,
    eps,
    report_all_steps,
    allow_exhaustive
  )


  # If we're reporting every step, extract from the list the important values and append
  # the state and merges at each step
  if(report_all_steps) {
    sbm$collapse_results <- collapse_results %>%
      purrr::map_dfr(~dplyr::tibble(entropy_delta = .$entropy_delta,
                                    n_blocks = .$n_blocks)) %>%
      dplyr::mutate(state = purrr::map(collapse_results, 'state'),
                    merges = purrr::map(collapse_results,
                                        ~dplyr::tibble(from = .$merge_from,
                                                       into = .$merge_into)))
  } else {
    # Otherwise, just build a single row df with results and final state
    sbm$collapse_results <- dplyr::tibble(entropy_delta = collapse_results$entropy_delta,
                  n_blocks = collapse_results$n_blocks) %>%
      dplyr::mutate(state = list(collapse_results$state))
  }

  sbm
}


