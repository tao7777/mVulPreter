static int peer_recv_callback(rdpTransport* transport, wStream* s, void* extra)
{
	freerdp_peer* client = (freerdp_peer*) extra;
	rdpRdp* rdp = client->context->rdp;

	switch (rdp->state)
	{
		case CONNECTION_STATE_INITIAL:
			if (!rdp_server_accept_nego(rdp, s))
				return -1;

			if (rdp->nego->selected_protocol & PROTOCOL_NLA)
			{
 				sspi_CopyAuthIdentity(&client->identity, &(rdp->nego->transport->credssp->identity));
 				IFCALLRET(client->Logon, client->authenticated, client, &client->identity, TRUE);
 				credssp_free(rdp->nego->transport->credssp);
 			}
 			else
 			{
				IFCALLRET(client->Logon, client->authenticated, client, &client->identity, FALSE);
			}

			break;

		case CONNECTION_STATE_NEGO:
			if (!rdp_server_accept_mcs_connect_initial(rdp, s))
				return -1;
			break;

		case CONNECTION_STATE_MCS_CONNECT:
			if (!rdp_server_accept_mcs_erect_domain_request(rdp, s))
				return -1;
			break;

		case CONNECTION_STATE_MCS_ERECT_DOMAIN:
			if (!rdp_server_accept_mcs_attach_user_request(rdp, s))
				return -1;
			break;

		case CONNECTION_STATE_MCS_ATTACH_USER:
			if (!rdp_server_accept_mcs_channel_join_request(rdp, s))
				return -1;
			break;

		case CONNECTION_STATE_MCS_CHANNEL_JOIN:
			if (rdp->settings->DisableEncryption)
			{
				if (!rdp_server_accept_client_keys(rdp, s))
					return -1;
				break;
			}
			rdp->state = CONNECTION_STATE_ESTABLISH_KEYS;
			/* FALLTHROUGH */

		case CONNECTION_STATE_ESTABLISH_KEYS:
			if (!rdp_server_accept_client_info(rdp, s))
				return -1;

			IFCALL(client->Capabilities, client);

			if (!rdp_send_demand_active(rdp))
				return -1;
			break;

		case CONNECTION_STATE_LICENSE:
			if (!rdp_server_accept_confirm_active(rdp, s))
			{
				/**
				 * During reactivation sequence the client might sent some input or channel data
				 * before receiving the Deactivate All PDU. We need to process them as usual.
				 */
				Stream_SetPosition(s, 0);
				return peer_recv_pdu(client, s);
			}
			break;

		case CONNECTION_STATE_ACTIVE:
			if (peer_recv_pdu(client, s) < 0)
				return -1;
			break;

		default:
			fprintf(stderr, "Invalid state %d\n", rdp->state);
			return -1;
	}

	return 0;
}
