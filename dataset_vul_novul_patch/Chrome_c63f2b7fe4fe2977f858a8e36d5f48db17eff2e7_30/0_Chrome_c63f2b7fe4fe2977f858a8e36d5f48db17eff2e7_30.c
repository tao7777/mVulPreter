bool Extension::InitFromValue(const DictionaryValue& source, int flags,
                              std::string* error) {
  URLPattern::ParseOption parse_strictness =
      (flags & STRICT_ERROR_CHECKS ? URLPattern::ERROR_ON_PORTS
                                   : URLPattern::IGNORE_PORTS);

  permission_set_.reset(new ExtensionPermissionSet());

  if (source.HasKey(keys::kPublicKey)) {
    std::string public_key_bytes;
    if (!source.GetString(keys::kPublicKey,
                          &public_key_) ||
        !ParsePEMKeyBytes(public_key_,
                          &public_key_bytes) ||
        !GenerateId(public_key_bytes, &id_)) {
      *error = errors::kInvalidKey;
      return false;
    }
  } else if (flags & REQUIRE_KEY) {
    *error = errors::kInvalidKey;
    return false;
  } else {
    id_ = Extension::GenerateIdForPath(path());
    if (id_.empty()) {
      NOTREACHED() << "Could not create ID from path.";
      return false;
    }
  }

  manifest_value_.reset(source.DeepCopy());

  extension_url_ = Extension::GetBaseURLFromExtensionId(id());

  std::string version_str;
  if (!source.GetString(keys::kVersion, &version_str)) {
    *error = errors::kInvalidVersion;
    return false;
  }
  version_.reset(Version::GetVersionFromString(version_str));
  if (!version_.get() ||
      version_->components().size() > 4) {
    *error = errors::kInvalidVersion;
    return false;
  }

  string16 localized_name;
  if (!source.GetString(keys::kName, &localized_name)) {
    *error = errors::kInvalidName;
    return false;
  }
  base::i18n::AdjustStringForLocaleDirection(&localized_name);
  name_ = UTF16ToUTF8(localized_name);

  if (source.HasKey(keys::kDescription)) {
    if (!source.GetString(keys::kDescription,
                          &description_)) {
      *error = errors::kInvalidDescription;
      return false;
    }
  }

  if (source.HasKey(keys::kHomepageURL)) {
    std::string tmp;
    if (!source.GetString(keys::kHomepageURL, &tmp)) {
      *error = ExtensionErrorUtils::FormatErrorMessage(
          errors::kInvalidHomepageURL, "");
      return false;
    }
    homepage_url_ = GURL(tmp);
    if (!homepage_url_.is_valid() ||
        (!homepage_url_.SchemeIs("http") &&
            !homepage_url_.SchemeIs("https"))) {
      *error = ExtensionErrorUtils::FormatErrorMessage(
          errors::kInvalidHomepageURL, tmp);
      return false;
    }
  }

  if (source.HasKey(keys::kUpdateURL)) {
    std::string tmp;
    if (!source.GetString(keys::kUpdateURL, &tmp)) {
      *error = ExtensionErrorUtils::FormatErrorMessage(
          errors::kInvalidUpdateURL, "");
      return false;
    }
    update_url_ = GURL(tmp);
    if (!update_url_.is_valid() ||
        update_url_.has_ref()) {
      *error = ExtensionErrorUtils::FormatErrorMessage(
          errors::kInvalidUpdateURL, tmp);
      return false;
    }
  }

  if (source.HasKey(keys::kMinimumChromeVersion)) {
    std::string minimum_version_string;
    if (!source.GetString(keys::kMinimumChromeVersion,
                          &minimum_version_string)) {
      *error = errors::kInvalidMinimumChromeVersion;
      return false;
    }

    scoped_ptr<Version> minimum_version(
        Version::GetVersionFromString(minimum_version_string));
    if (!minimum_version.get()) {
      *error = errors::kInvalidMinimumChromeVersion;
      return false;
    }

    chrome::VersionInfo current_version_info;
    if (!current_version_info.is_valid()) {
      NOTREACHED();
      return false;
    }

    scoped_ptr<Version> current_version(
        Version::GetVersionFromString(current_version_info.Version()));
    if (!current_version.get()) {
      DCHECK(false);
      return false;
    }

    if (current_version->CompareTo(*minimum_version) < 0) {
      *error = ExtensionErrorUtils::FormatErrorMessage(
          errors::kChromeVersionTooLow,
          l10n_util::GetStringUTF8(IDS_PRODUCT_NAME),
          minimum_version_string);
      return false;
    }
  }

  source.GetBoolean(keys::kConvertedFromUserScript,
                    &converted_from_user_script_);

  if (source.HasKey(keys::kIcons)) {
    DictionaryValue* icons_value = NULL;
    if (!source.GetDictionary(keys::kIcons, &icons_value)) {
      *error = errors::kInvalidIcons;
      return false;
    }

    for (size_t i = 0; i < arraysize(kIconSizes); ++i) {
      std::string key = base::IntToString(kIconSizes[i]);
      if (icons_value->HasKey(key)) {
        std::string icon_path;
        if (!icons_value->GetString(key, &icon_path)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidIconPath, key);
          return false;
        }

        if (!icon_path.empty() && icon_path[0] == '/')
          icon_path = icon_path.substr(1);

        if (icon_path.empty()) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidIconPath, key);
          return false;
        }

        icons_.Add(kIconSizes[i], icon_path);
      }
    }
  }

  is_theme_ = false;
  if (source.HasKey(keys::kTheme)) {
    if (ContainsNonThemeKeys(source)) {
      *error = errors::kThemesCannotContainExtensions;
      return false;
    }

    DictionaryValue* theme_value = NULL;
    if (!source.GetDictionary(keys::kTheme, &theme_value)) {
      *error = errors::kInvalidTheme;
      return false;
    }
    is_theme_ = true;

    DictionaryValue* images_value = NULL;
    if (theme_value->GetDictionary(keys::kThemeImages, &images_value)) {
      for (DictionaryValue::key_iterator iter = images_value->begin_keys();
           iter != images_value->end_keys(); ++iter) {
        std::string val;
        if (!images_value->GetString(*iter, &val)) {
          *error = errors::kInvalidThemeImages;
          return false;
        }
      }
      theme_images_.reset(images_value->DeepCopy());
    }

    DictionaryValue* colors_value = NULL;
    if (theme_value->GetDictionary(keys::kThemeColors, &colors_value)) {
      for (DictionaryValue::key_iterator iter = colors_value->begin_keys();
           iter != colors_value->end_keys(); ++iter) {
        ListValue* color_list = NULL;
        double alpha = 0.0;
        int color = 0;
        if (!colors_value->GetListWithoutPathExpansion(*iter, &color_list) ||
            ((color_list->GetSize() != 3) &&
             ((color_list->GetSize() != 4) ||
              !color_list->GetDouble(3, &alpha))) ||
            !color_list->GetInteger(0, &color) ||
            !color_list->GetInteger(1, &color) ||
            !color_list->GetInteger(2, &color)) {
          *error = errors::kInvalidThemeColors;
          return false;
        }
      }
      theme_colors_.reset(colors_value->DeepCopy());
    }

    DictionaryValue* tints_value = NULL;
    if (theme_value->GetDictionary(keys::kThemeTints, &tints_value)) {
      for (DictionaryValue::key_iterator iter = tints_value->begin_keys();
           iter != tints_value->end_keys(); ++iter) {
        ListValue* tint_list = NULL;
        double v = 0.0;
        if (!tints_value->GetListWithoutPathExpansion(*iter, &tint_list) ||
            tint_list->GetSize() != 3 ||
            !tint_list->GetDouble(0, &v) ||
            !tint_list->GetDouble(1, &v) ||
            !tint_list->GetDouble(2, &v)) {
          *error = errors::kInvalidThemeTints;
          return false;
        }
      }
      theme_tints_.reset(tints_value->DeepCopy());
    }

    DictionaryValue* display_properties_value = NULL;
    if (theme_value->GetDictionary(keys::kThemeDisplayProperties,
        &display_properties_value)) {
      theme_display_properties_.reset(
          display_properties_value->DeepCopy());
    }

    return true;
  }

  if (source.HasKey(keys::kPlugins)) {
    ListValue* list_value = NULL;
    if (!source.GetList(keys::kPlugins, &list_value)) {
      *error = errors::kInvalidPlugins;
      return false;
    }

    for (size_t i = 0; i < list_value->GetSize(); ++i) {
      DictionaryValue* plugin_value = NULL;
      std::string path_str;
      bool is_public = false;

      if (!list_value->GetDictionary(i, &plugin_value)) {
        *error = errors::kInvalidPlugins;
        return false;
      }

      if (!plugin_value->GetString(keys::kPluginsPath, &path_str)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidPluginsPath, base::IntToString(i));
        return false;
      }

      if (plugin_value->HasKey(keys::kPluginsPublic)) {
        if (!plugin_value->GetBoolean(keys::kPluginsPublic, &is_public)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidPluginsPublic, base::IntToString(i));
          return false;
        }
      }

