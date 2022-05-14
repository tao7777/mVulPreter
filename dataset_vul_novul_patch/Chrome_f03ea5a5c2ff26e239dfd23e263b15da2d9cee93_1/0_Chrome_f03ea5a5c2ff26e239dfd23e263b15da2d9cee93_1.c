void ServiceWorkerContextCore::OnReportConsoleMessage(
    ServiceWorkerVersion* version,
    blink::mojom::ConsoleMessageSource source,
    blink::mojom::ConsoleMessageLevel message_level,
    const base::string16& message,
    int line_number,
     const GURL& source_url) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  // RenderFrameHostImpl::DidAddMessageToConsole, which also asks the
  const bool is_builtin_component = HasWebUIScheme(source_url);

  LogConsoleMessage(ConsoleMessageLevelToLogSeverity(message_level), message,
                    line_number, is_builtin_component, wrapper_->is_incognito(),
                    base::UTF8ToUTF16(source_url.spec()));

  observer_list_->Notify(
      FROM_HERE, &ServiceWorkerContextCoreObserver::OnReportConsoleMessage,
      version->version_id(),
      ConsoleMessage(source, message_level, message, line_number, source_url));
}
