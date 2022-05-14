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
 
#if defined(OS_ANDROID)
  chrome::android::Java_PushMessagingServiceObserver_onMessageHandled(
      base::android::AttachCurrentThread());
#endif
 }
