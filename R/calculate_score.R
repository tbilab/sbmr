#' Calculate block collapse scores
#'
#' A helper function that is used in `visualize_collapse_results` and
#' `choose_best_collapse_state`.
#'
#' @param remove_state Should the state column be removed from results? Will be
#'   `TRUE` when visualizing results, `FALSE` when using score to choose best
#'   state.
#' @param use_entropy Should the score be calculated with the entropy of the
#'   collapse? If `FALSE`, the entropy delta will be used. The full entropy is
#'   needed to make comparisons across separate collapses in the case of
#'   \code{\link{collapse_run}}.
#' @inheritParams verify_model
#' @inheritParams build_score_fn
#'
#' @return Collapse results dataframe with a score column attached
#' @export
#'
calculate_collapse_score <- function(sbm, heuristic, remove_state = TRUE, use_entropy = FALSE){

  collapse_results <- get_collapse_results(sbm)

  if(!use_entropy & !("entropy_delta" %in% colnames(collapse_results))) {
    warning(paste("Can't use entropy delta for collapse score",
                  "because collapse results doesn't report",
                  "entropy delta for results from collapse_run().",
                  "Using entropy instead."))

    use_entropy <- TRUE
  }

  value_col <- if(use_entropy) rlang::sym("entropy") else rlang::sym("entropy_delta")

  collapse_results <- collapse_results %>%
    dplyr::arrange(n_blocks) %>%
    dplyr::select(!!value_col, n_blocks, state)

  if(remove_state){
    collapse_results <- collapse_results %>%
      dplyr::select(-state)
  }

  if(!is.null(heuristic)){
    collapse_results <- collapse_results %>%
      dplyr::mutate(score = build_score_fn(heuristic)(!!value_col, n_blocks))
  }

  collapse_results
}
