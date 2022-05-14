 DevToolsUI::DevToolsUI(content::WebUI* web_ui)
    : WebUIController(web_ui),
      bindings_(web_ui->GetWebContents()) {
   web_ui->SetBindings(0);
   Profile* profile = Profile::FromWebUI(web_ui);
   content::URLDataSource::Add(
       profile,
       new DevToolsDataSource(profile->GetRequestContext()));
 }
