  bool SetImeConfig(const std::string& section,
  // IBusController override.
  virtual bool SetImeConfig(const std::string& section,
                            const std::string& config_name,
                            const ImeConfigValue& value) {
     if (!IBusConnectionsAreAlive()) {
       LOG(ERROR) << "SetImeConfig: IBus connection is not alive";
      return false;
    }

    bool is_preload_engines = false;

    std::vector<std::string> string_list;
    if ((value.type == ImeConfigValue::kValueTypeStringList) &&
        (section == kGeneralSectionName) &&
        (config_name == kPreloadEnginesConfigName)) {
      FilterInputMethods(value.string_list_value, &string_list);
      is_preload_engines = true;
    } else {
      string_list = value.string_list_value;
    }

    GVariant* variant = NULL;
    switch (value.type) {
      case ImeConfigValue::kValueTypeString:
        variant = g_variant_new_string(value.string_value.c_str());
        break;
      case ImeConfigValue::kValueTypeInt:
        variant = g_variant_new_int32(value.int_value);
        break;
      case ImeConfigValue::kValueTypeBool:
        variant = g_variant_new_boolean(value.bool_value);
        break;
      case ImeConfigValue::kValueTypeStringList:
        GVariantBuilder variant_builder;
        g_variant_builder_init(&variant_builder, G_VARIANT_TYPE("as"));
        const size_t size = string_list.size();  // don't use string_list_value.
        for (size_t i = 0; i < size; ++i) {
          g_variant_builder_add(&variant_builder, "s", string_list[i].c_str());
        }
        variant = g_variant_builder_end(&variant_builder);
        break;
    }

    if (!variant) {
      LOG(ERROR) << "SetImeConfig: variant is NULL";
      return false;
    }
    DCHECK(g_variant_is_floating(variant));

    ibus_config_set_value_async(ibus_config_,
                                section.c_str(),
                                config_name.c_str(),
                                variant,
                                -1,  // use the default ibus timeout
                                NULL,  // cancellable
                                SetImeConfigCallback,
                                g_object_ref(ibus_config_));

 
     if (is_preload_engines) {
      VLOG(1) << "SetImeConfig: " << section << "/" << config_name
              << ": " << value.ToString();
     }
     return true;
   }
