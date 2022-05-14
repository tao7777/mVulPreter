 void DevToolsWindow::InspectedContentsClosing() {
   web_contents_->GetRenderViewHost()->ClosePage();
 }
