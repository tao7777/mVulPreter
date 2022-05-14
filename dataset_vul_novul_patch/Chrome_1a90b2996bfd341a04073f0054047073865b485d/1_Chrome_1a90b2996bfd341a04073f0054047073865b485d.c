void PushMessagingServiceImpl::DidHandleMessage(
    const std::string& app_id,
    const base::Closure& message_handled_closure) {
  auto in_flight_iterator = in_flight_message_deliveries_.find(app_id);
  DCHECK(in_flight_iterator != in_flight_message_deliveries_.end());

  in_flight_message_deliveries_.erase(in_flight_iterator);

#if BUILDFLAG(ENABLE_BACKGROUND)
  if (in_flight_message_deliveries_.empty())
    in_flight_keep_alive_.reset();
#endif
 
   message_handled_closure.Run();
 
  if (push_messaging_service_observer_)
    push_messaging_service_observer_->OnMessageHandled();
 }
