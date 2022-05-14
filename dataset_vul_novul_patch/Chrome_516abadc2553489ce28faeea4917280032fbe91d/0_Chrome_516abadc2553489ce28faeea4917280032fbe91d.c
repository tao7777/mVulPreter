bool AffiliationFetcher::ParseResponse(
    AffiliationFetcherDelegate::Result* result) const {

  std::string serialized_response;
  if (!fetcher_->GetResponseAsString(&serialized_response)) {
    NOTREACHED();
  }

  affiliation_pb::LookupAffiliationResponse response;
  if (!response.ParseFromString(serialized_response))
    return false;

  result->reserve(requested_facet_uris_.size());

  std::map<FacetURI, size_t> facet_uri_to_class_index;
  for (int i = 0; i < response.affiliation_size(); ++i) {
    const affiliation_pb::Affiliation& equivalence_class(
        response.affiliation(i));
 
     AffiliatedFacets affiliated_uris;
     for (int j = 0; j < equivalence_class.facet_size(); ++j) {
      const std::string& uri_spec(equivalence_class.facet(j).id());
       FacetURI uri = FacetURI::FromPotentiallyInvalidSpec(uri_spec);
       if (!uri.is_valid())
        continue;
      affiliated_uris.push_back(uri);
    }

    if (affiliated_uris.empty())
      continue;

    for (const FacetURI& uri : affiliated_uris) {
      if (!facet_uri_to_class_index.count(uri))
        facet_uri_to_class_index[uri] = result->size();
      if (facet_uri_to_class_index[uri] !=
          facet_uri_to_class_index[affiliated_uris[0]]) {
        return false;
      }
    }

    if (facet_uri_to_class_index[affiliated_uris[0]] == result->size())
      result->push_back(affiliated_uris);
  }

  for (const FacetURI& uri : requested_facet_uris_) {
    if (!facet_uri_to_class_index.count(uri))
      result->push_back(AffiliatedFacets(1, uri));
  }

  return true;
}
