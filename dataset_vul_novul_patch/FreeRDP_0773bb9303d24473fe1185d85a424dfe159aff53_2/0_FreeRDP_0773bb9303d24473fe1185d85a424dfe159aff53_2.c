BOOL transport_connect_nla(rdpTransport* transport)
{
	freerdp* instance;
	rdpSettings* settings;

	if (transport->layer == TRANSPORT_LAYER_TSG)
		return TRUE;

	if (!transport_connect_tls(transport))
		return FALSE;

	/* Network Level Authentication */

	if (transport->settings->Authentication != TRUE)
		return TRUE;

	settings = transport->settings;
	instance = (freerdp*) settings->instance;

	if (transport->credssp == NULL)
		transport->credssp = credssp_new(instance, transport, settings);

	if (credssp_authenticate(transport->credssp) < 0)
	{
		if (!connectErrorCode)
			connectErrorCode = AUTHENTICATIONERROR;

		fprintf(stderr, "Authentication failure, check credentials.\n"
 			"If credentials are valid, the NTLMSSP implementation may be to blame.\n");
 
 		credssp_free(transport->credssp);
		transport->credssp = NULL;
 		return FALSE;
 	}
 
	credssp_free(transport->credssp);

	return TRUE;
}
