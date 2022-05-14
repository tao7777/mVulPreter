 MdSettingsUI::MdSettingsUI(content::WebUI* web_ui)
     : content::WebUIController(web_ui) {
   AddSettingsPageUIHandler(new AppearanceHandler(web_ui));
  AddSettingsPageUIHandler(new ClearBrowsingDataHandler(web_ui));
   AddSettingsPageUIHandler(new DownloadsHandler());
   AddSettingsPageUIHandler(new StartupPagesHandler(web_ui));
 
  content::WebUIDataSource* html_source =
      content::WebUIDataSource::Create(chrome::kChromeUIMdSettingsHost);

  for (size_t i = 0; i < kSettingsResourcesSize; ++i) {
    html_source->AddResourcePath(kSettingsResources[i].name,
                                 kSettingsResources[i].value);
  }

  AddLocalizedStrings(html_source);
  html_source->SetDefaultResource(IDR_SETTINGS_SETTINGS_HTML);

  content::WebUIDataSource::Add(web_ui->GetWebContents()->GetBrowserContext(),
                                html_source);
}
