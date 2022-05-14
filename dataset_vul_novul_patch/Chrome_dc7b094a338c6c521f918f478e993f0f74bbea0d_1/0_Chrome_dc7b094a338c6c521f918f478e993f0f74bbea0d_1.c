   virtual void CancelHandwritingStrokes(int stroke_count) {
     if (!initialized_successfully_)
       return;
    ibus_controller_->CancelHandwriting(stroke_count);
   }