#if !defined(OS_CHROMEOS)
      plugins_.push_back(PluginInfo());
      plugins_.back().path = path().AppendASCII(path_str);
      plugins_.back().is_public = is_public;
#endif
    }
  }

  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableExperimentalExtensionApis) &&
      source.HasKey(keys::kNaClModules)) {
    ListValue* list_value = NULL;
    if (!source.GetList(keys::kNaClModules, &list_value)) {
      *error = errors::kInvalidNaClModules;
      return false;
    }

    for (size_t i = 0; i < list_value->GetSize(); ++i) {
      DictionaryValue* module_value = NULL;
      std::string path_str;
      std::string mime_type;

      if (!list_value->GetDictionary(i, &module_value)) {
        *error = errors::kInvalidNaClModules;
        return false;
      }

      if (!module_value->GetString(keys::kNaClModulesPath, &path_str)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidNaClModulesPath, base::IntToString(i));
        return false;
      }

      if (!module_value->GetString(keys::kNaClModulesMIMEType, &mime_type)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidNaClModulesMIMEType, base::IntToString(i));
        return false;
      }

      nacl_modules_.push_back(NaClModuleInfo());
      nacl_modules_.back().url = GetResourceURL(path_str);
      nacl_modules_.back().mime_type = mime_type;
    }
  }

  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableExperimentalExtensionApis) &&
      source.HasKey(keys::kToolstrips)) {
    ListValue* list_value = NULL;
    if (!source.GetList(keys::kToolstrips, &list_value)) {
      *error = errors::kInvalidToolstrips;
      return false;
    }

    for (size_t i = 0; i < list_value->GetSize(); ++i) {
      GURL toolstrip;
      DictionaryValue* toolstrip_value = NULL;
      std::string toolstrip_path;
      if (list_value->GetString(i, &toolstrip_path)) {
        toolstrip = GetResourceURL(toolstrip_path);
      } else if (list_value->GetDictionary(i, &toolstrip_value)) {
        if (!toolstrip_value->GetString(keys::kToolstripPath,
                                        &toolstrip_path)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidToolstrip, base::IntToString(i));
          return false;
        }
        toolstrip = GetResourceURL(toolstrip_path);
      } else {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidToolstrip, base::IntToString(i));
        return false;
      }
      toolstrips_.push_back(toolstrip);
    }
  }

  if (source.HasKey(keys::kContentScripts)) {
    ListValue* list_value;
    if (!source.GetList(keys::kContentScripts, &list_value)) {
      *error = errors::kInvalidContentScriptsList;
      return false;
    }

    for (size_t i = 0; i < list_value->GetSize(); ++i) {
      DictionaryValue* content_script = NULL;
      if (!list_value->GetDictionary(i, &content_script)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidContentScript, base::IntToString(i));
        return false;
      }

      UserScript script;
      if (!LoadUserScriptHelper(content_script, i, flags, error, &script))
        return false;  // Failed to parse script context definition.
      script.set_extension_id(id());
      if (converted_from_user_script_) {
        script.set_emulate_greasemonkey(true);
        script.set_match_all_frames(true);  // Greasemonkey matches all frames.
      }
      content_scripts_.push_back(script);
    }
  }

  DictionaryValue* page_action_value = NULL;

  if (source.HasKey(keys::kPageActions)) {
    ListValue* list_value = NULL;
    if (!source.GetList(keys::kPageActions, &list_value)) {
      *error = errors::kInvalidPageActionsList;
      return false;
    }

    size_t list_value_length = list_value->GetSize();

    if (list_value_length == 0u) {
    } else if (list_value_length == 1u) {
      if (!list_value->GetDictionary(0, &page_action_value)) {
        *error = errors::kInvalidPageAction;
        return false;
      }
    } else {  // list_value_length > 1u.
      *error = errors::kInvalidPageActionsListSize;
      return false;
    }
  } else if (source.HasKey(keys::kPageAction)) {
    if (!source.GetDictionary(keys::kPageAction, &page_action_value)) {
      *error = errors::kInvalidPageAction;
      return false;
    }
  }

  if (page_action_value) {
    page_action_.reset(
        LoadExtensionActionHelper(page_action_value, error));
    if (!page_action_.get())
      return false;  // Failed to parse page action definition.
  }

  if (source.HasKey(keys::kBrowserAction)) {
    DictionaryValue* browser_action_value = NULL;
    if (!source.GetDictionary(keys::kBrowserAction, &browser_action_value)) {
      *error = errors::kInvalidBrowserAction;
      return false;
    }

    browser_action_.reset(
        LoadExtensionActionHelper(browser_action_value, error));
    if (!browser_action_.get())
      return false;  // Failed to parse browser action definition.
  }

  if (source.HasKey(keys::kFileBrowserHandlers)) {
    ListValue* file_browser_handlers_value = NULL;
    if (!source.GetList(keys::kFileBrowserHandlers,
                              &file_browser_handlers_value)) {
      *error = errors::kInvalidFileBrowserHandler;
      return false;
    }

    file_browser_handlers_.reset(
        LoadFileBrowserHandlers(file_browser_handlers_value, error));
    if (!file_browser_handlers_.get())
      return false;  // Failed to parse file browser actions definition.
  }

  if (!LoadIsApp(manifest_value_.get(), error) ||
      !LoadExtent(manifest_value_.get(), keys::kWebURLs,
                  &extent_,
                  errors::kInvalidWebURLs, errors::kInvalidWebURL,
                  parse_strictness, error) ||
      !EnsureNotHybridApp(manifest_value_.get(), error) ||
      !LoadLaunchURL(manifest_value_.get(), error) ||
      !LoadLaunchContainer(manifest_value_.get(), error) ||
      !LoadAppIsolation(manifest_value_.get(), error)) {
    return false;
  }

  if (source.HasKey(keys::kOptionsPage)) {
    std::string options_str;
    if (!source.GetString(keys::kOptionsPage, &options_str)) {
      *error = errors::kInvalidOptionsPage;
      return false;
    }

    if (is_hosted_app()) {
      GURL options_url(options_str);
      if (!options_url.is_valid() ||
          !(options_url.SchemeIs("http") || options_url.SchemeIs("https"))) {
        *error = errors::kInvalidOptionsPageInHostedApp;
        return false;
      }
      options_url_ = options_url;
    } else {
      GURL absolute(options_str);
      if (absolute.is_valid()) {
        *error = errors::kInvalidOptionsPageExpectUrlInPackage;
        return false;
      }
      options_url_ = GetResourceURL(options_str);
      if (!options_url_.is_valid()) {
        *error = errors::kInvalidOptionsPage;
        return false;
      }
    }
  }

  ExtensionAPIPermissionSet api_permissions;
  URLPatternSet host_permissions;

  if (source.HasKey(keys::kPermissions)) {
    ListValue* permissions = NULL;
    if (!source.GetList(keys::kPermissions, &permissions)) {
      *error = ExtensionErrorUtils::FormatErrorMessage(
          errors::kInvalidPermissions, "");
      return false;
    }

    for (size_t i = 0; i < permissions->GetSize(); ++i) {
      std::string permission_str;
      if (!permissions->GetString(i, &permission_str)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidPermission, base::IntToString(i));
        return false;
      }

      ExtensionAPIPermission* permission =
          ExtensionPermissionsInfo::GetInstance()->GetByName(permission_str);

      if (!IsComponentOnlyPermission(permission)
#ifndef NDEBUG
           && !CommandLine::ForCurrentProcess()->HasSwitch(
                 switches::kExposePrivateExtensionApi)
#endif
          ) {
        continue;
      }

      if (web_extent().is_empty() || location() == Extension::COMPONENT) {
        if (permission != NULL) {
          if (IsDisallowedExperimentalPermission(permission->id()) &&
              location() != Extension::COMPONENT) {
            *error = errors::kExperimentalFlagRequired;
            return false;
          }
          api_permissions.insert(permission->id());
          continue;
        }
      } else {
        if (permission != NULL && permission->is_hosted_app()) {
          if (IsDisallowedExperimentalPermission(permission->id())) {
            *error = errors::kExperimentalFlagRequired;
            return false;
          }
          api_permissions.insert(permission->id());
          continue;
        }
      }

      URLPattern pattern = URLPattern(CanExecuteScriptEverywhere() ?
          URLPattern::SCHEME_ALL : kValidHostPermissionSchemes);

      URLPattern::ParseResult parse_result = pattern.Parse(permission_str,
                                                           parse_strictness);
      if (parse_result == URLPattern::PARSE_SUCCESS) {
        if (!CanSpecifyHostPermission(pattern)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidPermissionScheme, base::IntToString(i));
          return false;
        }

        pattern.SetPath("/*");

        if (pattern.MatchesScheme(chrome::kFileScheme) &&
            !CanExecuteScriptEverywhere()) {
          wants_file_access_ = true;
          if (!(flags & ALLOW_FILE_ACCESS))
            pattern.set_valid_schemes(
                pattern.valid_schemes() & ~URLPattern::SCHEME_FILE);
        }

        host_permissions.AddPattern(pattern);
      }

    }
  }

  if (source.HasKey(keys::kBackground)) {
    std::string background_str;
    if (!source.GetString(keys::kBackground, &background_str)) {
      *error = errors::kInvalidBackground;
      return false;
    }

    if (is_hosted_app()) {
      if (!api_permissions.count(ExtensionAPIPermission::kBackground)) {
        *error = errors::kBackgroundPermissionNeeded;
        return false;
      }
      GURL bg_page(background_str);
      if (!bg_page.is_valid()) {
        *error = errors::kInvalidBackgroundInHostedApp;
        return false;
      }

      if (!(bg_page.SchemeIs("https") ||
           (CommandLine::ForCurrentProcess()->HasSwitch(
                switches::kAllowHTTPBackgroundPage) &&
            bg_page.SchemeIs("http")))) {
        *error = errors::kInvalidBackgroundInHostedApp;
        return false;
      }
      background_url_ = bg_page;
    } else {
      background_url_ = GetResourceURL(background_str);
    }
  }

  if (source.HasKey(keys::kDefaultLocale)) {
    if (!source.GetString(keys::kDefaultLocale, &default_locale_) ||
        !l10n_util::IsValidLocaleSyntax(default_locale_)) {
      *error = errors::kInvalidDefaultLocale;
      return false;
    }
  }

  if (source.HasKey(keys::kChromeURLOverrides)) {
    DictionaryValue* overrides = NULL;
    if (!source.GetDictionary(keys::kChromeURLOverrides, &overrides)) {
      *error = errors::kInvalidChromeURLOverrides;
      return false;
    }

    for (DictionaryValue::key_iterator iter = overrides->begin_keys();
         iter != overrides->end_keys(); ++iter) {
      std::string page = *iter;
      std::string val;
      if ((page != chrome::kChromeUINewTabHost &&
#if defined(TOUCH_UI)
           page != chrome::kChromeUIKeyboardHost &&
#endif
#if defined(OS_CHROMEOS)
           page != chrome::kChromeUIActivationMessageHost &&
#endif
           page != chrome::kChromeUIBookmarksHost &&
           page != chrome::kChromeUIHistoryHost) ||
          !overrides->GetStringWithoutPathExpansion(*iter, &val)) {
        *error = errors::kInvalidChromeURLOverrides;
        return false;
      }
      chrome_url_overrides_[page] = GetResourceURL(val);
    }

    if (overrides->size() > 1) {
      *error = errors::kMultipleOverrides;
      return false;
    }
  }

  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableExperimentalExtensionApis) &&
      source.HasKey(keys::kInputComponents)) {
    ListValue* list_value = NULL;
    if (!source.GetList(keys::kInputComponents, &list_value)) {
      *error = errors::kInvalidInputComponents;
      return false;
    }

    for (size_t i = 0; i < list_value->GetSize(); ++i) {
      DictionaryValue* module_value = NULL;
      std::string name_str;
      InputComponentType type;
      std::string id_str;
      std::string description_str;
      std::string language_str;
      std::set<std::string> layouts;
      std::string shortcut_keycode_str;
      bool shortcut_alt = false;
      bool shortcut_ctrl = false;
      bool shortcut_shift = false;

      if (!list_value->GetDictionary(i, &module_value)) {
        *error = errors::kInvalidInputComponents;
        return false;
      }

      if (!module_value->GetString(keys::kName, &name_str)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidInputComponentName, base::IntToString(i));
        return false;
      }

      std::string type_str;
      if (module_value->GetString(keys::kType, &type_str)) {
        if (type_str == "ime") {
          type = INPUT_COMPONENT_TYPE_IME;
        } else if (type_str == "virtual_keyboard") {
          type = INPUT_COMPONENT_TYPE_VIRTUAL_KEYBOARD;
        } else {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidInputComponentType, base::IntToString(i));
          return false;
        }
      } else {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidInputComponentType, base::IntToString(i));
        return false;
      }

      if (!module_value->GetString(keys::kId, &id_str)) {
        id_str = "";
      }

      if (!module_value->GetString(keys::kDescription, &description_str)) {
        *error = ExtensionErrorUtils::FormatErrorMessage(
            errors::kInvalidInputComponentDescription, base::IntToString(i));
        return false;
      }

      if (!module_value->GetString(keys::kLanguage, &language_str)) {
        language_str = "";
      }

      ListValue* layouts_value = NULL;
      if (!module_value->GetList(keys::kLayouts, &layouts_value)) {
        *error = errors::kInvalidInputComponentLayouts;
        return false;
      }

      for (size_t j = 0; j < layouts_value->GetSize(); ++j) {
        std::string layout_name_str;
        if (!layouts_value->GetString(j, &layout_name_str)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidInputComponentLayoutName, base::IntToString(i),
              base::IntToString(j));
          return false;
        }
        layouts.insert(layout_name_str);
      }

      if (module_value->HasKey(keys::kShortcutKey)) {
        DictionaryValue* shortcut_value = NULL;
        if (!module_value->GetDictionary(keys::kShortcutKey, &shortcut_value)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidInputComponentShortcutKey, base::IntToString(i));
          return false;
        }

        if (!shortcut_value->GetString(keys::kKeycode, &shortcut_keycode_str)) {
          *error = ExtensionErrorUtils::FormatErrorMessage(
              errors::kInvalidInputComponentShortcutKeycode,
              base::IntToString(i));
          return false;
        }

        if (!shortcut_value->GetBoolean(keys::kAltKey, &shortcut_alt)) {
          shortcut_alt = false;
        }

        if (!shortcut_value->GetBoolean(keys::kCtrlKey, &shortcut_ctrl)) {
          shortcut_ctrl = false;
        }

        if (!shortcut_value->GetBoolean(keys::kShiftKey, &shortcut_shift)) {
          shortcut_shift = false;
        }
      }

      input_components_.push_back(InputComponentInfo());
      input_components_.back().name = name_str;
      input_components_.back().type = type;
      input_components_.back().id = id_str;
      input_components_.back().description = description_str;
      input_components_.back().language = language_str;
      input_components_.back().layouts.insert(layouts.begin(), layouts.end());
      input_components_.back().shortcut_keycode = shortcut_keycode_str;
      input_components_.back().shortcut_alt = shortcut_alt;
      input_components_.back().shortcut_ctrl = shortcut_ctrl;
      input_components_.back().shortcut_shift = shortcut_shift;
    }
  }

  if (source.HasKey(keys::kOmnibox)) {
    if (!source.GetString(keys::kOmniboxKeyword, &omnibox_keyword_) ||
        omnibox_keyword_.empty()) {
      *error = errors::kInvalidOmniboxKeyword;
      return false;
    }
  }

  if (CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableExperimentalExtensionApis) &&
      source.HasKey(keys::kContentSecurityPolicy)) {
    std::string content_security_policy;
    if (!source.GetString(keys::kContentSecurityPolicy,
                          &content_security_policy)) {
      *error = errors::kInvalidContentSecurityPolicy;
      return false;
    }
    const char kBadCSPCharacters[] = {'\r', '\n', '\0'};
    if (content_security_policy.find_first_of(kBadCSPCharacters, 0,
                                              arraysize(kBadCSPCharacters)) !=
        std::string::npos) {
      *error = errors::kInvalidContentSecurityPolicy;
      return false;
    }
    content_security_policy_ = content_security_policy;
  }

  if (source.HasKey(keys::kDevToolsPage)) {
    std::string devtools_str;
    if (!source.GetString(keys::kDevToolsPage, &devtools_str)) {
      *error = errors::kInvalidDevToolsPage;
      return false;
    }
    if (!api_permissions.count(ExtensionAPIPermission::kExperimental)) {
      *error = errors::kDevToolsExperimental;
      return false;
    }
    devtools_url_ = GetResourceURL(devtools_str);
  }

  if (source.HasKey(keys::kSidebar)) {
    DictionaryValue* sidebar_value = NULL;
    if (!source.GetDictionary(keys::kSidebar, &sidebar_value)) {
      *error = errors::kInvalidSidebar;
      return false;
    }
    if (!api_permissions.count(ExtensionAPIPermission::kExperimental)) {
      *error = errors::kSidebarExperimental;
      return false;
    }
    sidebar_defaults_.reset(LoadExtensionSidebarDefaults(sidebar_value, error));
    if (!sidebar_defaults_.get())
      return false;  // Failed to parse sidebar definition.
   }
 
  if (source.HasKey(keys::kTtsEngine)) {
     DictionaryValue* tts_dict = NULL;
    if (!source.GetDictionary(keys::kTtsEngine, &tts_dict)) {
       *error = errors::kInvalidTts;
       return false;
     }

    if (tts_dict->HasKey(keys::kTtsVoices)) {
      ListValue* tts_voices = NULL;
      if (!tts_dict->GetList(keys::kTtsVoices, &tts_voices)) {
        *error = errors::kInvalidTtsVoices;
        return false;
      }

      for (size_t i = 0; i < tts_voices->GetSize(); i++) {
        DictionaryValue* one_tts_voice = NULL;
        if (!tts_voices->GetDictionary(i, &one_tts_voice)) {
          *error = errors::kInvalidTtsVoices;
          return false;
        }

        TtsVoice voice_data;
        if (one_tts_voice->HasKey(keys::kTtsVoicesVoiceName)) {
          if (!one_tts_voice->GetString(
                  keys::kTtsVoicesVoiceName, &voice_data.voice_name)) {
            *error = errors::kInvalidTtsVoicesVoiceName;
             return false;
           }
         }
        if (one_tts_voice->HasKey(keys::kTtsVoicesLang)) {
           if (!one_tts_voice->GetString(
                  keys::kTtsVoicesLang, &voice_data.lang) ||
              !l10n_util::IsValidLocaleSyntax(voice_data.lang)) {
            *error = errors::kInvalidTtsVoicesLang;
             return false;
           }
         }
        if (one_tts_voice->HasKey(keys::kTtsVoicesGender)) {
          if (!one_tts_voice->GetString(
                  keys::kTtsVoicesGender, &voice_data.gender) ||
              (voice_data.gender != keys::kTtsGenderMale &&
               voice_data.gender != keys::kTtsGenderFemale)) {
            *error = errors::kInvalidTtsVoicesGender;
             return false;
           }
         }
        if (one_tts_voice->HasKey(keys::kTtsVoicesEventTypes)) {
          ListValue* event_types_list;
          if (!one_tts_voice->GetList(
                  keys::kTtsVoicesEventTypes, &event_types_list)) {
            *error = errors::kInvalidTtsVoicesEventTypes;
            return false;
          }
          for (size_t i = 0; i < event_types_list->GetSize(); i++) {
            std::string event_type;
            if (!event_types_list->GetString(i, &event_type)) {
              *error = errors::kInvalidTtsVoicesEventTypes;
              return false;
            }
            if (event_type != keys::kTtsVoicesEventTypeEnd &&
                event_type != keys::kTtsVoicesEventTypeError &&
                event_type != keys::kTtsVoicesEventTypeMarker &&
                event_type != keys::kTtsVoicesEventTypeSentence &&
                event_type != keys::kTtsVoicesEventTypeStart &&
                event_type != keys::kTtsVoicesEventTypeWord) {
              *error = errors::kInvalidTtsVoicesEventTypes;
              return false;
            }
            if (voice_data.event_types.find(event_type) !=
                voice_data.event_types.end()) {
              *error = errors::kInvalidTtsVoicesEventTypes;
              return false;
            }
            voice_data.event_types.insert(event_type);
          }
        }
 
         tts_voices_.push_back(voice_data);
       }
    }
  }

  incognito_split_mode_ = is_app();
  if (source.HasKey(keys::kIncognito)) {
    std::string value;
    if (!source.GetString(keys::kIncognito, &value)) {
      *error = errors::kInvalidIncognitoBehavior;
      return false;
    }
    if (value == values::kIncognitoSpanning) {
      incognito_split_mode_ = false;
    } else if (value == values::kIncognitoSplit) {
      incognito_split_mode_ = true;
    } else {
      *error = errors::kInvalidIncognitoBehavior;
      return false;
    }
  }

  if (HasMultipleUISurfaces()) {
    *error = errors::kOneUISurfaceOnly;
    return false;
  }

  permission_set_.reset(
      new ExtensionPermissionSet(this, api_permissions, host_permissions));

  DCHECK(source.Equals(manifest_value_.get()));

  return true;
}
