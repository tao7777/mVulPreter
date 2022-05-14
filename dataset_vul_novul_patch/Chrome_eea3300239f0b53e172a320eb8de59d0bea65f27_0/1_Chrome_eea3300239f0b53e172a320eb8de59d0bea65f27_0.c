DevToolsUIBindings::DevToolsUIBindings(content::WebContents* web_contents)
    : profile_(Profile::FromBrowserContext(web_contents->GetBrowserContext())),
      android_bridge_(DevToolsAndroidBridge::Factory::GetForProfile(profile_)),
      web_contents_(web_contents),
      delegate_(new DefaultBindingsDelegate(web_contents_)),
      devices_updates_enabled_(false),
      frontend_loaded_(false),
      reloading_(false),
      weak_factory_(this) {
  g_instances.Get().push_back(this);
  frontend_contents_observer_.reset(new FrontendWebContentsObserver(this));
  web_contents_->GetMutableRendererPrefs()->can_accept_load_drops = false;

  file_helper_.reset(new DevToolsFileHelper(web_contents_, profile_, this));
  file_system_indexer_ = new DevToolsFileSystemIndexer();
  extensions::ChromeExtensionWebContentsObserver::CreateForWebContents(
      web_contents_);

   embedder_message_dispatcher_.reset(
       DevToolsEmbedderMessageDispatcher::CreateForDevToolsFrontend(this));
  frontend_host_.reset(content::DevToolsFrontendHost::Create(
      web_contents_->GetMainFrame(),
      base::Bind(&DevToolsUIBindings::HandleMessageFromDevToolsFrontend,
                 base::Unretained(this))));
 }
