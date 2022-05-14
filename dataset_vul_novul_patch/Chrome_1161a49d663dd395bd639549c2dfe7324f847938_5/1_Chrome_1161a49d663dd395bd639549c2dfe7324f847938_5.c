void PrepareDropData(DropData* drop_data, const ui::OSExchangeData& data) {
  base::string16 plain_text;
  data.GetString(&plain_text);
  if (!plain_text.empty())
    drop_data->text = base::NullableString16(plain_text, false);
 
   GURL url;
   base::string16 url_title;
  data.GetURLAndTitle(&url, &url_title);
   if (url.is_valid()) {
     drop_data->url = url;
     drop_data->url_title = url_title;
  }

  base::string16 html;
  GURL html_base_url;
  data.GetHtml(&html, &html_base_url);
  if (!html.empty())
    drop_data->html = base::NullableString16(html, false);
  if (html_base_url.is_valid())
    drop_data->html_base_url = html_base_url;

  std::vector<ui::OSExchangeData::FileInfo> files;
  if (data.GetFilenames(&files) && !files.empty()) {
    for (std::vector<ui::OSExchangeData::FileInfo>::const_iterator
             it = files.begin(); it != files.end(); ++it) {
      drop_data->filenames.push_back(
          DropData::FileInfo(
              base::UTF8ToUTF16(it->path.AsUTF8Unsafe()),
              base::UTF8ToUTF16(it->display_name.AsUTF8Unsafe())));
    }
  }

  Pickle pickle;
  if (data.GetPickledData(ui::Clipboard::GetWebCustomDataFormatType(), &pickle))
    ui::ReadCustomDataIntoMap(
        pickle.data(), pickle.size(), &drop_data->custom_data);
}
