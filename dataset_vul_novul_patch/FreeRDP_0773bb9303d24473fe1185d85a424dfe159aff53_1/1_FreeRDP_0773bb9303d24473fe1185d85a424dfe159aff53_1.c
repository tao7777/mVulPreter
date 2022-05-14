BOOL transport_accept_nla(rdpTransport* transport)
{
	freerdp* instance;
	rdpSettings* settings;

	if (transport->TlsIn == NULL)
		transport->TlsIn = tls_new(transport->settings);

	if (transport->TlsOut == NULL)
		transport->TlsOut = transport->TlsIn;

	transport->layer = TRANSPORT_LAYER_TLS;
	transport->TlsIn->sockfd = transport->TcpIn->sockfd;

	if (tls_accept(transport->TlsIn, transport->settings->CertificateFile, transport->settings->PrivateKeyFile) != TRUE)
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
 		fprintf(stderr, "client authentication failure\n");
 		credssp_free(transport->credssp);
 		return FALSE;
 	}
 
	/* don't free credssp module yet, we need to copy the credentials from it first */

	return TRUE;
}
