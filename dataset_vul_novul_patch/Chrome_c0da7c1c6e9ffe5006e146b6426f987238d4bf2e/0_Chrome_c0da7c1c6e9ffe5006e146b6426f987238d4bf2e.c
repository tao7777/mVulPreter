 void DevToolsWindow::InspectedContentsClosing() {
  intercepted_page_beforeunload_ = false;
   web_contents_->GetRenderViewHost()->ClosePage();
 }
