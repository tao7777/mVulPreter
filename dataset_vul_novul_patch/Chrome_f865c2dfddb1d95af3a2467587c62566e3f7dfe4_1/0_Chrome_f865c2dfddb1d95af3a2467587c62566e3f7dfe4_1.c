 DevToolsUI::DevToolsUI(content::WebUI* web_ui)
    : WebUIController(web_ui) {
   web_ui->SetBindings(0);
   Profile* profile = Profile::FromWebUI(web_ui);
   content::URLDataSource::Add(
       profile,
       new DevToolsDataSource(profile->GetRequestContext()));

  GURL url = web_ui->GetWebContents()->GetVisibleURL();
  if (url.spec() == SanitizeFrontendURL(url).spec())
    bindings_.reset(new DevToolsUIBindings(web_ui->GetWebContents()));
 }
