CMD_FUNC(m_authenticate)
{
	aClient *agent_p = NULL;

	/* Failing to use CAP REQ for sasl is a protocol violation. */
	if (!SASL_SERVER || !MyConnect(sptr) || BadPtr(parv[1]) || !CHECKPROTO(sptr, PROTO_SASL))
		return 0;

	if (sptr->local->sasl_complete)
	{
		sendto_one(sptr, err_str(ERR_SASLALREADY), me.name, BadPtr(sptr->name) ? "*" : sptr->name);
 		return 0;
 	}
 
	if ((parv[1][0] == ':') || strchr(parv[1], ' '))
	{
		sendto_one(sptr, err_str(ERR_CANNOTDOCOMMAND), me.name, "*", "AUTHENTICATE", "Invalid parameter");
		return 0;
	}

 	if (strlen(parv[1]) > 400)
 	{
 		sendto_one(sptr, err_str(ERR_SASLTOOLONG), me.name, BadPtr(sptr->name) ? "*" : sptr->name);
		return 0;
	}

	if (*sptr->local->sasl_agent)
		agent_p = find_client(sptr->local->sasl_agent, NULL);

	if (agent_p == NULL)
	{
		char *addr = BadPtr(sptr->ip) ? "0" : sptr->ip;
		char *certfp = moddata_client_get(sptr, "certfp");

		sendto_server(NULL, 0, 0, ":%s SASL %s %s H %s %s",
		    me.name, SASL_SERVER, encode_puid(sptr), addr, addr);

		if (certfp)
			sendto_server(NULL, 0, 0, ":%s SASL %s %s S %s %s",
			    me.name, SASL_SERVER, encode_puid(sptr), parv[1], certfp);
		else
			sendto_server(NULL, 0, 0, ":%s SASL %s %s S %s",
			    me.name, SASL_SERVER, encode_puid(sptr), parv[1]);
	}
	else
		sendto_server(NULL, 0, 0, ":%s SASL %s %s C %s",
		    me.name, AGENT_SID(agent_p), encode_puid(sptr), parv[1]);

	sptr->local->sasl_out++;

	return 0;
}
