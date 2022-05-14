void Com_WriteConfig_f( void ) {
	char	filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 ) {
		Com_Printf( "Usage: writeconfig <filename>\n" );
 		return;
 	}
 

	if (!COM_CompareExtension(filename, ".cfg"))
	{
		Com_Printf("Com_WriteConfig_f: Only the \".cfg\" extension is supported by this command!\n");
		return;
	}

 	Q_strncpyz( filename, Cmd_Argv(1), sizeof( filename ) );
 	COM_DefaultExtension( filename, sizeof( filename ), ".cfg" );
 	Com_Printf( "Writing %s.\n", filename );
	Com_WriteConfigToFile( filename );
}
