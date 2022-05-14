void RunCallbacksWithDisabled(LogoCallbacks callbacks) {
  if (callbacks.on_cached_encoded_logo_available) {
    std::move(callbacks.on_cached_encoded_logo_available)
        .Run(LogoCallbackReason::DISABLED, base::nullopt);
  }
  if (callbacks.on_cached_decoded_logo_available) {
    std::move(callbacks.on_cached_decoded_logo_available)
        .Run(LogoCallbackReason::DISABLED, base::nullopt);
  }
  if (callbacks.on_fresh_encoded_logo_available) {
    std::move(callbacks.on_fresh_encoded_logo_available)
        .Run(LogoCallbackReason::DISABLED, base::nullopt);
  }
  if (callbacks.on_fresh_decoded_logo_available) {
    std::move(callbacks.on_fresh_decoded_logo_available)
        .Run(LogoCallbackReason::DISABLED, base::nullopt);
  }
}
