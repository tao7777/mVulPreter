bool NavigationRateLimiter::CanProceed() {
  if (!enabled)
    return true;

  static constexpr int kStateUpdateLimit = 200;
  static constexpr base::TimeDelta kStateUpdateLimitResetInterval =
      base::TimeDelta::FromSeconds(10);

  if (++count_ <= kStateUpdateLimit)
    return true;

  const base::TimeTicks now = base::TimeTicks::Now();
  if (now - time_first_count_ > kStateUpdateLimitResetInterval) {
    time_first_count_ = now;
    count_ = 1;
    error_message_sent_ = false;
    return true;
   }
 
   if (!error_message_sent_) {
     error_message_sent_ = true;
     if (auto* local_frame = DynamicTo<LocalFrame>(frame_.Get())) {
      local_frame->Console().AddMessage(ConsoleMessage::Create(
          mojom::ConsoleMessageSource::kJavaScript,
          mojom::ConsoleMessageLevel::kWarning,
          "Throttling navigation to prevent the browser from hanging. See "
          "https://crbug.com/882238. Command line switch "
          "--disable-ipc-flooding-protection can be used to bypass the "
          "protection"));
    }
  }

  return false;
}
