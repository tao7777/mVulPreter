int OmniboxViewWin::OnPerformDropImpl(const views::DropTargetEvent& event,
                                      bool in_drag) {
  const ui::OSExchangeData& data = event.data();

  if (data.HasURL()) {
    GURL url;
    string16 title;
     if (data.GetURLAndTitle(&url, &title)) {
       string16 text(StripJavascriptSchemas(UTF8ToUTF16(url.spec())));
       SetUserText(text);
      model()->AcceptInput(CURRENT_TAB, true);
       return CopyOrLinkDragOperation(event.source_operations());
     }
   } else if (data.HasString()) {
    int string_drop_position = drop_highlight_position();
    string16 text;
    if ((string_drop_position != -1 || !in_drag) && data.GetString(&text)) {
      DCHECK(string_drop_position == -1 ||
             ((string_drop_position >= 0) &&
              (string_drop_position <= GetTextLength())));
      if (in_drag) {
        if (event.source_operations()== ui::DragDropTypes::DRAG_MOVE)
          MoveSelectedText(string_drop_position);
        else
          InsertText(string_drop_position, text);
      } else {
        PasteAndGo(CollapseWhitespace(text, true));
      }
      return CopyOrLinkDragOperation(event.source_operations());
    }
  }

  return ui::DragDropTypes::DRAG_NONE;
}
