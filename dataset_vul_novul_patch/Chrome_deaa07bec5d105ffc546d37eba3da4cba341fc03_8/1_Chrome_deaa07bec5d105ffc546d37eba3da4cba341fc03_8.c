void LogoService::SetClockForTests(std::unique_ptr<base::Clock> clock) {
  clock_for_test_ = std::move(clock);
}
