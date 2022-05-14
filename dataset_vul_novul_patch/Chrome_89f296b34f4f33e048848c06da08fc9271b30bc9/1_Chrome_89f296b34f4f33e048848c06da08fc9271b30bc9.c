void PageFormAnalyserLogger::Flush() {
  std::string text;
  for (ConsoleLevel level : {kError, kWarning, kVerbose}) {
    for (LogEntry& entry : node_buffer_[level]) {
       text.clear();
       text += "[DOM] ";
       text += entry.message;
      for (unsigned i = 0; i < entry.nodes.size(); ++i)
        text += " %o";
 
       blink::WebConsoleMessage message(level, blink::WebString::FromUTF8(text));
      message.nodes = std::move(entry.nodes);  // avoids copying node vectors.
       frame_->AddMessageToConsole(message);
     }
   }
  node_buffer_.clear();
}
