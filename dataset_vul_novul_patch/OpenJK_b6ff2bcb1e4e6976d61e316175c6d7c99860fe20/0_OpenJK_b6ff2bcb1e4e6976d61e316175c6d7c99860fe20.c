void CL_Init( void ) {
	Com_Printf( "----- Client Initialization -----\n" );

	Con_Init();

	if(!com_fullyInitialized)
	{
		CL_ClearState();
		clc.state = CA_DISCONNECTED;	// no longer CA_UNINITIALIZED
		cl_oldGameSet = qfalse;
	}

	cls.realtime = 0;

	CL_InitInput();

	cl_noprint = Cvar_Get( "cl_noprint", "0", 0 );
#ifdef UPDATE_SERVER_NAME
	cl_motd = Cvar_Get( "cl_motd", "1", 0 );
#endif
	cl_autoupdate = Cvar_Get( "cl_autoupdate", "0", CVAR_ARCHIVE );

	cl_timeout = Cvar_Get( "cl_timeout", "200", 0 );

	cl_wavefilerecord = Cvar_Get( "cl_wavefilerecord", "0", CVAR_TEMP );

	cl_timeNudge = Cvar_Get( "cl_timeNudge", "0", CVAR_TEMP );
	cl_shownet = Cvar_Get( "cl_shownet", "0", CVAR_TEMP );
	cl_shownuments = Cvar_Get( "cl_shownuments", "0", CVAR_TEMP );
	cl_visibleClients = Cvar_Get( "cl_visibleClients", "0", CVAR_TEMP );
	cl_showServerCommands = Cvar_Get( "cl_showServerCommands", "0", 0 );
	cl_showSend = Cvar_Get( "cl_showSend", "0", CVAR_TEMP );
	cl_showTimeDelta = Cvar_Get( "cl_showTimeDelta", "0", CVAR_TEMP );
	cl_freezeDemo = Cvar_Get( "cl_freezeDemo", "0", CVAR_TEMP );
	rcon_client_password = Cvar_Get( "rconPassword", "", CVAR_TEMP );
	cl_activeAction = Cvar_Get( "activeAction", "", CVAR_TEMP );

	cl_timedemo = Cvar_Get( "timedemo", "0", 0 );
	cl_timedemoLog = Cvar_Get ("cl_timedemoLog", "", CVAR_ARCHIVE);
	cl_autoRecordDemo = Cvar_Get ("cl_autoRecordDemo", "0", CVAR_ARCHIVE);
	cl_aviFrameRate = Cvar_Get ("cl_aviFrameRate", "25", CVAR_ARCHIVE);
	cl_aviMotionJpeg = Cvar_Get ("cl_aviMotionJpeg", "1", CVAR_ARCHIVE);
	cl_avidemo = Cvar_Get( "cl_avidemo", "0", 0 );
	cl_forceavidemo = Cvar_Get( "cl_forceavidemo", "0", 0 );

	rconAddress = Cvar_Get( "rconAddress", "", 0 );

	cl_yawspeed = Cvar_Get( "cl_yawspeed", "140", CVAR_ARCHIVE );
	cl_pitchspeed = Cvar_Get( "cl_pitchspeed", "140", CVAR_ARCHIVE );
	cl_anglespeedkey = Cvar_Get( "cl_anglespeedkey", "1.5", 0 );

	cl_maxpackets = Cvar_Get( "cl_maxpackets", "38", CVAR_ARCHIVE );
	cl_packetdup = Cvar_Get( "cl_packetdup", "1", CVAR_ARCHIVE );

	cl_showPing = Cvar_Get( "cl_showPing", "0", CVAR_ARCHIVE );

	cl_run = Cvar_Get( "cl_run", "1", CVAR_ARCHIVE );
	cl_sensitivity = Cvar_Get( "sensitivity", "5", CVAR_ARCHIVE );
	cl_mouseAccel = Cvar_Get( "cl_mouseAccel", "0", CVAR_ARCHIVE );
	cl_freelook = Cvar_Get( "cl_freelook", "1", CVAR_ARCHIVE );

	cl_mouseAccelStyle = Cvar_Get( "cl_mouseAccelStyle", "0", CVAR_ARCHIVE );
	cl_mouseAccelOffset = Cvar_Get( "cl_mouseAccelOffset", "5", CVAR_ARCHIVE );
	Cvar_CheckRange(cl_mouseAccelOffset, 0.001f, 50000.0f, qfalse);

	cl_showMouseRate = Cvar_Get( "cl_showmouserate", "0", 0 );
 
 	cl_allowDownload = Cvar_Get( "cl_allowDownload", "1", CVAR_ARCHIVE );
 #ifdef USE_CURL_DLOPEN
	cl_cURLLib = Cvar_Get("cl_cURLLib", DEFAULT_CURL_LIB, CVAR_ARCHIVE | CVAR_PROTECTED);
 #endif
 
	Cvar_Get( "cg_autoswitch", "0", CVAR_ARCHIVE );

	Cvar_Get( "cg_wolfparticles", "1", CVAR_ARCHIVE );

	cl_conXOffset = Cvar_Get( "cl_conXOffset", "0", 0 );
	cl_inGameVideo = Cvar_Get( "r_inGameVideo", "1", CVAR_ARCHIVE );

	cl_serverStatusResendTime = Cvar_Get( "cl_serverStatusResendTime", "750", 0 );

	cl_recoilPitch = Cvar_Get( "cg_recoilPitch", "0", CVAR_ROM );

	cl_bypassMouseInput = Cvar_Get( "cl_bypassMouseInput", "0", 0 ); //CVAR_ROM );			// NERVE - SMF

	m_pitch = Cvar_Get( "m_pitch", "0.022", CVAR_ARCHIVE );
	m_yaw = Cvar_Get( "m_yaw", "0.022", CVAR_ARCHIVE );
	m_forward = Cvar_Get( "m_forward", "0.25", CVAR_ARCHIVE );
	m_side = Cvar_Get( "m_side", "0.25", CVAR_ARCHIVE );
	m_filter = Cvar_Get( "m_filter", "0", CVAR_ARCHIVE );

	j_pitch =        Cvar_Get ("j_pitch",        "0.022", CVAR_ARCHIVE);
	j_yaw =          Cvar_Get ("j_yaw",          "-0.022", CVAR_ARCHIVE);
	j_forward =      Cvar_Get ("j_forward",      "-0.25", CVAR_ARCHIVE);
	j_side =         Cvar_Get ("j_side",         "0.25", CVAR_ARCHIVE);
	j_up =           Cvar_Get ("j_up",           "0", CVAR_ARCHIVE);

	j_pitch_axis =   Cvar_Get ("j_pitch_axis",   "3", CVAR_ARCHIVE);
	j_yaw_axis =     Cvar_Get ("j_yaw_axis",     "2", CVAR_ARCHIVE);
	j_forward_axis = Cvar_Get ("j_forward_axis", "1", CVAR_ARCHIVE);
	j_side_axis =    Cvar_Get ("j_side_axis",    "0", CVAR_ARCHIVE);
	j_up_axis =      Cvar_Get ("j_up_axis",      "4", CVAR_ARCHIVE);

	Cvar_CheckRange(j_pitch_axis, 0, MAX_JOYSTICK_AXIS-1, qtrue);
	Cvar_CheckRange(j_yaw_axis, 0, MAX_JOYSTICK_AXIS-1, qtrue);
	Cvar_CheckRange(j_forward_axis, 0, MAX_JOYSTICK_AXIS-1, qtrue);
	Cvar_CheckRange(j_side_axis, 0, MAX_JOYSTICK_AXIS-1, qtrue);
	Cvar_CheckRange(j_up_axis, 0, MAX_JOYSTICK_AXIS-1, qtrue);

	cl_motdString = Cvar_Get( "cl_motdString", "", CVAR_ROM );

	Cvar_Get( "cl_maxPing", "800", CVAR_ARCHIVE );

	cl_lanForcePackets = Cvar_Get ("cl_lanForcePackets", "1", CVAR_ARCHIVE);

	cl_guid = Cvar_Get( "cl_guid", "unknown", CVAR_USERINFO | CVAR_ROM );

	cl_guidServerUniq = Cvar_Get ("cl_guidServerUniq", "1", CVAR_ARCHIVE);

	cl_consoleKeys = Cvar_Get( "cl_consoleKeys", "~ ` 0x7e 0x60", CVAR_ARCHIVE);

	Cvar_Get( "cg_drawCompass", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_drawNotifyText", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_quickMessageAlt", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_popupLimboMenu", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_descriptiveText", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_drawTeamOverlay", "2", CVAR_ARCHIVE );
	Cvar_Get( "cg_uselessNostalgia", "0", CVAR_ARCHIVE ); // JPW NERVE
	Cvar_Get( "cg_drawGun", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_cursorHints", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_voiceSpriteTime", "6000", CVAR_ARCHIVE );
	Cvar_Get( "cg_teamChatsOnly", "0", CVAR_ARCHIVE );
	Cvar_Get( "cg_noVoiceChats", "0", CVAR_ARCHIVE );
	Cvar_Get( "cg_noVoiceText", "0", CVAR_ARCHIVE );
	Cvar_Get( "cg_crosshairSize", "48", CVAR_ARCHIVE );
	Cvar_Get( "cg_drawCrosshair", "1", CVAR_ARCHIVE );
	Cvar_Get( "cg_zoomDefaultSniper", "20", CVAR_ARCHIVE );
	Cvar_Get( "cg_zoomstepsniper", "2", CVAR_ARCHIVE );

	Cvar_Get( "mp_playerType", "0", 0 );
	Cvar_Get( "mp_currentPlayerType", "0", 0 );
	Cvar_Get( "mp_weapon", "0", 0 );
	Cvar_Get( "mp_team", "0", 0 );
	Cvar_Get( "mp_currentTeam", "0", 0 );

	Cvar_Get( "name", "WolfPlayer", CVAR_USERINFO | CVAR_ARCHIVE );
	cl_rate = Cvar_Get( "rate", "25000", CVAR_USERINFO | CVAR_ARCHIVE );     // NERVE - SMF - changed from 3000
	Cvar_Get( "snaps", "20", CVAR_USERINFO | CVAR_ARCHIVE );
	Cvar_Get( "model", "multi", CVAR_USERINFO | CVAR_ARCHIVE );
	Cvar_Get( "head", "default", CVAR_USERINFO | CVAR_ARCHIVE );
	Cvar_Get( "color", "4", CVAR_USERINFO | CVAR_ARCHIVE );
	Cvar_Get( "handicap", "100", CVAR_USERINFO | CVAR_ARCHIVE );
	Cvar_Get( "sex", "male", CVAR_USERINFO | CVAR_ARCHIVE );
	Cvar_Get( "cl_anonymous", "0", CVAR_USERINFO | CVAR_ARCHIVE );

	Cvar_Get( "password", "", CVAR_USERINFO );
	Cvar_Get( "cg_predictItems", "1", CVAR_USERINFO | CVAR_ARCHIVE );

#ifdef USE_MUMBLE
	cl_useMumble = Cvar_Get ("cl_useMumble", "0", CVAR_ARCHIVE | CVAR_LATCH);
	cl_mumbleScale = Cvar_Get ("cl_mumbleScale", "0.0254", CVAR_ARCHIVE);
#endif

#ifdef USE_VOIP
	cl_voipSend = Cvar_Get ("cl_voipSend", "0", 0);
	cl_voipSendTarget = Cvar_Get ("cl_voipSendTarget", "spatial", 0);
	cl_voipGainDuringCapture = Cvar_Get ("cl_voipGainDuringCapture", "0.2", CVAR_ARCHIVE);
	cl_voipCaptureMult = Cvar_Get ("cl_voipCaptureMult", "2.0", CVAR_ARCHIVE);
	cl_voipUseVAD = Cvar_Get ("cl_voipUseVAD", "0", CVAR_ARCHIVE);
	cl_voipVADThreshold = Cvar_Get ("cl_voipVADThreshold", "0.25", CVAR_ARCHIVE);
	cl_voipShowMeter = Cvar_Get ("cl_voipShowMeter", "1", CVAR_ARCHIVE);

	cl_voip = Cvar_Get ("cl_voip", "1", CVAR_ARCHIVE);
	Cvar_CheckRange( cl_voip, 0, 1, qtrue );
	cl_voipProtocol = Cvar_Get ("cl_voipProtocol", cl_voip->integer ? "opus" : "", CVAR_USERINFO | CVAR_ROM);
#endif

	Cvar_Get( "cg_autoactivate", "1", CVAR_USERINFO | CVAR_ARCHIVE );

	Cvar_Get( "cg_viewsize", "100", CVAR_ARCHIVE );
	Cvar_Get ("cg_stereoSeparation", "0", CVAR_ROM);

	Cvar_Get( "cg_autoReload", "1", CVAR_ARCHIVE | CVAR_USERINFO );

	cl_missionStats = Cvar_Get( "g_missionStats", "0", CVAR_ROM );
	cl_waitForFire = Cvar_Get( "cl_waitForFire", "0", CVAR_ROM );

	cl_language = Cvar_Get( "cl_language", "0", CVAR_ARCHIVE );
	cl_debugTranslation = Cvar_Get( "cl_debugTranslation", "0", 0 );

	cl_updateavailable = Cvar_Get( "cl_updateavailable", "0", CVAR_ROM );
	cl_updatefiles = Cvar_Get( "cl_updatefiles", "", CVAR_ROM );

	Q_strncpyz( cls.autoupdateServerNames[0], AUTOUPDATE_SERVER1_NAME, MAX_QPATH );
	Q_strncpyz( cls.autoupdateServerNames[1], AUTOUPDATE_SERVER2_NAME, MAX_QPATH );
	Q_strncpyz( cls.autoupdateServerNames[2], AUTOUPDATE_SERVER3_NAME, MAX_QPATH );
	Q_strncpyz( cls.autoupdateServerNames[3], AUTOUPDATE_SERVER4_NAME, MAX_QPATH );
	Q_strncpyz( cls.autoupdateServerNames[4], AUTOUPDATE_SERVER5_NAME, MAX_QPATH );

	Cmd_AddCommand( "cmd", CL_ForwardToServer_f );
	Cmd_AddCommand( "configstrings", CL_Configstrings_f );
	Cmd_AddCommand( "clientinfo", CL_Clientinfo_f );
	Cmd_AddCommand( "snd_restart", CL_Snd_Restart_f );
	Cmd_AddCommand( "vid_restart", CL_Vid_Restart_f );
	Cmd_AddCommand( "ui_restart", CL_UI_Restart_f );          // NERVE - SMF
	Cmd_AddCommand( "disconnect", CL_Disconnect_f );
	Cmd_AddCommand( "record", CL_Record_f );
	Cmd_AddCommand( "demo", CL_PlayDemo_f );
	Cmd_SetCommandCompletionFunc( "demo", CL_CompleteDemoName );
	Cmd_AddCommand( "cinematic", CL_PlayCinematic_f );
	Cmd_AddCommand( "stoprecord", CL_StopRecord_f );
	Cmd_AddCommand( "connect", CL_Connect_f );
	Cmd_AddCommand( "reconnect", CL_Reconnect_f );
	Cmd_AddCommand( "localservers", CL_LocalServers_f );
	Cmd_AddCommand( "globalservers", CL_GlobalServers_f );
	Cmd_AddCommand( "rcon", CL_Rcon_f );
	Cmd_SetCommandCompletionFunc( "rcon", CL_CompleteRcon );
	Cmd_AddCommand( "ping", CL_Ping_f );
	Cmd_AddCommand( "serverstatus", CL_ServerStatus_f );
	Cmd_AddCommand( "showip", CL_ShowIP_f );
	Cmd_AddCommand( "fs_openedList", CL_OpenedPK3List_f );
	Cmd_AddCommand( "fs_referencedList", CL_ReferencedPK3List_f );
	Cmd_AddCommand ("video", CL_Video_f );
	Cmd_AddCommand ("stopvideo", CL_StopVideo_f );

	Cmd_AddCommand( "cache_startgather", CL_Cache_StartGather_f );
	Cmd_AddCommand( "cache_usedfile", CL_Cache_UsedFile_f );
	Cmd_AddCommand( "cache_setindex", CL_Cache_SetIndex_f );
	Cmd_AddCommand( "cache_mapchange", CL_Cache_MapChange_f );
	Cmd_AddCommand( "cache_endgather", CL_Cache_EndGather_f );

	Cmd_AddCommand( "updatehunkusage", CL_UpdateLevelHunkUsage );
	Cmd_AddCommand( "updatescreen", SCR_UpdateScreen );
	Cmd_AddCommand( "SaveTranslations", CL_SaveTranslations_f );     // NERVE - SMF - localization
	Cmd_AddCommand( "SaveNewTranslations", CL_SaveNewTranslations_f );   // NERVE - SMF - localization
	Cmd_AddCommand( "LoadTranslations", CL_LoadTranslations_f );     // NERVE - SMF - localization

	Cmd_AddCommand( "startSingleplayer", CL_startSingleplayer_f );      // NERVE - SMF

	Cmd_AddCommand( "setRecommended", CL_SetRecommended_f );

	CL_InitRef();

	SCR_Init();


	Cvar_Set( "cl_running", "1" );

	autoupdateChecked = qfalse;
	autoupdateStarted = qfalse;

	CL_InitTranslation();   // NERVE - SMF - localization

	CL_GenerateQKey();
	CL_UpdateGUID( NULL, 0 );

	Com_Printf( "----- Client Initialization Complete -----\n" );
}
