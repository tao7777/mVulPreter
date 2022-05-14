 PrintPreviewUI::PrintPreviewUI(content::WebUI* web_ui)
     : ConstrainedWebDialogUI(web_ui),
       initial_preview_start_time_(base::TimeTicks::Now()),
      id_(g_print_preview_ui_id_map.Get().Add(this)),
       handler_(NULL),
       source_is_modifiable_(true),
       tab_closed_(false) {
  Profile* profile = Profile::FromWebUI(web_ui);
  ChromeURLDataManager::AddDataSource(profile, new PrintPreviewDataSource());

   handler_ = new PrintPreviewHandler();
   web_ui->AddMessageHandler(handler_);
 
  g_print_preview_request_id_map.Get().Set(id_, -1);
 }
