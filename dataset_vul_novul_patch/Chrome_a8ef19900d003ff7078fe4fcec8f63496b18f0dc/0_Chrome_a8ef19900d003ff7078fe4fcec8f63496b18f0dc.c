WebContents* DevToolsWindow::OpenURLFromTab(
    WebContents* source,
    const content::OpenURLParams& params) {
   DCHECK(source == main_web_contents_);
   if (!params.url.SchemeIs(content::kChromeDevToolsScheme)) {
     WebContents* inspected_web_contents = GetInspectedWebContents();
    if (!inspected_web_contents)
      return nullptr;
    content::OpenURLParams modified = params;
    modified.referrer = content::Referrer();
    return inspected_web_contents->OpenURL(modified);
   }
   bindings_->Reload();
   return main_web_contents_;
}
