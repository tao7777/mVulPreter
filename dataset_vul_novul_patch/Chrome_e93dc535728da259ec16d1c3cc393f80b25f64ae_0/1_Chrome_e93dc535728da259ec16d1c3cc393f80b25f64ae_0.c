void ClipboardWin::OnClipboardUpdate() {
  DCHECK(window_);

  if (::IsClipboardFormatAvailable(CF_UNICODETEXT)) {
    base::string16 text;
    {
      ScopedClipboard clipboard;
      if (!clipboard.Init(window_->hwnd())) {
        LOG(WARNING) << "Couldn't open the clipboard." << GetLastError();
        return;
      }

      HGLOBAL text_global = clipboard.GetData(CF_UNICODETEXT);
      if (!text_global) {
        LOG(WARNING) << "Couldn't get data from the clipboard: "
                     << GetLastError();
         return;
       }
 
      base::win::ScopedHGlobal<WCHAR> text_lock(text_global);
       if (!text_lock.get()) {
         LOG(WARNING) << "Couldn't lock clipboard data: " << GetLastError();
         return;
      }
      text.assign(text_lock.get());
    }

    protocol::ClipboardEvent event;
    event.set_mime_type(kMimeTypeTextUtf8);
    event.set_data(ReplaceCrLfByLf(base::UTF16ToUTF8(text)));

    if (client_clipboard_.get()) {
      client_clipboard_->InjectClipboardEvent(event);
    }
  }
}
