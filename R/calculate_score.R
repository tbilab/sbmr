#' Calculate block collapse scores
#'
#' A helper function that is used in `visualize_collapse_results` and
#' `choose_best_collapse_state`.
#'
#' @param remove_state Should the state column be removed from results? Will be
#'   `TRUE` when visualizing results, `FALSE` when using score to choose best
#'   state.
#' @inheritParams verify_model
#' @inheritParams build_score_fn
#'
#' @return Collapse results dataframe with a score column attached
#' @export
#'
calculate_collapse_score <- function(sbm, heuristic, remove_state = TRUE){

  collapse_results <- get_collapse_results(sbm)

  is_collapse_run <- "final_entropy" %in% names(collapse_results)

  value_col <- if(is_collapse_run) rlang::sym("final_entropy") else rlang::sym("entropy_delta")

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
