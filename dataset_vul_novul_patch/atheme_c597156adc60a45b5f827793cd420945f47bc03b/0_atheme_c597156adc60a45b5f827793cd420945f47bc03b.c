 void _moddeinit(module_unload_intent_t intent)
 {
 	service_named_unbind_command("chanserv", &cs_flags);

	hook_del_nick_can_register(check_registration_keywords);

	hook_del_user_can_register(check_registration_keywords);

	del_conf_item("ANOPE_FLAGS_COMPAT", &chansvs.me->conf_table);
 }
