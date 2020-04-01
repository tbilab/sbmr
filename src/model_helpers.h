// Helper functions that get used throughout code 

inline double ent(const double e_rs, const double e_r, const double e_s)
{
  // OUT_MSG << "e_rs: " << e_rs
  //         << ", e_r: " << e_r
  //         << ", e_s: " << e_s
  //         << " -> " << e_rs * std::log(e_rs / (e_r * e_s))
  //         << std::endl;
  return e_rs * std::log(e_rs / (e_r * e_s));
}