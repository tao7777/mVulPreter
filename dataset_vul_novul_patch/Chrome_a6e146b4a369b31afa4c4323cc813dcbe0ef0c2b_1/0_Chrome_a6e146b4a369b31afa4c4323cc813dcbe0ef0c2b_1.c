 void LiveSyncTest::SetupMockGaiaResponses() {
   username_ = "user@gmail.com";
   password_ = "password";
  integration_factory_.reset(new URLFetcherFactory());
  factory_.reset(new FakeURLFetcherFactory(integration_factory_.get()));
   factory_->SetFakeResponse(kClientLoginUrl, "SID=sid\nLSID=lsid", true);
   factory_->SetFakeResponse(kGetUserInfoUrl, "email=user@gmail.com", true);
   factory_->SetFakeResponse(kIssueAuthTokenUrl, "auth", true);
  factory_->SetFakeResponse(kSearchDomainCheckUrl, ".google.com", true);
  URLFetcher::set_factory(factory_.get());
}
