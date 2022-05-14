void ChromeOSSendHandwritingStroke(InputMethodStatusConnection* connection,
  virtual void SendHandwritingStroke(const HandwritingStroke& stroke) {
  }

  virtual void CancelHandwriting(int n_strokes) {
  }
};

IBusController* IBusController::Create() {
#if defined(HAVE_IBUS)
  return IBusControllerImpl::GetInstance();
#else
  return new IBusControllerStubImpl;
#endif
 }
