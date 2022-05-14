void LogoService::SetLogoCacheForTests(std::unique_ptr<LogoCache> cache) {
  logo_cache_for_test_ = std::move(cache);
}
