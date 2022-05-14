 void _modinit(module_t *m)
 {
 	service_named_bind_command("chanserv", &cs_flags);

	add_bool_conf_item("ANOPE_FLAGS_COMPAT", &chansvs.me->conf_table, 0, &anope_flags_compat, true);

	hook_add_event("nick_can_register");
	hook_add_nick_can_register(check_registration_keywords);

	hook_add_event("user_can_register");
	hook_add_user_can_register(check_registration_keywords);
 }
