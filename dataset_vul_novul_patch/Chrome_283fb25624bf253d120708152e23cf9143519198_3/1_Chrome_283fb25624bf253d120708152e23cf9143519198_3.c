 bool HasCallType(Plugin* plugin,
                  CallType call_type,
                 nacl::string call_name,
                  const char* caller) {
   uintptr_t id = plugin->browser_interface()->StringToIdentifier(call_name);
   PLUGIN_PRINTF(("ScriptableHandle::%s (id=%"NACL_PRIxPTR")\n",
                 caller, id));
  return plugin->HasMethod(id, call_type);
}
