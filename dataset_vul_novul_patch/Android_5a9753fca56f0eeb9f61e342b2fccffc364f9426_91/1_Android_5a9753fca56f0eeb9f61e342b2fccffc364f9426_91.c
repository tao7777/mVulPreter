   virtual void Predict(MB_PREDICTION_MODE mode) {
     mbptr_->mode_info_context->mbmi.mode = mode;
    REGISTER_STATE_CHECK(pred_fn_(mbptr_,
                                  data_ptr_[0] - kStride,
                                  data_ptr_[0] - 1, kStride,
                                  data_ptr_[0], kStride));
   }
