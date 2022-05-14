bool IsBlockedNavigation(net::Error error_code) {
  switch (error_code) {
    case net::ERR_BLOCKED_BY_CLIENT:
    case net::ERR_BLOCKED_BY_RESPONSE:
    case net::ERR_BLOCKED_BY_XSS_AUDITOR:
    case net::ERR_UNSAFE_REDIRECT:
      return true;
    default:
      return false;
  }
}
