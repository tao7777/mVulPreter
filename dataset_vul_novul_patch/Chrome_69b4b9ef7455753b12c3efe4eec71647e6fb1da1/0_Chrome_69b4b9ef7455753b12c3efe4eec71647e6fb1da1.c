   std::unique_ptr<net::test_server::HttpResponse> GetConfigResponse(
       const net::test_server::HttpRequest& request) {
    // Config should not be fetched when in holdback.
    EXPECT_FALSE(
        data_reduction_proxy::params::IsIncludedInHoldbackFieldTrial());

     auto response = std::make_unique<net::test_server::BasicHttpResponse>();
     response->set_content(config_.SerializeAsString());
     response->set_content_type("text/plain");
    if (config_run_loop_)
      config_run_loop_->Quit();
    return response;
  }
