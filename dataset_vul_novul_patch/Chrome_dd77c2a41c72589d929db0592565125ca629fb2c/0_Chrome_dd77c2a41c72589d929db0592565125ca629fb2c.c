int32_t PPB_Flash_MessageLoop_Impl::InternalRun(
    const RunFromHostProxyCallback& callback) {
  if (state_->run_called()) {
    if (!callback.is_null())
      callback.Run(PP_ERROR_FAILED);
    return PP_ERROR_FAILED;
  }
  state_->set_run_called();
  state_->set_run_callback(callback);

  scoped_refptr<State> state_protector(state_);
   {
     base::MessageLoop::ScopedNestableTaskAllower allow(
         base::MessageLoop::current());
    blink::WebView::willEnterModalLoop();

     base::MessageLoop::current()->Run();

    blink::WebView::didExitModalLoop();
   }
 
  return state_protector->result();
}
