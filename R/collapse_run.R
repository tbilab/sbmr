#' Run Agglomerative Merging to target a range of block numbers
#'
#' @inheritParams collapse_blocks
#' @param num_final_blocks Array of integers corresponding to number of blocks to check in run.
#' @param parallel Run in parallel using `furrr`?
#'
#' @return Tibble with three columns with rows corresponding to the result of
#'   each merge step:  `entropy`, `num_blocks` left in model, and a list column
#'   of `state` which is the state dump dataframe for model at end of merge.
#' @export
#'
#' @examples
#' set.seed(42)
#'
#' # Start with a random network of two blocks with 25 nodes each
#' network <- sim_basic_block_network(n_blocks = 3, n_nodes_per_block = 25)
#'
#' # Create SBM from simulated data
#' my_sbm <- create_sbm(network)
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on network
#' collapse_results <- collapse_run(my_sbm, sigma = 3, num_final_blocks = 1:6)
#'
#' # Visualize results of collapsing
#' visualize_collapse_results(collapse_results)
#'
#' # Choose best result
#' my_sbm <- choose_best_collapse_state(my_sbm, collapse_results, verbose = TRUE)
#'
collapse_run <- function(
  sbm,
  level = 0,
  sigma = 2,
  num_final_blocks = 1:10,
  num_block_proposals = 5,
  num_mcmc_sweeps = 10,
  eps = NULL,
  parallel = FALSE
){
  # Gather info needed to make copy of sbm on other thread
  model_data <- sbm$get_data()
  model_eps <- sbm$EPS

  block_range <- num_final_blocks

  if(parallel){
    # Set up parallel processing environment. .skip will avoid re-creating a
    # plan if one already exists (saves substantial time on subsequent runs)
    future::plan(future::multiprocess, .skip = TRUE)

    results <- furrr::future_map_dfr(
      block_range,
      function(desired_num){
        collapse_blocks(create_sbm(model_data, eps = model_eps),
                        desired_num_blocks = desired_num,
                        sigma = sigma,
                        num_block_proposals = num_block_proposals,
                        num_mcmc_sweeps = num_mcmc_sweeps)
      }
    )
  } else {
    collapse_results <- sbm$collapse_run(as.integer(level),
                                         as.integer(num_mcmc_sweeps),
                                         as.integer(num_block_proposals),
                                         sigma,
                                         as.integer(block_range))

    results <- purrr::map_dfr(
      collapse_results,
      ~dplyr::tibble(entropy = .$entropy,
                     num_blocks = .$num_blocks)
    ) %>%
    dplyr::mutate(state = purrr::map(collapse_results, 'state'))
  }

   results
}


