#' Flower and polinators network
#'
#' Tidy dataset of polinator-flower network from the 1923 paper "Experimental
#' pollination. An outline of the ecology of flowers and insects.", by Clements,
#' R. E., and F. L. Long.
#'
#' From the \href{https://www.nceas.ucsb.edu/interactionweb/html/clements_1923.html}{Interaction Web DataBase
#' entry:}
#' "The study took place on Pikes Peak in Colorado, U.S.A., in the early 1920's.
#' The authors recorded the identities of flower visitors to 94 native and two
#' exotic plant species."
#'
#' Original data for network was downloaded from
#' \url{https://www.nceas.ucsb.edu/interactionweb/html/clements_1923.html} on
#' February 18, 2020
#'
#' @format A edge data frame with 2 columns and 923 rows:
#' \describe{
#'   \item{polinator}{Scientific name in genus-species format of polinating
#'   animal}
#'   \item{flower}{Scientific name of the flower that the polinator visited}
#' }
#'
#' @source \href{https://www.nceas.ucsb.edu/interactionweb/html/clements_1923.html}{Interaction Web DataBase
#' entry:}
"clements_polinators"
