 SECURITY_STATUS SEC_ENTRY DeleteSecurityContext(PCtxtHandle phContext)
 {
	char* Name = NULL;
 	SECURITY_STATUS status;
 	SecurityFunctionTableA* table;
 
	Name = (char*) sspi_SecureHandleGetUpperPointer(phContext);

	if (!Name)
		return SEC_E_SECPKG_NOT_FOUND;

	table = sspi_GetSecurityFunctionTableAByNameA(Name);

	if (!table)
		return SEC_E_SECPKG_NOT_FOUND;

	if (table->DeleteSecurityContext == NULL)
		return SEC_E_UNSUPPORTED_FUNCTION;

	status = table->DeleteSecurityContext(phContext);

	return status;
}
