 void InitializeOriginStatFromOriginRequestSummary(
     OriginStat* origin,
     const OriginRequestSummary& summary) {
  origin->set_origin(summary.origin.spec());
   origin->set_number_of_hits(1);
   origin->set_average_position(summary.first_occurrence + 1);
   origin->set_always_access_network(summary.always_access_network);
  origin->set_accessed_network(summary.accessed_network);
}
