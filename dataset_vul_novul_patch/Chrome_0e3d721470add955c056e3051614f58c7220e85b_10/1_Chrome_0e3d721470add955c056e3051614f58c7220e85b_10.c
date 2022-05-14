void SynchronousCompositorImpl::SetInputHandler(
    cc::InputHandler* input_handler) {
  DCHECK(CalledOnValidThread());
  if (input_handler_)
    input_handler_->SetRootLayerScrollOffsetDelegate(NULL);
  input_handler_ = input_handler;
  if (input_handler_)
    input_handler_->SetRootLayerScrollOffsetDelegate(this);
}
