void PrintPreviewDataSource::StartDataRequest(const std::string& path,
                                              bool is_incognito,
                                              int request_id) {
  if (!EndsWith(path, "/print.pdf", true)) {
    ChromeWebUIDataSource::StartDataRequest(path, is_incognito, request_id);
    return;
  }

   scoped_refptr<base::RefCountedBytes> data;
   std::vector<std::string> url_substr;
   base::SplitString(path, '/', &url_substr);
  int preview_ui_id = -1;
   int page_index = 0;
  if (url_substr.size() == 3 &&
      base::StringToInt(url_substr[0], &preview_ui_id),
      base::StringToInt(url_substr[1], &page_index) &&
      preview_ui_id >= 0) {
     PrintPreviewDataService::GetInstance()->GetDataEntry(
        preview_ui_id, page_index, &data);
   }
   if (data.get()) {
     SendResponse(request_id, data);
    return;
  }
  scoped_refptr<base::RefCountedBytes> empty_bytes(new base::RefCountedBytes);
  SendResponse(request_id, empty_bytes);
}
