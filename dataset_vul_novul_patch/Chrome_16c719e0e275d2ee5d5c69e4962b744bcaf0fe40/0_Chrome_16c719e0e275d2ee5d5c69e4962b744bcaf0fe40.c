int OmniboxViewViews::OnDrop(const ui::OSExchangeData& data) {
  if (HasTextBeingDragged())
    return ui::DragDropTypes::DRAG_NONE;

  if (data.HasURL(ui::OSExchangeData::CONVERT_FILENAMES)) {
    GURL url;
    base::string16 title;
    if (data.GetURLAndTitle(
            ui::OSExchangeData::CONVERT_FILENAMES, &url, &title)) {
      base::string16 text(
          StripJavascriptSchemas(base::UTF8ToUTF16(url.spec())));
      if (model()->CanPasteAndGo(text)) {
        model()->PasteAndGo(text);
        return ui::DragDropTypes::DRAG_COPY;
      }
    }
   } else if (data.HasString()) {
     base::string16 text;
     if (data.GetString(&text)) {
      base::string16 collapsed_text(
          StripJavascriptSchemas(base::CollapseWhitespace(text, true)));
       if (model()->CanPasteAndGo(collapsed_text))
         model()->PasteAndGo(collapsed_text);
       return ui::DragDropTypes::DRAG_COPY;
    }
  }

  return ui::DragDropTypes::DRAG_NONE;
}
