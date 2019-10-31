#include <Rcpp.h>
#include <random>
#include "Node.h" 
#include "SBM.h" 

using namespace Rcpp;
using std::string;
using std::vector;
using std::map;

// =======================================================
// Constructor that takes the nodes unique id integer and type
// =======================================================
SBM::SBM(){
  // Nothing really needs doing
}


// [[Rcpp::export]]
List load_data(vector<string> edges_a, vector<string> edges_b){
  SBM my_SBM;
  int n_edges = edges_a.size();
  
  return List::create(
    _["n_edges"] = n_edges
  );
}





/*** R
data <- readr::read_csv('../southern_women.csv', col_types = readr::cols(event = 'c', individual = 'c')) 
load_data(data$event, data$individual)
*/
