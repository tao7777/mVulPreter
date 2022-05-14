dump_global_data(FILE *fp, data_t * data)
{
#ifdef _WITH_VRRP_
	char buf[64];
#endif

	if (!data)
		return;

	conf_write(fp, "------< Global definitions >------");

#if HAVE_DECL_CLONE_NEWNET
	conf_write(fp, " Network namespace = %s", data->network_namespace ? data->network_namespace : "(default)");
#endif
	if (data->instance_name)
		conf_write(fp, " Instance name = %s", data->instance_name);
	if (data->router_id)
		conf_write(fp, " Router ID = %s", data->router_id);
	if (data->smtp_server.ss_family) {
		conf_write(fp, " Smtp server = %s", inet_sockaddrtos(&data->smtp_server));
		conf_write(fp, " Smtp server port = %u", ntohs(inet_sockaddrport(&data->smtp_server)));
	}
	if (data->smtp_helo_name)
		conf_write(fp, " Smtp HELO name = %s" , data->smtp_helo_name);
	if (data->smtp_connection_to)
		conf_write(fp, " Smtp server connection timeout = %lu"
				    , data->smtp_connection_to / TIMER_HZ);
	if (data->email_from) {
		conf_write(fp, " Email notification from = %s"
				    , data->email_from);
		dump_list(fp, data->email);
	}
	conf_write(fp, " Default smtp_alert = %s",
			data->smtp_alert == -1 ? "unset" : data->smtp_alert ? "on" : "off");
#ifdef _WITH_VRRP_
	conf_write(fp, " Default smtp_alert_vrrp = %s",
			data->smtp_alert_vrrp == -1 ? "unset" : data->smtp_alert_vrrp ? "on" : "off");
#endif
#ifdef _WITH_LVS_
	conf_write(fp, " Default smtp_alert_checker = %s",
			data->smtp_alert_checker == -1 ? "unset" : data->smtp_alert_checker ? "on" : "off");
#endif
#ifdef _WITH_VRRP_
	conf_write(fp, " Dynamic interfaces = %s", data->dynamic_interfaces ? "true" : "false");
	if (data->dynamic_interfaces)
		conf_write(fp, " Allow interface changes = %s", data->allow_if_changes ? "true" : "false");
	if (data->no_email_faults)
		conf_write(fp, " Send emails for fault transitions = off");
#endif
#ifdef _WITH_LVS_
	if (data->lvs_tcp_timeout)
		conf_write(fp, " LVS TCP timeout = %d", data->lvs_tcp_timeout);
	if (data->lvs_tcpfin_timeout)
		conf_write(fp, " LVS TCP FIN timeout = %d", data->lvs_tcpfin_timeout);
	if (data->lvs_udp_timeout)
		conf_write(fp, " LVS TCP timeout = %d", data->lvs_udp_timeout);
#ifdef _WITH_VRRP_
#ifndef _DEBUG_
	if (prog_type == PROG_TYPE_VRRP)
#endif
		conf_write(fp, " Default interface = %s", data->default_ifp ? data->default_ifp->ifname : DFLT_INT);
	if (data->lvs_syncd.vrrp) {
		conf_write(fp, " LVS syncd vrrp instance = %s"
				    , data->lvs_syncd.vrrp->iname);
		if (data->lvs_syncd.ifname)
			conf_write(fp, " LVS syncd interface = %s"
				    , data->lvs_syncd.ifname);
		conf_write(fp, " LVS syncd syncid = %u"
				    , data->lvs_syncd.syncid);
#ifdef _HAVE_IPVS_SYNCD_ATTRIBUTES_
		if (data->lvs_syncd.sync_maxlen)
			conf_write(fp, " LVS syncd maxlen = %u", data->lvs_syncd.sync_maxlen);
		if (data->lvs_syncd.mcast_group.ss_family != AF_UNSPEC)
			conf_write(fp, " LVS mcast group %s", inet_sockaddrtos(&data->lvs_syncd.mcast_group));
		if (data->lvs_syncd.mcast_port)
			conf_write(fp, " LVS syncd mcast port = %d", data->lvs_syncd.mcast_port);
		if (data->lvs_syncd.mcast_ttl)
			conf_write(fp, " LVS syncd mcast ttl = %u", data->lvs_syncd.mcast_ttl);
#endif
	}
#endif
	conf_write(fp, " LVS flush = %s", data->lvs_flush ? "true" : "false");
#endif
	if (data->notify_fifo.name) {
		conf_write(fp, " Global notify fifo = %s", data->notify_fifo.name);
		if (data->notify_fifo.script)
			conf_write(fp, " Global notify fifo script = %s, uid:gid %d:%d",
				    cmd_str(data->notify_fifo.script),
				    data->notify_fifo.script->uid,
				    data->notify_fifo.script->gid);
	}
#ifdef _WITH_VRRP_
	if (data->vrrp_notify_fifo.name) {
		conf_write(fp, " VRRP notify fifo = %s", data->vrrp_notify_fifo.name);
		if (data->vrrp_notify_fifo.script)
			conf_write(fp, " VRRP notify fifo script = %s, uid:gid %d:%d",
				    cmd_str(data->vrrp_notify_fifo.script),
				    data->vrrp_notify_fifo.script->uid,
				    data->vrrp_notify_fifo.script->gid);
	}
#endif
#ifdef _WITH_LVS_
	if (data->lvs_notify_fifo.name) {
		conf_write(fp, " LVS notify fifo = %s", data->lvs_notify_fifo.name);
		if (data->lvs_notify_fifo.script)
			conf_write(fp, " LVS notify fifo script = %s, uid:gid %d:%d",
				    cmd_str(data->lvs_notify_fifo.script),
				    data->lvs_notify_fifo.script->uid,
				    data->lvs_notify_fifo.script->gid);
	}
#endif
#ifdef _WITH_VRRP_
	if (data->vrrp_mcast_group4.sin_family) {
		conf_write(fp, " VRRP IPv4 mcast group = %s"
				    , inet_sockaddrtos((struct sockaddr_storage *)&data->vrrp_mcast_group4));
	}
	if (data->vrrp_mcast_group6.sin6_family) {
		conf_write(fp, " VRRP IPv6 mcast group = %s"
				    , inet_sockaddrtos((struct sockaddr_storage *)&data->vrrp_mcast_group6));
	}
	conf_write(fp, " Gratuitous ARP delay = %u",
		       data->vrrp_garp_delay/TIMER_HZ);
	conf_write(fp, " Gratuitous ARP repeat = %u", data->vrrp_garp_rep);
	conf_write(fp, " Gratuitous ARP refresh timer = %lu",
		       data->vrrp_garp_refresh.tv_sec);
	conf_write(fp, " Gratuitous ARP refresh repeat = %d", data->vrrp_garp_refresh_rep);
	conf_write(fp, " Gratuitous ARP lower priority delay = %d", data->vrrp_garp_lower_prio_delay == PARAMETER_UNSET ? PARAMETER_UNSET : data->vrrp_garp_lower_prio_delay / TIMER_HZ);
	conf_write(fp, " Gratuitous ARP lower priority repeat = %d", data->vrrp_garp_lower_prio_rep);
	conf_write(fp, " Send advert after receive lower priority advert = %s", data->vrrp_lower_prio_no_advert ? "false" : "true");
	conf_write(fp, " Send advert after receive higher priority advert = %s", data->vrrp_higher_prio_send_advert ? "true" : "false");
	conf_write(fp, " Gratuitous ARP interval = %d", data->vrrp_garp_interval);
	conf_write(fp, " Gratuitous NA interval = %d", data->vrrp_gna_interval);
	conf_write(fp, " VRRP default protocol version = %d", data->vrrp_version);
	if (data->vrrp_iptables_inchain[0])
		conf_write(fp," Iptables input chain = %s", data->vrrp_iptables_inchain);
	if (data->vrrp_iptables_outchain[0])
		conf_write(fp," Iptables output chain = %s", data->vrrp_iptables_outchain);
#ifdef _HAVE_LIBIPSET_
	conf_write(fp, " Using ipsets = %s", data->using_ipsets ? "true" : "false");
	if (data->vrrp_ipset_address[0])
		conf_write(fp," ipset IPv4 address set = %s", data->vrrp_ipset_address);
	if (data->vrrp_ipset_address6[0])
		conf_write(fp," ipset IPv6 address set = %s", data->vrrp_ipset_address6);
	if (data->vrrp_ipset_address_iface6[0])
		conf_write(fp," ipset IPv6 address,iface set = %s", data->vrrp_ipset_address_iface6);
#endif

	conf_write(fp, " VRRP check unicast_src = %s", data->vrrp_check_unicast_src ? "true" : "false");
	conf_write(fp, " VRRP skip check advert addresses = %s", data->vrrp_skip_check_adv_addr ? "true" : "false");
	conf_write(fp, " VRRP strict mode = %s", data->vrrp_strict ? "true" : "false");
	conf_write(fp, " VRRP process priority = %d", data->vrrp_process_priority);
	conf_write(fp, " VRRP don't swap = %s", data->vrrp_no_swap ? "true" : "false");
#ifdef _HAVE_SCHED_RT_
	conf_write(fp, " VRRP realtime priority = %u", data->vrrp_realtime_priority);
#if HAVE_DECL_RLIMIT_RTTIME
	conf_write(fp, " VRRP realtime limit = %lu", data->vrrp_rlimit_rt);
#endif
#endif
#endif
#ifdef _WITH_LVS_
	conf_write(fp, " Checker process priority = %d", data->checker_process_priority);
	conf_write(fp, " Checker don't swap = %s", data->checker_no_swap ? "true" : "false");
#ifdef _HAVE_SCHED_RT_
	conf_write(fp, " Checker realtime priority = %u", data->checker_realtime_priority);
#if HAVE_DECL_RLIMIT_RTTIME
	conf_write(fp, " Checker realtime limit = %lu", data->checker_rlimit_rt);
#endif
#endif
#endif
#ifdef _WITH_BFD_
	conf_write(fp, " BFD process priority = %d", data->bfd_process_priority);
	conf_write(fp, " BFD don't swap = %s", data->bfd_no_swap ? "true" : "false");
#ifdef _HAVE_SCHED_RT_
	conf_write(fp, " BFD realtime priority = %u", data->bfd_realtime_priority);
#if HAVE_DECL_RLIMIT_RTTIME
	conf_write(fp, " BFD realtime limit = %lu", data->bfd_rlimit_rt);
#endif
#endif
#endif
#ifdef _WITH_SNMP_VRRP_
	conf_write(fp, " SNMP vrrp %s", data->enable_snmp_vrrp ? "enabled" : "disabled");
#endif
#ifdef _WITH_SNMP_CHECKER_
	conf_write(fp, " SNMP checker %s", data->enable_snmp_checker ? "enabled" : "disabled");
#endif
#ifdef _WITH_SNMP_RFCV2_
	conf_write(fp, " SNMP RFCv2 %s", data->enable_snmp_rfcv2 ? "enabled" : "disabled");
#endif
#ifdef _WITH_SNMP_RFCV3_
	conf_write(fp, " SNMP RFCv3 %s", data->enable_snmp_rfcv3 ? "enabled" : "disabled");
#endif
#ifdef _WITH_SNMP_
	conf_write(fp, " SNMP traps %s", data->enable_traps ? "enabled" : "disabled");
	conf_write(fp, " SNMP socket = %s", data->snmp_socket ? data->snmp_socket : "default (unix:/var/agentx/master)");
#endif
#ifdef _WITH_DBUS_
	conf_write(fp, " DBus %s", data->enable_dbus ? "enabled" : "disabled");
	conf_write(fp, " DBus service name = %s", data->dbus_service_name ? data->dbus_service_name : "");
#endif
	conf_write(fp, " Script security %s", script_security ? "enabled" : "disabled");
	conf_write(fp, " Default script uid:gid %d:%d", default_script_uid, default_script_gid);
#ifdef _WITH_VRRP_
	conf_write(fp, " vrrp_netlink_cmd_rcv_bufs = %u", global_data->vrrp_netlink_cmd_rcv_bufs);
	conf_write(fp, " vrrp_netlink_cmd_rcv_bufs_force = %u", global_data->vrrp_netlink_cmd_rcv_bufs_force);
	conf_write(fp, " vrrp_netlink_monitor_rcv_bufs = %u", global_data->vrrp_netlink_monitor_rcv_bufs);
	conf_write(fp, " vrrp_netlink_monitor_rcv_bufs_force = %u", global_data->vrrp_netlink_monitor_rcv_bufs_force);
#endif
#ifdef _WITH_LVS_
	conf_write(fp, " lvs_netlink_cmd_rcv_bufs = %u", global_data->lvs_netlink_cmd_rcv_bufs);
	conf_write(fp, " lvs_netlink_cmd_rcv_bufs_force = %u", global_data->lvs_netlink_cmd_rcv_bufs_force);
	conf_write(fp, " lvs_netlink_monitor_rcv_bufs = %u", global_data->lvs_netlink_monitor_rcv_bufs);
	conf_write(fp, " lvs_netlink_monitor_rcv_bufs_force = %u", global_data->lvs_netlink_monitor_rcv_bufs_force);
	conf_write(fp, " rs_init_notifies = %u", global_data->rs_init_notifies);
	conf_write(fp, " no_checker_emails = %u", global_data->no_checker_emails);
#endif
#ifdef _WITH_VRRP_
	buf[0] = '\0';
	if (global_data->vrrp_rx_bufs_policy & RX_BUFS_POLICY_MTU)
		strcpy(buf, " rx_bufs_policy = MTU");
	else if (global_data->vrrp_rx_bufs_policy & RX_BUFS_POLICY_ADVERT)
		strcpy(buf, " rx_bufs_policy = ADVERT");
	else if (global_data->vrrp_rx_bufs_policy & RX_BUFS_SIZE)
		sprintf(buf, " rx_bufs_size = %lu", global_data->vrrp_rx_bufs_size);
 	if (buf[0])
 		conf_write(fp, "%s", buf);
 	conf_write(fp, " rx_bufs_multiples = %u", global_data->vrrp_rx_bufs_multiples);
 #endif
 }
