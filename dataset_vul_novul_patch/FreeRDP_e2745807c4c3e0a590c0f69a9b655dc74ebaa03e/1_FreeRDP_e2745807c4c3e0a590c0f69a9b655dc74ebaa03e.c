BOOL license_read_scope_list(wStream* s, SCOPE_LIST* scopeList)
{
	UINT32 i;
	UINT32 scopeCount;

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;
 
 	Stream_Read_UINT32(s, scopeCount); /* ScopeCount (4 bytes) */
 
 	scopeList->count = scopeCount;
 	scopeList->array = (LICENSE_BLOB*) malloc(sizeof(LICENSE_BLOB) * scopeCount);
 
	/* ScopeArray */
	for (i = 0; i < scopeCount; i++)
	{
		scopeList->array[i].type = BB_SCOPE_BLOB;

		if (!license_read_binary_blob(s, &scopeList->array[i]))
			return FALSE;
	}

	return TRUE;
}
