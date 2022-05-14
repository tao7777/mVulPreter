void BlockedPlugin::LoadPlugin() {
  CHECK(plugin_);
  if (hidden_)
    return;
  WebPluginContainer* container = plugin_->container();
   WebPlugin* new_plugin =
       render_view()->CreatePluginNoCheck(frame_, plugin_params_);
   if (new_plugin && new_plugin->initialize(container)) {
     container->setPlugin(new_plugin);
     container->invalidate();
     container->reportGeometry();
    plugin_->ReplayReceivedData(new_plugin);
    plugin_->destroy();
  }
}
