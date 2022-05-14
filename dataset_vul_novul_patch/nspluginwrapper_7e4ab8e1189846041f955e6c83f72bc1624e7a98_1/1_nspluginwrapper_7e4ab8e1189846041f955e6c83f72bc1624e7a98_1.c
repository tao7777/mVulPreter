g_NPN_GetValue(NPP instance, NPNVariable variable, void *value)
{
  D(bug("NPN_GetValue instance=%p, variable=%d [%s]\n", instance, variable, string_of_NPNVariable(variable)));

  if (!thread_check()) {
	npw_printf("WARNING: NPN_GetValue not called from the main thread\n");
	return NPERR_INVALID_INSTANCE_ERROR;
  }

  PluginInstance *plugin = NULL;
  if (instance)
	plugin = PLUGIN_INSTANCE(instance);

  switch (variable) {
  case NPNVxDisplay:
	*(void **)value = x_display;
	break;
  case NPNVxtAppContext:
	*(void **)value = XtDisplayToApplicationContext(x_display);
	break;
  case NPNVToolkit:
	*(NPNToolkitType *)value = NPW_TOOLKIT;
	break;
#if USE_XPCOM
  case NPNVserviceManager: {
	nsIServiceManager *sm;
	int ret = NS_GetServiceManager(&sm);
	if (NS_FAILED(ret)) {
	  npw_printf("WARNING: NS_GetServiceManager failed\n");
	  return NPERR_GENERIC_ERROR;
	}
	*(nsIServiceManager **)value = sm;
	break;
  }
  case NPNVDOMWindow:
  case NPNVDOMElement:
	npw_printf("WARNING: %s is not supported by NPN_GetValue()\n", string_of_NPNVariable(variable));
	return NPERR_INVALID_PARAM;
#endif
  case NPNVnetscapeWindow:
	if (plugin == NULL) {
	  npw_printf("ERROR: NPNVnetscapeWindow requires a non NULL instance\n");
	  return NPERR_INVALID_INSTANCE_ERROR;
	}
	if (plugin->browser_toplevel == NULL) {
	  GdkNativeWindow netscape_xid = None;
	  NPError error = g_NPN_GetValue_real(instance, variable, &netscape_xid);
	  if (error != NPERR_NO_ERROR)
		return error;
	  if (netscape_xid == None)
		return NPERR_GENERIC_ERROR;
	  plugin->browser_toplevel = gdk_window_foreign_new(netscape_xid);
	  if (plugin->browser_toplevel == NULL)
		return NPERR_GENERIC_ERROR;
	}
	*((GdkNativeWindow *)value) = GDK_WINDOW_XWINDOW(plugin->browser_toplevel);
	break;
#if ALLOW_WINDOWLESS_PLUGINS
  case NPNVSupportsWindowless:
#endif
   case NPNVSupportsXEmbedBool:
   case NPNVWindowNPObject:
   case NPNVPluginElementNPObject:
 	return g_NPN_GetValue_real(instance, variable, value);
   default:
 	switch (variable & 0xff) {
	case 13: /* NPNVToolkit */
	  if (NPW_TOOLKIT == NPNVGtk2) {
		*(NPNToolkitType *)value = NPW_TOOLKIT;
		return NPERR_NO_ERROR;
	  }
	  break;
	}
	D(bug("WARNING: unhandled variable %d (%s) in NPN_GetValue()\n", variable, string_of_NPNVariable(variable)));
	return NPERR_INVALID_PARAM;
  }

  return NPERR_NO_ERROR;
}
