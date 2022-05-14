void DevToolsUIBindings::CallClientFunction(const std::string& function_name,
                                            const base::Value* arg1,
                                            const base::Value* arg2,
                                             const base::Value* arg3) {
   if (!web_contents_->GetURL().SchemeIs(content::kChromeDevToolsScheme))
     return;
   std::string javascript = function_name + "(";
   if (arg1) {
     std::string json;
    base::JSONWriter::Write(*arg1, &json);
    javascript.append(json);
    if (arg2) {
      base::JSONWriter::Write(*arg2, &json);
      javascript.append(", ").append(json);
      if (arg3) {
        base::JSONWriter::Write(*arg3, &json);
        javascript.append(", ").append(json);
      }
    }
  }
  javascript.append(");");
  web_contents_->GetMainFrame()->ExecuteJavaScript(
      base::UTF8ToUTF16(javascript));
}
