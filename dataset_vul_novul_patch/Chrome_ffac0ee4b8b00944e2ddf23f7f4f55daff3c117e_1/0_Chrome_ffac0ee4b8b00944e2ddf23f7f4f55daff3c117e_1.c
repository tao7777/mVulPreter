void ManifestUmaUtil::FetchFailed(FetchFailureReason reason) {
  ManifestFetchResultType fetch_result_type = MANIFEST_FETCH_RESULT_TYPE_COUNT;
  switch (reason) {
     case FETCH_EMPTY_URL:
       fetch_result_type = MANIFEST_FETCH_ERROR_EMPTY_URL;
       break;
    case FETCH_FROM_UNIQUE_ORIGIN:
      fetch_result_type = MANIFEST_FETCH_ERROR_FROM_UNIQUE_ORIGIN;
      break;
     case FETCH_UNSPECIFIED_REASON:
       fetch_result_type = MANIFEST_FETCH_ERROR_UNSPECIFIED;
       break;
  }
  DCHECK_NE(fetch_result_type, MANIFEST_FETCH_RESULT_TYPE_COUNT);

  UMA_HISTOGRAM_ENUMERATION(kUMANameFetchResult,
                            fetch_result_type,
                            MANIFEST_FETCH_RESULT_TYPE_COUNT);
}
