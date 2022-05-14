void ObserverOnLogoAvailable(LogoObserver* observer,
                             bool from_cache,
                             LogoCallbackReason type,
                             const base::Optional<Logo>& logo) {
  switch (type) {
    case LogoCallbackReason::DISABLED:
    case LogoCallbackReason::CANCELED:
    case LogoCallbackReason::FAILED:
      break;
    case LogoCallbackReason::REVALIDATED:
      break;
    case LogoCallbackReason::DETERMINED:
      observer->OnLogoAvailable(logo ? &logo.value() : nullptr, from_cache);
      break;
  }
  if (!from_cache) {
    observer->OnObserverRemoved();
  }
}
