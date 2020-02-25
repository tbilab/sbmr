x <- 1:10

test_that("Lowest method works", {

  y <- sin(x)

  score <- build_score_fn('lowest')(y)

  expect_equal(which(score == max(score)), 5)
})

test_that("Rolling mean deviance method works", {

  y <- sin(x)

  score <- build_score_fn('dev_from_rolling_mean')(y)

  expect_equal(which(score == max(score)), 4)
})

test_that("A bad heuristic gives informative error", {

  y <- sin(x)

  expect_error(
    build_score_fn('non-existant-heuristic'),
    "Hueristic must be either a function or one of {lowest, dev_from_rolling_mean, nls_residual, delta_ratio, trend_deviation}",
    fixed = TRUE
  )

})


test_that("NLS residual method works", {

  y <- 500 - 20*log(x)

  # Make there be no difference between 3rd and 4th element
  y[3] <- y[4]

  score <- build_score_fn('nls_residual')(y, x)

  expect_equal(which(score == max(score)), 3)
})

test_that("Custom function with a single argument works", {

  my_great_score <- function(y){
    s <- rnorm(length(x))
    #  Make ffth element biggest
    s[5] <- max(s) + abs(s[5])
    s
  }

  y <- 500 - 20*log(x)

  score <- build_score_fn(my_great_score)(y, x)

  expect_equal(which(score == max(score)), 5)
})

test_that("Custom function with a two arguments works", {

  my_great_score <- function(y,x){
    n <- length(x)
    (n - x) - y
  }

  y <- 50 - 4*log(x)

  score <- build_score_fn(my_great_score)(y, x)

  expect_equal(which(score == max(score)), 4)
})
