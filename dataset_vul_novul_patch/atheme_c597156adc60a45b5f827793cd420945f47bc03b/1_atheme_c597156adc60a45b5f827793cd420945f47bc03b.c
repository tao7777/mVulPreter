 void _moddeinit(module_unload_intent_t intent)
 {
 	service_named_unbind_command("chanserv", &cs_flags);
 }
