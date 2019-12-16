#' Sample from model posterior
#'
#' Function is currently under construction
#'
#' @inheritParams create_sbm
#' @param n_sweeps Number of MCMC sweeps of all nodes (after burn-in)
#' @param n_chains How many seperate MCMC chains to start and run
#' @param burnin_sweeps How many sweeps to run before starting to record
#' @param return_sweep_info Return entropy and number of nodes changed for each
#'   sweep (defaults to `TRUE`)
#' @param starting_partition Algorithm to start each chain with? Current options
#'   are `"agglomerative"` or `"random"`.
#' @param verbose Array of what info should be printed as model runs. Default
#'   shows `"initialization"` info of model. Other options are `"silent` to
#'   print nothing, or `"iteration progress"` which prints out results of each
#'   mcmc iteration as it happens.
#' @param beta Inverse temperature parameter for MCMC acceptance probability
#' @param epsilon Parameter to inforce ergodicity of markov chain. Setting to
#'   zero will remove ergodicity from model, setting high will make model moves
#'   more random.
#' @param greedy If using agglomerative initialization should algorithm search
#'   every possible merge?
#' @param n_checks_per_group If not using agglomerative initialization, how many
#'   options for merging should be explored per group?
#' @param initialization_mcmc_steps If using agglomerative initialization, how
#'   many steps of MCMC should be run after merging groups to allow model to
#'   find optimal layout in new group partition?
#' @param ... Additional arguments to \code{\link{create_sbm}}
#'
#' @return
#' @export
#'
#' @examples
#'
#' sample_posterior(sim_basic_block_network())
#'
sample_posterior <- function(
  edges, nodes = NULL,
  n_sweeps = 100,
  n_chains = 1,
  burnin_sweeps = 10,
  return_sweep_info = TRUE, # separate df of entropy by sweep
  starting_partition = "agglomerative",
  beta = 1.5,
  epsilon = 0.1,
  greedy = TRUE,
  n_checks_per_group = 5,
  initialization_mcmc_steps = 15,
  verbose = c("initialization"),
  ...
){
  sbm <- create_sbm(edges, nodes, ...)

  #
  # # Run initialization algorithm on base level nodes
  # sbm %>% collapse_groups(
  #   greedy = greedy,
  #   num_mcmc_sweeps = initialization_mcmc_steps,
  #   beta = beta,
  # )
  # sbm$collapse_groups(0, as.integer(initialization_mcmc_steps))
  #
  # tibble(
  #   n_groups =  merge_results %>%
  #     purrr::map('state') %>%
  #     purrr::map_int(~{
  #       .x %>%
  #         dplyr::filter(level == 1) %>%
  #         dplyr::pull(id) %>%
  #         length()
  #     }),
  #   entropy = merge_results %>% purrr::map_dbl('entropy')
  # ) %>%
  #   ggplot(aes(x = n_groups, y = entropy)) +
  #   geom_line()
  #


  # for(i in 1:10){
  #   entro_pre <- sbm$compute_entropy(0L)
  #   groups_moved <- sbm$mcmc_sweep(0L,FALSE)
  #   print(paste("started with entropy of", entro_pre, "and moved", groups_moved))
  # }


  sbm
}
