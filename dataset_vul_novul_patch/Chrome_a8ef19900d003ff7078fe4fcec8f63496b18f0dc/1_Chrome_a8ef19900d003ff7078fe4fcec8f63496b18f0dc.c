WebContents* DevToolsWindow::OpenURLFromTab(
    WebContents* source,
    const content::OpenURLParams& params) {
   DCHECK(source == main_web_contents_);
   if (!params.url.SchemeIs(content::kChromeDevToolsScheme)) {
     WebContents* inspected_web_contents = GetInspectedWebContents();
    return inspected_web_contents ?
        inspected_web_contents->OpenURL(params) : NULL;
   }
   bindings_->Reload();
   return main_web_contents_;
}
