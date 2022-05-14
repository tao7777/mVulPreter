m_authenticate(struct Client *client_p, struct Client *source_p,
	int parc, const char *parv[])
{
	struct Client *agent_p = NULL;
	struct Client *saslserv_p = NULL;

	/* They really should use CAP for their own sake. */
	if(!IsCapable(source_p, CLICAP_SASL))
		return 0;

	if (strlen(client_p->id) == 3)
	{
		exit_client(client_p, client_p, client_p, "Mixing client and server protocol");
 		return 0;
 	}
 
	if (*parv[1] == ':' || strchr(parv[1], ' '))
	{
		exit_client(client_p, client_p, client_p, "Malformed AUTHENTICATE");
		return 0;
	}

 	saslserv_p = find_named_client(ConfigFileEntry.sasl_service);
 	if (saslserv_p == NULL || !IsService(saslserv_p))
 	{
		sendto_one(source_p, form_str(ERR_SASLABORTED), me.name, EmptyString(source_p->name) ? "*" : source_p->name);
		return 0;
	}

	if(source_p->localClient->sasl_complete)
	{
		*source_p->localClient->sasl_agent = '\0';
		source_p->localClient->sasl_complete = 0;
	}

	if(strlen(parv[1]) > 400)
	{
		sendto_one(source_p, form_str(ERR_SASLTOOLONG), me.name, EmptyString(source_p->name) ? "*" : source_p->name);
		return 0;
	}

	if(!*source_p->id)
	{
		/* Allocate a UID. */
		strcpy(source_p->id, generate_uid());
		add_to_id_hash(source_p->id, source_p);
	}

	if(*source_p->localClient->sasl_agent)
		agent_p = find_id(source_p->localClient->sasl_agent);

	if(agent_p == NULL)
	{
		sendto_one(saslserv_p, ":%s ENCAP %s SASL %s %s H %s %s",
					me.id, saslserv_p->servptr->name, source_p->id, saslserv_p->id,
					source_p->host, source_p->sockhost);

		if (!strcmp(parv[1], "EXTERNAL") && source_p->certfp != NULL)
			sendto_one(saslserv_p, ":%s ENCAP %s SASL %s %s S %s %s",
						me.id, saslserv_p->servptr->name, source_p->id, saslserv_p->id,
						parv[1], source_p->certfp);
		else
			sendto_one(saslserv_p, ":%s ENCAP %s SASL %s %s S %s",
						me.id, saslserv_p->servptr->name, source_p->id, saslserv_p->id,
						parv[1]);

		rb_strlcpy(source_p->localClient->sasl_agent, saslserv_p->id, IDLEN);
	}
	else
		sendto_one(agent_p, ":%s ENCAP %s SASL %s %s C %s",
				me.id, agent_p->servptr->name, source_p->id, agent_p->id,
				parv[1]);
	source_p->localClient->sasl_out++;

	return 0;
}
