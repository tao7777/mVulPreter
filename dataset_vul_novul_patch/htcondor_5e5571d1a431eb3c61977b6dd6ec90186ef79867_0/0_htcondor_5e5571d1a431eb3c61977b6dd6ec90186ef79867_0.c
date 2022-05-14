rm_cred_handler(Service * /*service*/, int /*i*/, Stream *stream) {
  char * name = NULL;
  int rtnVal = FALSE;
  int rc;
  bool found_cred;
  CredentialWrapper * cred_wrapper = NULL;
  char * owner = NULL;
  const char * user;

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
     
    sprintf (name, "%s", (char*)(pColon+sizeof(char)));
   
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

  dprintf (D_ALWAYS, "Attempting to delete cred %s for user %s\n", name, owner);
  

  found_cred=false;
  credentials.Rewind();
  while (credentials.Next(cred_wrapper)) {
	  if (cred_wrapper->cred->GetType() == X509_CREDENTIAL_TYPE) {
		  if ((strcmp(cred_wrapper->cred->GetName(), name) == 0) && 
			  (strcmp(cred_wrapper->cred->GetOwner(), owner) == 0)) {
			  credentials.DeleteCurrent();
			  found_cred=true;
			  break; // found it
		  }
	  }
  }


  if (found_cred) {
    priv_state priv = set_root_priv();
    unlink (cred_wrapper->GetStorageName());
    SaveCredentialList();
    set_priv(priv);
    delete cred_wrapper;
    dprintf (D_ALWAYS, "Removed credential %s for owner %s\n", name, owner);
  } else {
    dprintf (D_ALWAYS, "Unable to remove credential %s:%s (not found)\n", owner, name); 
  }
	    
  
  free (owner);
  
  socket->encode();
 

  rc = (found_cred)?CREDD_SUCCESS:CREDD_CREDENTIAL_NOT_FOUND;
  socket->code(rc);

  rtnVal = TRUE;

EXIT:
  if (name != NULL) {
	  free (name);
  }
  return rtnVal;
}
