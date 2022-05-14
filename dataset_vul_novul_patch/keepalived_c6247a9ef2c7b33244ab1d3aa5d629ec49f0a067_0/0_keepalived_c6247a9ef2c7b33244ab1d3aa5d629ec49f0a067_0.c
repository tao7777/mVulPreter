 init_global_keywords(bool global_active)
 {
	/* global definitions mapping */
	install_keyword_root("linkbeat_use_polling", use_polling_handler, global_active);
#if HAVE_DECL_CLONE_NEWNET
	install_keyword_root("net_namespace", &net_namespace_handler, global_active);
	install_keyword_root("namespace_with_ipsets", &namespace_ipsets_handler, global_active);
#endif
	install_keyword_root("use_pid_dir", &use_pid_dir_handler, global_active);
	install_keyword_root("instance", &instance_handler, global_active);
	install_keyword_root("child_wait_time", &child_wait_handler, global_active);
	install_keyword_root("global_defs", NULL, global_active);
	install_keyword("router_id", &routerid_handler);
	install_keyword("notification_email_from", &emailfrom_handler);
	install_keyword("smtp_server", &smtpserver_handler);
	install_keyword("smtp_helo_name", &smtphelo_handler);
	install_keyword("smtp_connect_timeout", &smtpto_handler);
	install_keyword("notification_email", &email_handler);
	install_keyword("smtp_alert", &smtp_alert_handler);
#ifdef _WITH_VRRP_
	install_keyword("smtp_alert_vrrp", &smtp_alert_vrrp_handler);
#endif
#ifdef _WITH_LVS_
	install_keyword("smtp_alert_checker", &smtp_alert_checker_handler);
#endif
#ifdef _WITH_VRRP_
	install_keyword("dynamic_interfaces", &dynamic_interfaces_handler);
	install_keyword("no_email_faults", &no_email_faults_handler);
	install_keyword("default_interface", &default_interface_handler);
#endif
#ifdef _WITH_LVS_
	install_keyword("lvs_timeouts", &lvs_timeouts);
	install_keyword("lvs_flush", &lvs_flush_handler);
#ifdef _WITH_VRRP_
	install_keyword("lvs_sync_daemon", &lvs_syncd_handler);
#endif
#endif
#ifdef _WITH_VRRP_
	install_keyword("vrrp_mcast_group4", &vrrp_mcast_group4_handler);
	install_keyword("vrrp_mcast_group6", &vrrp_mcast_group6_handler);
	install_keyword("vrrp_garp_master_delay", &vrrp_garp_delay_handler);
	install_keyword("vrrp_garp_master_repeat", &vrrp_garp_rep_handler);
	install_keyword("vrrp_garp_master_refresh", &vrrp_garp_refresh_handler);
	install_keyword("vrrp_garp_master_refresh_repeat", &vrrp_garp_refresh_rep_handler);
	install_keyword("vrrp_garp_lower_prio_delay", &vrrp_garp_lower_prio_delay_handler);
	install_keyword("vrrp_garp_lower_prio_repeat", &vrrp_garp_lower_prio_rep_handler);
	install_keyword("vrrp_garp_interval", &vrrp_garp_interval_handler);
	install_keyword("vrrp_gna_interval", &vrrp_gna_interval_handler);
	install_keyword("vrrp_lower_prio_no_advert", &vrrp_lower_prio_no_advert_handler);
	install_keyword("vrrp_higher_prio_send_advert", &vrrp_higher_prio_send_advert_handler);
	install_keyword("vrrp_version", &vrrp_version_handler);
	install_keyword("vrrp_iptables", &vrrp_iptables_handler);
#ifdef _HAVE_LIBIPSET_
	install_keyword("vrrp_ipsets", &vrrp_ipsets_handler);
#endif
	install_keyword("vrrp_check_unicast_src", &vrrp_check_unicast_src_handler);
	install_keyword("vrrp_skip_check_adv_addr", &vrrp_check_adv_addr_handler);
	install_keyword("vrrp_strict", &vrrp_strict_handler);
	install_keyword("vrrp_priority", &vrrp_prio_handler);
	install_keyword("vrrp_no_swap", &vrrp_no_swap_handler);
#ifdef _HAVE_SCHED_RT_
	install_keyword("vrrp_rt_priority", &vrrp_rt_priority_handler);
#if HAVE_DECL_RLIMIT_RTTIME == 1
	install_keyword("vrrp_rlimit_rtime", &vrrp_rt_rlimit_handler);
#endif
#endif
#endif
	install_keyword("notify_fifo", &global_notify_fifo);
	install_keyword("notify_fifo_script", &global_notify_fifo_script);
#ifdef _WITH_VRRP_
	install_keyword("vrrp_notify_fifo", &vrrp_notify_fifo);
	install_keyword("vrrp_notify_fifo_script", &vrrp_notify_fifo_script);
#endif
#ifdef _WITH_LVS_
	install_keyword("lvs_notify_fifo", &lvs_notify_fifo);
	install_keyword("lvs_notify_fifo_script", &lvs_notify_fifo_script);
	install_keyword("checker_priority", &checker_prio_handler);
	install_keyword("checker_no_swap", &checker_no_swap_handler);
#ifdef _HAVE_SCHED_RT_
	install_keyword("checker_rt_priority", &checker_rt_priority_handler);
#if HAVE_DECL_RLIMIT_RTTIME == 1
	install_keyword("checker_rlimit_rtime", &checker_rt_rlimit_handler);
#endif
#endif
#endif
#ifdef _WITH_BFD_
	install_keyword("bfd_priority", &bfd_prio_handler);
	install_keyword("bfd_no_swap", &bfd_no_swap_handler);
#ifdef _HAVE_SCHED_RT_
	install_keyword("bfd_rt_priority", &bfd_rt_priority_handler);
#if HAVE_DECL_RLIMIT_RTTIME == 1
	install_keyword("bfd_rlimit_rtime", &bfd_rt_rlimit_handler);
#endif
#endif
#endif
#ifdef _WITH_SNMP_
	install_keyword("snmp_socket", &snmp_socket_handler);
	install_keyword("enable_traps", &trap_handler);
#ifdef _WITH_SNMP_VRRP_
	install_keyword("enable_snmp_vrrp", &snmp_vrrp_handler);
	install_keyword("enable_snmp_keepalived", &snmp_vrrp_handler);	/* Deprecated v2.0.0 */
#endif
#ifdef _WITH_SNMP_RFC_
	install_keyword("enable_snmp_rfc", &snmp_rfc_handler);
#endif
#ifdef _WITH_SNMP_RFCV2_
	install_keyword("enable_snmp_rfcv2", &snmp_rfcv2_handler);
#endif
#ifdef _WITH_SNMP_RFCV3_
	install_keyword("enable_snmp_rfcv3", &snmp_rfcv3_handler);
#endif
#ifdef _WITH_SNMP_CHECKER_
	install_keyword("enable_snmp_checker", &snmp_checker_handler);
#endif
#endif
#ifdef _WITH_DBUS_
	install_keyword("enable_dbus", &enable_dbus_handler);
	install_keyword("dbus_service_name", &dbus_service_name_handler);
#endif
	install_keyword("script_user", &script_user_handler);
	install_keyword("enable_script_security", &script_security_handler);
#ifdef _WITH_VRRP_
	install_keyword("vrrp_netlink_cmd_rcv_bufs", &vrrp_netlink_cmd_rcv_bufs_handler);
	install_keyword("vrrp_netlink_cmd_rcv_bufs_force", &vrrp_netlink_cmd_rcv_bufs_force_handler);
	install_keyword("vrrp_netlink_monitor_rcv_bufs", &vrrp_netlink_monitor_rcv_bufs_handler);
	install_keyword("vrrp_netlink_monitor_rcv_bufs_force", &vrrp_netlink_monitor_rcv_bufs_force_handler);
#endif
#ifdef _WITH_LVS_
	install_keyword("lvs_netlink_cmd_rcv_bufs", &lvs_netlink_cmd_rcv_bufs_handler);
	install_keyword("lvs_netlink_cmd_rcv_bufs_force", &lvs_netlink_cmd_rcv_bufs_force_handler);
	install_keyword("lvs_netlink_monitor_rcv_bufs", &lvs_netlink_monitor_rcv_bufs_handler);
	install_keyword("lvs_netlink_monitor_rcv_bufs_force", &lvs_netlink_monitor_rcv_bufs_force_handler);
#endif
#ifdef _WITH_LVS_
	install_keyword("rs_init_notifies", &rs_init_notifies_handler);
	install_keyword("no_checker_emails", &no_checker_emails_handler);
#endif
#ifdef _WITH_VRRP_
 	install_keyword("vrrp_rx_bufs_policy", &vrrp_rx_bufs_policy_handler);
 	install_keyword("vrrp_rx_bufs_multiplier", &vrrp_rx_bufs_multiplier_handler);
 #endif
	install_keyword("umask", &umask_handler);
 }
