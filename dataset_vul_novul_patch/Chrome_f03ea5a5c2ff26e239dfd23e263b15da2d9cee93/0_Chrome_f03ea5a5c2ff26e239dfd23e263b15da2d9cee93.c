void RenderFrameHostImpl::OnDidAddMessageToConsole(
void RenderFrameHostImpl::DidAddMessageToConsole(
    blink::mojom::ConsoleMessageLevel log_level,
     const base::string16& message,
     int32_t line_no,
     const base::string16& source_id) {
  // TODO(https://crbug.com/786836): Update downstream code to use
  // ConsoleMessageLevel everywhere to avoid this conversion.
  logging::LogSeverity log_severity = logging::LOG_VERBOSE;
  switch (log_level) {
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
   }
 
  if (delegate_->DidAddMessageToConsole(log_severity, message, line_no,
                                        source_id)) {
     return;
  }
 
  // Pass through log severity only on builtin components pages to limit console
   const bool is_builtin_component =
       HasWebUIScheme(delegate_->GetMainFrameLastCommittedURL()) ||
      GetContentClient()->browser()->IsBuiltinComponent(
          GetProcess()->GetBrowserContext(), GetLastCommittedOrigin());
   const bool is_off_the_record =
       GetSiteInstance()->GetBrowserContext()->IsOffTheRecord();
 
  LogConsoleMessage(log_severity, message, line_no, is_builtin_component,
                     is_off_the_record, source_id);
 }
