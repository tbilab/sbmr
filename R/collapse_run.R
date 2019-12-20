#' Run Agglomerative Merging to target a range of group numbers
#'
#' @inheritParams collapse_groups
#' @param num_final_groups Array of integers corresponding to number of groups to check in run.
#' @param parallel Run in parallel using `furrr`?
#'
#' @return Tibble with three columns with rows corresponding to the result of
#'   each merge step:  `entropy`, `num_groups` left in model, and a list column
#'   of `state` which is the state dump dataframe for model at end of merge.
#' @export
#'
#' @examples
#' set.seed(42)
#'
#' # Start with a random network of two groups with 25 nodes each
#' network <- sim_basic_block_network(n_groups = 3, n_nodes_per_group = 25)
#'
#' # Create SBM from simulated data
#' my_sbm <- create_sbm(network)
#'
#' # Run agglomerative clustering with no intermediate MCMC steps on network
#' collapse_results <- collapse_run(my_sbm, sigma = 3, num_final_groups = 1:6)
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
  num_final_groups = 1:10,
  num_group_proposals = 5,
  num_mcmc_sweeps = 10,
  beta = 1.5,
  eps = NULL,
  parallel = FALSE
){
  # Gather info needed to make copy of sbm on other thread
  model_data <- sbm$get_data()
  model_eps <- sbm$EPS

  group_range <- num_final_groups

  if(parallel){
    # Set up parallel processing environment. .skip will avoid re-creating a
    # plan if one already exists (saves substantial time on subsequent runs)
    future::plan(future::multiprocess, .skip = TRUE)

    results <- furrr::future_map_dfr(
      group_range,
      function(desired_num){
        collapse_groups(create_sbm(model_data, eps = model_eps),
                        desired_num_groups = desired_num,
                        sigma = sigma,
                        beta = beta,
                        num_group_proposals = num_group_proposals,
                        num_mcmc_sweeps = num_mcmc_sweeps)
      }
    )
  } else {
    sbm$SIGMA <- sigma
    sbm$BETA <- beta
    sbm$N_CHECKS_PER_GROUP <- num_group_proposals
    collapse_results <- sbm$collapse_run(as.integer(level),
                                as.integer(num_mcmc_sweeps),
                                as.integer(group_range))

    results <- purrr::map_dfr(
      collapse_results,
      ~dplyr::tibble(entropy = .$entropy,
                     num_groups = .$num_groups)
    ) %>%
    dplyr::mutate(state = purrr::map(collapse_results, 'state'))
  }

   results
}


