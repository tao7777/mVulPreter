process_demand_active(STREAM s)
 {
 	uint8 type;
 	uint16 len_src_descriptor, len_combined_caps;
	struct stream packet = *s;
 
 	/* at this point we need to ensure that we have ui created */
 	rd_create_ui();
 
 	in_uint32_le(s, g_rdp_shareid);
 	in_uint16_le(s, len_src_descriptor);
 	in_uint16_le(s, len_combined_caps);

	if (!s_check_rem(s, len_src_descriptor))
	{
		rdp_protocol_error("rdp_demand_active(), consume of source descriptor from stream would overrun", &packet);
	}
 	in_uint8s(s, len_src_descriptor);
 
 	logger(Protocol, Debug, "process_demand_active(), shareid=0x%x", g_rdp_shareid);

	rdp_process_server_caps(s, len_combined_caps);

	rdp_send_confirm_active();
	rdp_send_synchronise();
	rdp_send_control(RDP_CTL_COOPERATE);
	rdp_send_control(RDP_CTL_REQUEST_CONTROL);
	rdp_recv(&type);	/* RDP_PDU_SYNCHRONIZE */
	rdp_recv(&type);	/* RDP_CTL_COOPERATE */
	rdp_recv(&type);	/* RDP_CTL_GRANT_CONTROL */
	rdp_send_input(0, RDP_INPUT_SYNCHRONIZE, 0,
		       g_numlock_sync ? ui_get_numlock_state(read_keyboard_state()) : 0, 0);

	if (g_rdp_version >= RDP_V5)
	{
		rdp_enum_bmpcache2();
		rdp_send_fonts(3);
	}
	else
	{
		rdp_send_fonts(1);
		rdp_send_fonts(2);
	}

	rdp_recv(&type);	/* RDP_PDU_UNKNOWN 0x28 (Fonts?) */
	reset_order_state();
}
