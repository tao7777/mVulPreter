DocumentInit& DocumentInit::WithPreviousDocumentCSP(
    const ContentSecurityPolicy* previous_csp) {
  DCHECK(!previous_csp_);
  previous_csp_ = previous_csp;
  return *this;
}
