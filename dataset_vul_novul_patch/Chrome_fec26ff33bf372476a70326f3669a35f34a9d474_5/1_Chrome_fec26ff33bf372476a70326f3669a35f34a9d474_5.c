void LoadingStatsCollectorTest::TestRedirectStatusHistogram(
    const std::string& initial_url,
    const std::string& prediction_url,
    const std::string& navigation_url,
    RedirectStatus expected_status) {
   const std::string& script_url = "https://cdn.google.com/script.js";
   PreconnectPrediction prediction = CreatePreconnectPrediction(
       GURL(prediction_url).host(), initial_url != prediction_url,
      {{GURL(script_url).GetOrigin(), 1, net::NetworkIsolationKey()}});
   EXPECT_CALL(*mock_predictor_, PredictPreconnectOrigins(GURL(initial_url), _))
       .WillOnce(DoAll(SetArgPointee<1>(prediction), Return(true)));
 
  std::vector<content::mojom::ResourceLoadInfoPtr> resources;
  resources.push_back(
      CreateResourceLoadInfoWithRedirects({initial_url, navigation_url}));
  resources.push_back(
      CreateResourceLoadInfo(script_url, content::ResourceType::kScript));
  PageRequestSummary summary =
      CreatePageRequestSummary(navigation_url, initial_url, resources);

  stats_collector_->RecordPageRequestSummary(summary);

  histogram_tester_->ExpectUniqueSample(
      internal::kLoadingPredictorPreconnectLearningRedirectStatus,
      static_cast<int>(expected_status), 1);
}
