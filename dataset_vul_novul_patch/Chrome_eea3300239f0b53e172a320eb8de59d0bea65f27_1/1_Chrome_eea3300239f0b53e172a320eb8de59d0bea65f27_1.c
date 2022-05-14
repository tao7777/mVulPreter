     DidStartNavigationToPendingEntry(const GURL& url,
                                      content::ReloadType reload_type) {
  devtools_bindings_->frontend_host_.reset(
      content::DevToolsFrontendHost::Create(
          web_contents()->GetMainFrame(),
          base::Bind(&DevToolsUIBindings::HandleMessageFromDevToolsFrontend,
                     base::Unretained(devtools_bindings_))));
 }
