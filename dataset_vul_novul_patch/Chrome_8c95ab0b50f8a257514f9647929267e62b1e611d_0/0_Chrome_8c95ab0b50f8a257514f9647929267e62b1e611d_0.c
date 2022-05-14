  static scoped_refptr<Extension> MakeSyncTestExtension(
  static scoped_refptr<Extension> MakeSyncTestExtensionWithPluginPermission(
       SyncTestExtensionType type,
       const GURL& update_url,
       const GURL& launch_url,
       Manifest::Location location,
       int num_plugins,
       const base::FilePath& extension_path,
      int creation_flags,
      bool has_plugin_permission) {
     base::DictionaryValue source;
     source.SetString(keys::kName, "PossiblySyncableExtension");
     source.SetString(keys::kVersion, "0.0.0.0");
    if (type == APP)
      source.SetString(keys::kApp, "true");
    if (type == THEME)
      source.Set(keys::kTheme, new base::DictionaryValue());
    if (!update_url.is_empty()) {
      source.SetString(keys::kUpdateURL, update_url.spec());
    }
    if (!launch_url.is_empty()) {
      source.SetString(keys::kLaunchWebURL, launch_url.spec());
    }
    if (type != THEME) {
      source.SetBoolean(keys::kConvertedFromUserScript, type == USER_SCRIPT);
      base::ListValue* plugins = new base::ListValue();
      for (int i = 0; i < num_plugins; ++i) {
        base::DictionaryValue* plugin = new base::DictionaryValue();
        plugin->SetString(keys::kPluginsPath, std::string());
        plugins->Set(i, plugin);
       }
       source.Set(keys::kPlugins, plugins);
     }
    if (has_plugin_permission) {
      ListValue* plugins = new ListValue();
      plugins->Set(0, new StringValue("plugin"));
      source.Set(keys::kPermissions, plugins);
    }
 
     std::string error;
     scoped_refptr<Extension> extension = Extension::Create(
        extension_path, location, source, creation_flags, &error);
    EXPECT_TRUE(extension.get());
    EXPECT_EQ("", error);
     return extension;
   }
