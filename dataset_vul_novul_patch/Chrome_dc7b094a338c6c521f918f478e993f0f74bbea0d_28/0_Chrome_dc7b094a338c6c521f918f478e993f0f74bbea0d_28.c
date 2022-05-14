  virtual void SendHandwritingStroke(const HandwritingStroke& stroke) {
  virtual void SendHandwritingStroke(
      const input_method::HandwritingStroke& stroke) {
     if (!initialized_successfully_)
       return;
    ibus_controller_->SendHandwritingStroke(stroke);
   }
