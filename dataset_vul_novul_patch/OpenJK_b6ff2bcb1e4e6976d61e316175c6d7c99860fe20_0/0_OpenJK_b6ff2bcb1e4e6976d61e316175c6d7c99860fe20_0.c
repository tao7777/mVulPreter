void CL_InitRef( void ) {
	refimport_t ri;
	refexport_t *ret;
#ifdef USE_RENDERER_DLOPEN
	GetRefAPI_t		GetRefAPI;
	char			dllName[MAX_OSPATH];
#endif

 	Com_Printf( "----- Initializing Renderer ----\n" );
 
 #ifdef USE_RENDERER_DLOPEN
	cl_renderer = Cvar_Get("cl_renderer", "opengl1", CVAR_ARCHIVE | CVAR_LATCH | CVAR_PROTECTED);
 
 	Com_sprintf(dllName, sizeof(dllName), "renderer_mp_%s_" ARCH_STRING DLL_EXT, cl_renderer->string);
 
	if(!(rendererLib = Sys_LoadDll(dllName, qfalse)) && strcmp(cl_renderer->string, cl_renderer->resetString))
	{
		Com_Printf("failed:\n\"%s\"\n", Sys_LibraryError());
		Cvar_ForceReset("cl_renderer");

		Com_sprintf(dllName, sizeof(dllName), "renderer_mp_opengl1_" ARCH_STRING DLL_EXT);
		rendererLib = Sys_LoadDll(dllName, qfalse);
	}

	if(!rendererLib)
	{
		Com_Printf("failed:\n\"%s\"\n", Sys_LibraryError());
		Com_Error(ERR_FATAL, "Failed to load renderer");
	}

	GetRefAPI = Sys_LoadFunction(rendererLib, "GetRefAPI");
	if(!GetRefAPI)
	{
		Com_Error(ERR_FATAL, "Can't load symbol GetRefAPI: '%s'",  Sys_LibraryError());
	}
#endif

	ri.Cmd_AddCommand = Cmd_AddCommand;
	ri.Cmd_RemoveCommand = Cmd_RemoveCommand;
	ri.Cmd_Argc = Cmd_Argc;
	ri.Cmd_Argv = Cmd_Argv;
	ri.Cmd_ExecuteText = Cbuf_ExecuteText;
	ri.Printf = CL_RefPrintf;
	ri.Error = Com_Error;
	ri.Milliseconds = CL_ScaledMilliseconds;
#ifdef ZONE_DEBUG
	ri.Z_MallocDebug = CL_RefMallocDebug;
#else
	ri.Z_Malloc = CL_RefMalloc;
#endif
	ri.Free = Z_Free;
	ri.Tag_Free = CL_RefTagFree;
	ri.Hunk_Clear = Hunk_ClearToMark;
#ifdef HUNK_DEBUG
	ri.Hunk_AllocDebug = Hunk_AllocDebug;
#else
	ri.Hunk_Alloc = Hunk_Alloc;
#endif
	ri.Hunk_AllocateTempMemory = Hunk_AllocateTempMemory;
	ri.Hunk_FreeTempMemory = Hunk_FreeTempMemory;

	ri.CM_ClusterPVS = CM_ClusterPVS;
	ri.CM_DrawDebugSurface = CM_DrawDebugSurface;
	ri.FS_ReadFile = FS_ReadFile;
	ri.FS_FreeFile = FS_FreeFile;
	ri.FS_WriteFile = FS_WriteFile;
	ri.FS_FreeFileList = FS_FreeFileList;
	ri.FS_ListFiles = FS_ListFiles;
	ri.FS_FileIsInPAK = FS_FileIsInPAK;
	ri.FS_FileExists = FS_FileExists;
	ri.Cvar_Get = Cvar_Get;
	ri.Cvar_Set = Cvar_Set;
	ri.Cvar_SetValue = Cvar_SetValue;
	ri.Cvar_CheckRange = Cvar_CheckRange;
	ri.Cvar_VariableIntegerValue = Cvar_VariableIntegerValue;


	ri.CIN_UploadCinematic = CIN_UploadCinematic;
	ri.CIN_PlayCinematic = CIN_PlayCinematic;
	ri.CIN_RunCinematic = CIN_RunCinematic;

	ri.CL_WriteAVIVideoFrame = CL_WriteAVIVideoFrame;

	ri.IN_Init = IN_Init;
	ri.IN_Shutdown = IN_Shutdown;
	ri.IN_Restart = IN_Restart;

	ri.ftol = Q_ftol;

	ri.Sys_SetEnv = Sys_SetEnv;
	ri.Sys_GLimpSafeInit = Sys_GLimpSafeInit;
	ri.Sys_GLimpInit = Sys_GLimpInit;
	ri.Sys_LowPhysicalMemory = Sys_LowPhysicalMemory;

	ret = GetRefAPI( REF_API_VERSION, &ri );

	if ( !ret ) {
		Com_Error( ERR_FATAL, "Couldn't initialize refresh" );
	}

	re = *ret;

	Com_Printf( "---- Renderer Initialization Complete ----\n" );

	Cvar_Set( "cl_paused", "0" );
}
