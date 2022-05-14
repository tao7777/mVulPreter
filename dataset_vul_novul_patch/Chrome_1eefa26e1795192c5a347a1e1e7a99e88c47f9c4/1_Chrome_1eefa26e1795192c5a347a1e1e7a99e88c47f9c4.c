void ChromeExtensionWebContentsObserver::RenderViewCreated(
     content::RenderViewHost* render_view_host) {
   ReloadIfTerminated(render_view_host);
   ExtensionWebContentsObserver::RenderViewCreated(render_view_host);
 }
