  virtual void SendHandwritingStroke(const HandwritingStroke& stroke) {
     if (!initialized_successfully_)
       return;
    chromeos::SendHandwritingStroke(input_method_status_connection_, stroke);
   }
