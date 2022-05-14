static void webkit_web_view_settings_notify(WebKitWebSettings* webSettings, GParamSpec* pspec, WebKitWebView* webView)
{
    Settings* settings = core(webView)->settings();

    const gchar* name = g_intern_string(pspec->name);
    GValue value = { 0, { { 0 } } };
    g_value_init(&value, pspec->value_type);
    g_object_get_property(G_OBJECT(webSettings), name, &value);

    if (name == g_intern_string("default-encoding"))
        settings->setDefaultTextEncodingName(g_value_get_string(&value));
    else if (name == g_intern_string("cursive-font-family"))
        settings->setCursiveFontFamily(g_value_get_string(&value));
    else if (name == g_intern_string("default-font-family"))
        settings->setStandardFontFamily(g_value_get_string(&value));
    else if (name == g_intern_string("fantasy-font-family"))
        settings->setFantasyFontFamily(g_value_get_string(&value));
    else if (name == g_intern_string("monospace-font-family"))
        settings->setFixedFontFamily(g_value_get_string(&value));
    else if (name == g_intern_string("sans-serif-font-family"))
        settings->setSansSerifFontFamily(g_value_get_string(&value));
    else if (name == g_intern_string("serif-font-family"))
        settings->setSerifFontFamily(g_value_get_string(&value));
    else if (name == g_intern_string("default-font-size"))
        settings->setDefaultFontSize(pixelsFromSize(webView, g_value_get_int(&value)));
    else if (name == g_intern_string("default-monospace-font-size"))
        settings->setDefaultFixedFontSize(pixelsFromSize(webView, g_value_get_int(&value)));
    else if (name == g_intern_string("minimum-font-size"))
        settings->setMinimumFontSize(pixelsFromSize(webView, g_value_get_int(&value)));
    else if (name == g_intern_string("minimum-logical-font-size"))
        settings->setMinimumLogicalFontSize(pixelsFromSize(webView, g_value_get_int(&value)));
    else if (name == g_intern_string("enforce-96-dpi"))
        webkit_web_view_screen_changed(GTK_WIDGET(webView), NULL);
    else if (name == g_intern_string("auto-load-images"))
        settings->setLoadsImagesAutomatically(g_value_get_boolean(&value));
    else if (name == g_intern_string("auto-shrink-images"))
        settings->setShrinksStandaloneImagesToFit(g_value_get_boolean(&value));
    else if (name == g_intern_string("print-backgrounds"))
        settings->setShouldPrintBackgrounds(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-scripts"))
        settings->setJavaScriptEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-plugins"))
        settings->setPluginsEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-dns-prefetching"))
        settings->setDNSPrefetchingEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("resizable-text-areas"))
        settings->setTextAreasAreResizable(g_value_get_boolean(&value));
    else if (name == g_intern_string("user-stylesheet-uri"))
        settings->setUserStyleSheetLocation(KURL(KURL(), g_value_get_string(&value)));
    else if (name == g_intern_string("enable-developer-extras"))
        settings->setDeveloperExtrasEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-private-browsing"))
        settings->setPrivateBrowsingEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-caret-browsing"))
        settings->setCaretBrowsingEnabled(g_value_get_boolean(&value));
#if ENABLE(DATABASE)
    else if (name == g_intern_string("enable-html5-database")) {
        AbstractDatabase::setIsAvailable(g_value_get_boolean(&value));
    }
#endif
    else if (name == g_intern_string("enable-html5-local-storage"))
        settings->setLocalStorageEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-xss-auditor"))
        settings->setXSSAuditorEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-spatial-navigation"))
        settings->setSpatialNavigationEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-frame-flattening"))
        settings->setFrameFlatteningEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("javascript-can-open-windows-automatically"))
        settings->setJavaScriptCanOpenWindowsAutomatically(g_value_get_boolean(&value));
    else if (name == g_intern_string("javascript-can-access-clipboard"))
        settings->setJavaScriptCanAccessClipboard(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-offline-web-application-cache"))
        settings->setOfflineWebApplicationCacheEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("editing-behavior"))
        settings->setEditingBehaviorType(static_cast<WebCore::EditingBehaviorType>(g_value_get_enum(&value)));
    else if (name == g_intern_string("enable-universal-access-from-file-uris"))
        settings->setAllowUniversalAccessFromFileURLs(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-file-access-from-file-uris"))
        settings->setAllowFileAccessFromFileURLs(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-dom-paste"))
        settings->setDOMPasteAllowed(g_value_get_boolean(&value));
    else if (name == g_intern_string("tab-key-cycles-through-elements")) {
        Page* page = core(webView);
        if (page)
            page->setTabKeyCyclesThroughElements(g_value_get_boolean(&value));
    } else if (name == g_intern_string("enable-site-specific-quirks"))
        settings->setNeedsSiteSpecificQuirks(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-page-cache"))
        settings->setUsesPageCache(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-java-applet"))
        settings->setJavaEnabled(g_value_get_boolean(&value));
    else if (name == g_intern_string("enable-hyperlink-auditing"))
        settings->setHyperlinkAuditingEnabled(g_value_get_boolean(&value));
 
 #if ENABLE(SPELLCHECK)
     else if (name == g_intern_string("spell-checking-languages")) {
        WebKit::EditorClient* client = static_cast<WebKit::EditorClient*>(core(webView)->editorClient());
        static_cast<WebKit::TextCheckerClientEnchant*>(client->textChecker())->updateSpellCheckingLanguage(g_value_get_string(&value));
     }
 #endif
 
#if ENABLE(WEBGL)
    else if (name == g_intern_string("enable-webgl"))
        settings->setWebGLEnabled(g_value_get_boolean(&value));
#endif

    else if (!g_object_class_find_property(G_OBJECT_GET_CLASS(webSettings), name))
        g_warning("Unexpected setting '%s'", name);
    g_value_unset(&value);
}
