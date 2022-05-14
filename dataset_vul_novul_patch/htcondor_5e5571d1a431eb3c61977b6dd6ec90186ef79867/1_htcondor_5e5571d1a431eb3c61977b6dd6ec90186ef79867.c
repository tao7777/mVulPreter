get_cred_handler(Service * /*service*/, int /*i*/, Stream *stream) {
  char * name = NULL;
  int rtnVal = FALSE;
  bool found_cred=false;
  CredentialWrapper * cred = NULL;
  char * owner = NULL;
  const char * user = NULL;
  void * data = NULL;

  ReliSock * socket = (ReliSock*)stream;

  if (!socket->triedAuthentication()) {
    CondorError errstack;
    if( ! SecMan::authenticate_sock(socket, READ, &errstack) ) {
      dprintf (D_ALWAYS, "Unable to authenticate, qutting\n");
      goto EXIT;
    }
  }

  socket->decode();

  if (!socket->code(name)) {
    dprintf (D_ALWAYS, "Error receiving credential name\n"); 
    goto EXIT;
  }

  user = socket->getFullyQualifiedUser();
  dprintf (D_ALWAYS, "Authenticated as %s\n", user);

  if (strchr (name, ':')) {

    owner = strdup (name);
    char * pColon = strchr (owner, ':');
     *pColon = '\0';
     
    sprintf (name, (char*)(pColon+sizeof(char)));
   
     if (strcmp (owner, user) != 0) { 
       dprintf (D_ALWAYS, "Requesting another user's (%s) credential %s\n", owner, name);

      if (!isSuperUser (user)) {
	dprintf (D_ALWAYS, "User %s is NOT super user, request DENIED\n", user);
	goto EXIT;
      } else {
	dprintf (D_FULLDEBUG, "User %s is super user, request GRANTED\n", user);
      }
    }

  } else {
    owner = strdup (user);
  }

  dprintf (D_ALWAYS, "sending cred %s for user %s\n", name, owner);

  credentials.Rewind();
  while (credentials.Next(cred)) {
	  if (cred->cred->GetType() == X509_CREDENTIAL_TYPE) {
		  if ((strcmp(cred->cred->GetName(), name) == 0) && 
			  (strcmp(cred->cred->GetOwner(), owner) == 0)) {
			  found_cred=true;
			  break; // found it
      }
    }
  }
  
  socket->encode();

  if (found_cred) {
    dprintf (D_FULLDEBUG, "Found cred %s\n", cred->GetStorageName());
    
    int data_size;

    
    int rc = LoadData (cred->GetStorageName(), data, data_size);
    dprintf (D_FULLDEBUG, "Credential::LoadData returned %d\n", rc);
    if (rc == 0) {
      goto EXIT;
    }
    
    socket->code (data_size);
    socket->code_bytes (data, data_size);
    dprintf (D_ALWAYS, "Credential name %s for owner %s returned to user %s\n",
			name, owner, user);
  }
  else {
    dprintf (D_ALWAYS, "Cannot find cred %s\n", name);
    int rc = CREDD_CREDENTIAL_NOT_FOUND;
    socket->code (rc);
  }

  rtnVal = TRUE;
EXIT:
  if ( name != NULL) {
	  free (name);
  }
  if ( owner != NULL) {
	  free (owner);
  }
  if ( data != NULL) {
	  free (data);
  }
  return rtnVal;
}
