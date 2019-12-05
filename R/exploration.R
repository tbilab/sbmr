library(sbmR)
library(tidyverse)





edges <- tribble(
  ~from, ~to,
  "a1", "b1",
  "a1", "b2",
  "a1", "b3",
  "a2", "b3",
  "a3", "b2"
)

nodes <- tribble(
  ~id, ~type,
  "a1", "a",
  "a2", "a",
  "a3", "a",
  "b1", "b",
  "b2", "b",
  "c1", "c",
  "b3", "b"
)

setClass("SBM",
         slots = list(
           edges = "data.frame",
           nodes = "data.frame",
           model = "Rcpp_Rcpp_SBM",
           node_types = "data.frame")
)

setGeneric("add_node", function(x, id, type) standardGeneric("add_node"))
setMethod("add_node", "SBM", function(x, id, type){
  x@model$add_node(id, type, 0L)
})

my_sbm <- new("SBM", edges = edges, nodes = nodes)

my_sbm %>% add_node("a1", 0L)

my_sbm@model$add_node("a1", 0L, 0L)







# sbm <- new(Rcpp_SBM)
#
# sbm$add_node("a1", 0L, 0L)
# sbm$add_node("a2", 0L, 0L)
# sbm$add_node("a3", 0L, 0L)
# sbm$add_node("b1", 1L, 0L)
# sbm$add_node("b2", 1L, 0L)
# sbm$add_node("b3", 1L, 0L)
#
# sbm$add_node("a11", 0L, 1L)
# sbm$add_node("a12", 0L, 1L)
# sbm$add_node("b11", 1L, 1L)
# sbm$add_node("b12", 1L, 1L)
#
# sbm$add_connection("a1", "b1")
# sbm$add_connection("a1", "b2")
# sbm$add_connection("a1", "b3")
# sbm$add_connection("a2", "b3")
# sbm$add_connection("a3", "b2")
#
# sbm$set_node_parent("a1", "a11", 0, TRUE)
# sbm$set_node_parent("a2", "a11", 0, TRUE)
# sbm$set_node_parent("a3", "a12", 0, TRUE)
# sbm$set_node_parent("b1", "b11", 0, TRUE)
# sbm$set_node_parent("b2", "b11", 0, TRUE)
# sbm$set_node_parent("b3", "b12", 0, TRUE)
#
#
#
# # Set some model parameters
# sbm$GREEDY <- TRUE
# sbm$BETA <- 1.5
# sbm$EPS <- 0.1
# sbm$N_CHECKS_PER_GROUP <- 5
#
# original_state <- sbm$get_state()
#
# for(i in 1:10){
#   entro_pre <- sbm$compute_entropy(0L)
#   groups_moved <- sbm$mcmc_sweep(0L,FALSE)
#   print(paste("started with entropy of", entro_pre, "and moved", groups_moved))
# }
#
# new_state <- sbm$get_state()
#
# # Bring me back to original state
# sbm %>% load_state(original_state)
