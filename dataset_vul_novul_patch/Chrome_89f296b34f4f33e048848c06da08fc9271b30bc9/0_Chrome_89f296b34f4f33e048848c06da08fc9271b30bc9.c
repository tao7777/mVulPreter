void PageFormAnalyserLogger::Flush() {
  std::string text;
  for (ConsoleLevel level : {kError, kWarning, kVerbose}) {
    for (LogEntry& entry : node_buffer_[level]) {
       text.clear();
       text += "[DOM] ";
       text += entry.message;

      std::vector<blink::WebNode> nodesToLog;
      for (unsigned i = 0; i < entry.nodes.size(); ++i) {
        if (entry.nodes[i].IsElementNode()) {
          const blink::WebElement element =
              entry.nodes[i].ToConst<blink::WebElement>();
          const blink::WebInputElement* webInputElement =
              blink::ToWebInputElement(&element);

          // Filter out password inputs with values from being logged, as their
          // values are also logged.
          const bool shouldObfuscate =
              webInputElement &&
              webInputElement->IsPasswordFieldForAutofill() &&
              !webInputElement->Value().IsEmpty();

          if (!shouldObfuscate) {
            text += " %o";
            nodesToLog.push_back(element);
          }
        }
      }
 
       blink::WebConsoleMessage message(level, blink::WebString::FromUTF8(text));
      message.nodes = std::move(nodesToLog);  // avoids copying node vectors.
       frame_->AddMessageToConsole(message);
     }
   }
  node_buffer_.clear();
}
