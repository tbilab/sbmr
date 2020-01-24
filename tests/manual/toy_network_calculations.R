library(tidyverse)

ent <- function(e_rs,e_r,e_s,self = FALSE){
  if(e_rs == 0 | e_r == 0 | e_s == 0){
    return(0)
  }
  if(self){
    e_rs = e_rs*2
  }

  print(glue::glue("{e_rs}log({e_rs}/({e_r}*{e_s}) = {e_rs*log(e_rs/(e_r*e_s))}"))
  e_rs*log(e_rs/(e_r*e_s))
}

# Extra connection network default state
get_entropy <- function(e_a, e_b, e_c, e_aa, e_ab, e_ac, e_bb, e_cb, e_cc, node_degrees){
  edge_counts <- (e_a + e_b + e_c)/2

  degree_sum <- tibble(degrees = node_degrees) %>%
    count(degrees) %>%
    mutate(val = n*log(factorial(degrees))) %>%
    pull(val) %>%
    sum()

  edges_sum <- ent(e_aa, e_a, e_a, self = TRUE) + ent(e_ab, e_b, e_a)              + ent(e_ac, e_c, e_a) +
    ent(e_ab, e_a, e_b)              + ent(e_bb, e_b, e_b, self = TRUE) + ent(e_cb, e_c, e_b) +
    ent(e_ac, e_a, e_c)              + ent(e_cb, e_b, e_c)              + ent(e_cc, e_c, e_c, self = TRUE)

  -edge_counts - degree_sum - edges_sum/2
}

prob_move_to_block <- function(e_ts, e_t, eps, B){
  sum( (e_ts + eps)/(e_t + (eps*B)) )
}

# Extra edge network in default format
pre_move_uni <- get_entropy(
  e_a = 8,
  e_b = 9,
  e_c = 7,

  e_aa = 1,
  e_ab = 4,
  e_ac = 2,
  e_bb = 1,
  e_cb = 3,
  e_cc = 1,

  node_degrees = c(4, 4, 4, 5, 4, 3)
)

p_n4_to_c <- prob_move_to_block(
  e_ts = c(e_ac = 2, e_cb = 3, e_cc = 1*2),
  e_t = c(e_a = 8, e_b = 9, e_c = 7),
  eps = 0.1,
  B = 3
)

# Extra edge network in n4 -> c format
post_move_uni <- get_entropy(
  e_a = 8,
  e_b = 4,
  e_c = 12,

  e_aa = 1,
  e_ab = 2,
  e_ac = 4,
  e_bb = 0,
  e_cb = 2,
  e_cc = 3,

  node_degrees = c(4, 4, 4, 5, 4, 3)
)

p_n4_back_to_b <- prob_move_to_block(
  e_ts = c(e_ab = 2, e_bb = 0, e_bc = 2),
  e_t = c(e_a = 8, e_b = 4, e_c = 12),
  eps = 0.1,
  B = 3
)


unipartite_ent_delta <- post_move_uni - pre_move_uni

unipartite_move_prob <- exp(unipartite_ent_delta)*(p_n4_to_c/p_n4_back_to_b)

unipartite_move_prob

# Function to do the same for a simple bipartite network
get_entropy_bipartite <- function(e_a11, e_a12, e_a13, e_b11, e_b12, e_b13,
                                  a11_b11, a11_b12, a11_b13,
                                  a12_b11, a12_b12, a12_b13,
                                  a13_b11, a13_b12, a13_b13,
                                  node_degrees ){
  edge_counts <- (e_a11 + e_a12 + e_a13 + e_b11 + e_b12 + e_b13)/2

  # browser()
  degree_sum <- tibble(degrees = node_degrees) %>%
    count(degrees) %>%
    mutate(val = n*log(factorial(degrees))) %>%
    pull(val) %>%
    sum()

  edges_sum <- ent(a11_b11, e_a11, e_b11) + ent(a11_b12, e_a11, e_b12) + ent(a11_b13, e_a11, e_b13) +
               ent(a12_b11, e_a12, e_b11) + ent(a12_b12, e_a12, e_b12) + ent(a12_b13, e_a12, e_b13) +
               ent(a13_b11, e_a13, e_b11) + ent(a13_b12, e_a13, e_b12) + ent(a13_b13, e_a13, e_b13)
  -edge_counts - degree_sum - edges_sum

}

# Bipartite model in base state
pre_move_bi <- get_entropy_bipartite(e_a11 = 1, e_a12 = 5, e_a13 = 1, e_b11 = 5, e_b12 = 1, e_b13 = 1,
                      a11_b11 = 1, a11_b12 = 0, a11_b13 = 0,
                      a12_b11 = 4, a12_b12 = 0, a12_b13 = 1,
                      a13_b11 = 0, a13_b12 = 1, a13_b13 = 0,
                      node_degrees = c(1,2,3,1,2,3,1,1) )


p_a2_to_a11 <- prob_move_to_block(
  e_ts = c(a11_b11 = 1, a11_b12 = 0, a11_b13 = 0),
  e_t =  c(  e_b11 = 5,   e_b12 = 1,   e_b13 = 1),
  eps = 0.1,
  B = 3
)


# Bipartite entropy after moving a2 -> a_11
post_move_bi <- get_entropy_bipartite(e_a11 = 3, e_a12 = 3, e_a13 = 1, e_b11 = 5, e_b12 = 1, e_b13 = 1,
                      a11_b11 = 3, a11_b12 = 0, a11_b13 = 0,
                      a12_b11 = 2, a12_b12 = 0, a12_b13 = 1,
                      a13_b11 = 0, a13_b12 = 1, a13_b13 = 0,
                      node_degrees = c(1,2,3,1,2,3,1,1) )

p_a2_back_to_a12 <- prob_move_to_block(
  e_ts = c(a12_b11 = 2, a12_b12 = 0, a12_b13 = 1),
  e_t =  c(  e_b11 = 5,   e_b12 = 1,   e_b13 = 1),
  eps = 0.1,
  B = 3
)


bipartite_ent_delta <- post_move_bi - pre_move_bi
bipartite_move_prob <- exp(bipartite_ent_delta)*(p_a2_to_a11/p_a2_back_to_a12)
bipartite_move_prob
