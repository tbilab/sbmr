#' Run Agglomerative Merging to targer a range of group numbers
#'
#' @inheritParams collapse_groups
#' @param start_group_num Fewest number of groups to test
#' @param end_group_num Highest number of groups to test
#'
#' @return Tibble with three columns with rows corresponding to the result of
#'   each merge step:  `entropy`, `num_groups` left in model, and a list column
#'   of `state` which is the state dump dataframe for model at end of merge.
#' @export
#'
#' @examples
collapse_run <- function(
  sbm,
  level = 0,
  sigma = 2,
  start_group_num = 1,
  end_group_num = 10,
  num_group_proposals = 5,
  num_mcmc_sweeps = 10,
  beta = 1.5
){

  # Set free parameters
  sbm$BETA <- beta
  sbm$SIGMA <- sigma
  sbm$N_CHECKS_PER_GROUP <- num_group_proposals
  sbm$GREEDY <- FALSE

  collapse_results <- sbm$collapse_run(
    as.integer(level),
    as.integer(num_mcmc_sweeps),
    as.integer(start_group_num),
    as.integer(end_group_num)
  )

  # dplyr::tibble(entropy = collapse_results$entropy,
  #               num_groups = collapse_results$num_groups,
  #               state = collapse_results$state)


  purrr::map_dfr(
    collapse_results,
    ~dplyr::tibble(entropy = .$entropy,
                   num_groups = .$num_groups)
  ) %>%
    dplyr::mutate(state = purrr::map(collapse_results, 'state'))
}


