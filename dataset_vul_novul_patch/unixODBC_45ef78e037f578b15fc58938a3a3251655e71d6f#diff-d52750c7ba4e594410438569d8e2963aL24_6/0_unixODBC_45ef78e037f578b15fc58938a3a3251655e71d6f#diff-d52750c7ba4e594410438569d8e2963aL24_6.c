BOOL SQLWriteFileDSN(			LPCSTR	pszFileName,
								LPCSTR	pszAppName,
								LPCSTR	pszKeyName,
								LPCSTR	pszString )
{
	HINI	hIni;
	char	szFileName[ODBC_FILENAME_MAX+1];
 
 	if ( pszFileName[0] == '/' )
 	{
		strncpy( szFileName, pszFileName, sizeof(szFileName) - 5 );
 	}
 	else
 	{	
		char szPath[ODBC_FILENAME_MAX+1];
		*szPath = '\0';
		_odbcinst_FileINI( szPath );
		snprintf( szFileName, sizeof(szFileName) - 5, "%s/%s", szPath, pszFileName );
	}

    if ( strlen( szFileName ) < 4 || strcmp( szFileName + strlen( szFileName ) - 4, ".dsn" ))
    {
        strcat( szFileName, ".dsn" );
    }

#ifdef __OS2__
	if ( iniOpen( &hIni, szFileName, "#;", '[', ']', '=', TRUE, 0L ) != INI_SUCCESS )
#else
	if ( iniOpen( &hIni, szFileName, "#;", '[', ']', '=', TRUE ) != INI_SUCCESS )
#endif
	{
       	inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_INVALID_PATH, "" );
		return FALSE;
	}

	/* delete section */
	if ( pszString == NULL && pszKeyName == NULL )
	{
		if ( iniObjectSeek( hIni, (char *)pszAppName ) == INI_SUCCESS )
        {
			iniObjectDelete( hIni );
        }
	}
	/* delete entry */
	else if	( pszString == NULL )
	{
		if ( iniPropertySeek( hIni, (char *)pszAppName, (char *)pszKeyName, "" ) == INI_SUCCESS )
        {
			iniPropertyDelete( hIni );
        }
	}
	else
	{
		/* add section */
		if ( iniObjectSeek( hIni, (char *)pszAppName ) != INI_SUCCESS )
        {
			iniObjectInsert( hIni, (char *)pszAppName );
        }
		/* update entry */
		if ( iniPropertySeek( hIni, (char *)pszAppName, (char *)pszKeyName, "" ) == INI_SUCCESS )
		{
			iniObjectSeek( hIni, (char *)pszAppName );
			iniPropertyUpdate( hIni, (char *)pszKeyName, (char *)pszString );
		}
		/* add entry */
		else
		{
			iniObjectSeek( hIni, (char *)pszAppName );
			iniPropertyInsert( hIni, (char *)pszKeyName, (char *)pszString );
		}
	}

	if ( iniCommit( hIni ) != INI_SUCCESS )
	{
		iniClose( hIni );
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_REQUEST_FAILED, "" );
		return FALSE;
	}

	iniClose( hIni );

	return TRUE;
}
