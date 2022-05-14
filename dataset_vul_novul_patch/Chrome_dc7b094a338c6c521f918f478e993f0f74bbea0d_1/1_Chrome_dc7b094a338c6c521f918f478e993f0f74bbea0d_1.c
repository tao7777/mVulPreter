   virtual void CancelHandwritingStrokes(int stroke_count) {
     if (!initialized_successfully_)
       return;
    chromeos::CancelHandwriting(input_method_status_connection_, stroke_count);
   }
