void RenderFrameImpl::DidAddMessageToConsole(
    const blink::WebConsoleMessage& message,
    const blink::WebString& source_name,
    unsigned source_line,
    const blink::WebString& stack_trace) {
  logging::LogSeverity log_severity = logging::LOG_VERBOSE;
  switch (message.level) {
    case blink::mojom::ConsoleMessageLevel::kVerbose:
      log_severity = logging::LOG_VERBOSE;
      break;
    case blink::mojom::ConsoleMessageLevel::kInfo:
      log_severity = logging::LOG_INFO;
      break;
    case blink::mojom::ConsoleMessageLevel::kWarning:
      log_severity = logging::LOG_WARNING;
      break;
    case blink::mojom::ConsoleMessageLevel::kError:
      log_severity = logging::LOG_ERROR;
      break;
    default:
      log_severity = logging::LOG_VERBOSE;
  }

  if (ShouldReportDetailedMessageForSource(source_name)) {
    for (auto& observer : observers_) {
      observer.DetailedConsoleMessageAdded(
          message.text.Utf16(), source_name.Utf16(), stack_trace.Utf16(),
          source_line, static_cast<uint32_t>(log_severity));
     }
   }
 
  Send(new FrameHostMsg_DidAddMessageToConsole(
      routing_id_, static_cast<int32_t>(log_severity), message.text.Utf16(),
      static_cast<int32_t>(source_line), source_name.Utf16()));
 }
