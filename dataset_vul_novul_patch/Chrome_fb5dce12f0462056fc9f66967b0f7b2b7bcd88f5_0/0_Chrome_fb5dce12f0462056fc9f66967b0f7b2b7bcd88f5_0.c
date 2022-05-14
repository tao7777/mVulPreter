void AddMainWebResources(content::WebUIDataSource* html_source) {
  html_source->AddResourcePath("media_router.js", IDR_MEDIA_ROUTER_JS);
  html_source->AddResourcePath("media_router_common.css",
                               IDR_MEDIA_ROUTER_COMMON_CSS);
  html_source->AddResourcePath("media_router.css",
                               IDR_MEDIA_ROUTER_CSS);
  html_source->AddResourcePath("media_router_data.js",
                                IDR_MEDIA_ROUTER_DATA_JS);
   html_source->AddResourcePath("media_router_ui_interface.js",
                                IDR_MEDIA_ROUTER_UI_INTERFACE_JS);
 }
