#' Run Agglomerative Merging to target a range of block numbers
#'
#' @family modeling
#'
#' @inheritParams collapse_blocks
#' @param num_final_blocks Array of integers corresponding to number of blocks to check in run.
#' @param parallel Run in parallel using `furrr`?
#'
#' @inherit new_sbm_network return
#'
#' @export
#'
#' @examples
#'
#' set.seed(42)
#'
#' # Start with a random network of two blocks with 25 nodes each
#' net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#'   collapse_run(num_final_blocks = 1:5, sigma = 1.5)
#'
#' # Collapse runs can be done in parallel
#' \dontrun{
#'   net <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25) %>%
#'     collapse_run(num_final_blocks = 1:5, sigma = 1.5, parallel = TRUE)
#' }
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
collapse_run <- function(sbm,
                         num_final_blocks = 1:10,
                         num_mcmc_sweeps = 10,
                         sigma = 2,
                         eps = 0.1,
                         num_block_proposals = 5,
                         parallel = FALSE){
  UseMethod("collapse_run")
}

collapse_run.default <- function(sbm,
                                 num_final_blocks = 1:10,
                                 num_mcmc_sweeps = 10,
                                 sigma = 2,
                                 eps = 0.1,
                                 num_block_proposals = 5,
                                 parallel = FALSE){
  cat("collapse_run generic")
}

#' @export
collapse_run.sbm_network <- function(sbm,
                                     num_final_blocks = 1:10,
                                     num_mcmc_sweeps = 10,
                                     sigma = 2,
                                     eps = 0.1,
                                     num_block_proposals = 5,
                                     parallel = FALSE){

  block_range <- num_final_blocks

  if(parallel){
    # Set up parallel processing environment. .skip will avoid re-creating a
    # plan if one already exists (saves substantial time on subsequent runs)
    future::plan(future::multiprocess, .skip = TRUE)

    results <- furrr::future_map_dfr(
      block_range,
      function(desired_num){
        collapse_blocks(sbm,
                        desired_num_blocks = desired_num,
                        sigma = sigma,
                        eps = eps,
                        report_all_steps = FALSE,
                        num_block_proposals = num_block_proposals,
                        num_mcmc_sweeps = num_mcmc_sweeps) %>%
          get_collapse_results()
      }
    )
  } else {
    collapse_results <- attr(verify_model(sbm), 'model')$collapse_run(0L,
                                         as.integer(num_mcmc_sweeps),
                                         as.integer(num_block_proposals),
                                         sigma,
                                         eps,
                                         as.integer(block_range))

    results <- purrr::map_dfr(
      collapse_results,
      ~dplyr::tibble(entropy = .$entropy,
                     num_blocks = .$num_blocks)
    ) %>%
      dplyr::mutate(state = purrr::map(collapse_results, 'state'))
  }

  sbm$collapse_results <- results

  sbm
}



