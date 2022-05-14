ProcEstablishConnection(ClientPtr client)
{
    const char *reason;
    char *auth_proto, *auth_string;
    xConnClientPrefix *prefix;

    REQUEST(xReq);

     prefix = (xConnClientPrefix *) ((char *) stuff + sz_xReq);
     auth_proto = (char *) prefix + sz_xConnClientPrefix;
     auth_string = auth_proto + pad_to_int32(prefix->nbytesAuthProto);

    if ((client->req_len << 2) != sz_xReq + sz_xConnClientPrefix +
	pad_to_int32(prefix->nbytesAuthProto) +
	pad_to_int32(prefix->nbytesAuthString))
        reason = "Bad length";
    else if ((prefix->majorVersion != X_PROTOCOL) ||
         (prefix->minorVersion != X_PROTOCOL_REVISION))
         reason = "Protocol version mismatch";
     else

    return (SendConnSetup(client, reason));
}
