qboolean S_AL_Init( soundInterface_t *si )
{
#ifdef USE_OPENAL
	const char* device = NULL;
	const char* inputdevice = NULL;
	int i;

	if( !si ) {
		return qfalse;
	}

	for (i = 0; i < MAX_RAW_STREAMS; i++) {
		streamSourceHandles[i] = -1;
		streamPlaying[i] = qfalse;
		streamSources[i] = 0;
		streamNumBuffers[i] = 0;
		streamBufIndex[i] = 0;
	}

	s_alPrecache = Cvar_Get( "s_alPrecache", "1", CVAR_ARCHIVE );
	s_alGain = Cvar_Get( "s_alGain", "1.0", CVAR_ARCHIVE );
	s_alSources = Cvar_Get( "s_alSources", "96", CVAR_ARCHIVE );
	s_alDopplerFactor = Cvar_Get( "s_alDopplerFactor", "1.0", CVAR_ARCHIVE );
	s_alDopplerSpeed = Cvar_Get( "s_alDopplerSpeed", "9000", CVAR_ARCHIVE );
	s_alMinDistance = Cvar_Get( "s_alMinDistance", "120", CVAR_CHEAT );
	s_alMaxDistance = Cvar_Get("s_alMaxDistance", "1024", CVAR_CHEAT);
 	s_alRolloff = Cvar_Get( "s_alRolloff", "2", CVAR_CHEAT);
 	s_alGraceDistance = Cvar_Get("s_alGraceDistance", "512", CVAR_CHEAT);
 
	s_alDriver = Cvar_Get( "s_alDriver", ALDRIVER_DEFAULT, CVAR_ARCHIVE | CVAR_LATCH | CVAR_PROTECTED );
 
 	s_alInputDevice = Cvar_Get( "s_alInputDevice", "", CVAR_ARCHIVE | CVAR_LATCH );
 	s_alDevice = Cvar_Get("s_alDevice", "", CVAR_ARCHIVE | CVAR_LATCH);
 
	if ( COM_CompareExtension( s_alDriver->string, ".pk3" ) )
	{
		Com_Printf( "Rejecting DLL named \"%s\"", s_alDriver->string );
		return qfalse;
	}

 	if( !QAL_Init( s_alDriver->string ) )
 	{
		Com_Printf( "Failed to load library: \"%s\".\n", s_alDriver->string );
		if( !Q_stricmp( s_alDriver->string, ALDRIVER_DEFAULT ) || !QAL_Init( ALDRIVER_DEFAULT ) ) {
			return qfalse;
		}
	}

	device = s_alDevice->string;
	if(device && !*device)
		device = NULL;

	inputdevice = s_alInputDevice->string;
	if(inputdevice && !*inputdevice)
		inputdevice = NULL;


	enumeration_all_ext = qalcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT");
	enumeration_ext = qalcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");

	if(enumeration_ext || enumeration_all_ext)
	{
		char devicenames[16384] = "";
		const char *devicelist;
#ifdef _WIN32
		const char *defaultdevice;
#endif
		int curlen;

		if(enumeration_all_ext)
		{
			devicelist = qalcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
#ifdef _WIN32
			defaultdevice = qalcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
#endif
		}
		else
		{
			devicelist = qalcGetString(NULL, ALC_DEVICE_SPECIFIER);
#ifdef _WIN32
			defaultdevice = qalcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
#endif
			enumeration_ext = qtrue;
		}

#ifdef _WIN32
		if(!device && defaultdevice && !strcmp(defaultdevice, "Generic Hardware"))
			device = "Generic Software";
#endif


		if(devicelist)
		{
			while((curlen = strlen(devicelist)))
			{
				Q_strcat(devicenames, sizeof(devicenames), devicelist);
				Q_strcat(devicenames, sizeof(devicenames), "\n");

				devicelist += curlen + 1;
			}
		}

		s_alAvailableDevices = Cvar_Get("s_alAvailableDevices", devicenames, CVAR_ROM | CVAR_NORESTART);
	}

	alDevice = qalcOpenDevice(device);
	if( !alDevice && device )
	{
		Com_Printf( "Failed to open OpenAL device '%s', trying default.\n", device );
		alDevice = qalcOpenDevice(NULL);
	}

	if( !alDevice )
	{
		QAL_Shutdown( );
		Com_Printf( "Failed to open OpenAL device.\n" );
		return qfalse;
	}

	alContext = qalcCreateContext( alDevice, NULL );
	if( !alContext )
	{
		QAL_Shutdown( );
		qalcCloseDevice( alDevice );
		Com_Printf( "Failed to create OpenAL context.\n" );
		return qfalse;
	}
	qalcMakeContextCurrent( alContext );

	S_AL_BufferInit( );
	S_AL_SrcInit( );

	qalDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	qalDopplerFactor( s_alDopplerFactor->value );
	qalSpeedOfSound( s_alDopplerSpeed->value );

#ifdef USE_VOIP
	s_alCapture = Cvar_Get( "s_alCapture", "1", CVAR_ARCHIVE | CVAR_LATCH );
	if (!s_alCapture->integer)
	{
		Com_Printf("OpenAL capture support disabled by user ('+set s_alCapture 1' to enable)\n");
	}
#if USE_MUMBLE
	else if (cl_useMumble->integer)
	{
		Com_Printf("OpenAL capture support disabled for Mumble support\n");
	}
#endif
	else
	{
#ifdef __APPLE__
		if (qalcCaptureOpenDevice == NULL)
#else
		if (!qalcIsExtensionPresent(NULL, "ALC_EXT_capture"))
#endif
		{
			Com_Printf("No ALC_EXT_capture support, can't record audio.\n");
		}
		else
		{
			char inputdevicenames[16384] = "";
			const char *inputdevicelist;
			const char *defaultinputdevice;
			int curlen;

			capture_ext = qtrue;

			inputdevicelist = qalcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
			defaultinputdevice = qalcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);

			if (inputdevicelist)
			{
				while((curlen = strlen(inputdevicelist)))
				{
					Q_strcat(inputdevicenames, sizeof(inputdevicenames), inputdevicelist);
					Q_strcat(inputdevicenames, sizeof(inputdevicenames), "\n");
					inputdevicelist += curlen + 1;
				}
			}

			s_alAvailableInputDevices = Cvar_Get("s_alAvailableInputDevices", inputdevicenames, CVAR_ROM | CVAR_NORESTART);

			Com_Printf("OpenAL default capture device is '%s'\n", defaultinputdevice ? defaultinputdevice : "none");
			alCaptureDevice = qalcCaptureOpenDevice(inputdevice, 48000, AL_FORMAT_MONO16, VOIP_MAX_PACKET_SAMPLES*4);
			if( !alCaptureDevice && inputdevice )
			{
				Com_Printf( "Failed to open OpenAL Input device '%s', trying default.\n", inputdevice );
				alCaptureDevice = qalcCaptureOpenDevice(NULL, 48000, AL_FORMAT_MONO16, VOIP_MAX_PACKET_SAMPLES*4);
			}
			Com_Printf( "OpenAL capture device %s.\n",
				    (alCaptureDevice == NULL) ? "failed to open" : "opened");
		}
	}
#endif

	si->Shutdown = S_AL_Shutdown;
	si->StartSound = S_AL_StartSound;
	si->StartLocalSound = S_AL_StartLocalSound;
	si->StartBackgroundTrack = S_AL_StartBackgroundTrack;
	si->StopBackgroundTrack = S_AL_StopBackgroundTrack;
	si->RawSamples = S_AL_RawSamples;
	si->StopAllSounds = S_AL_StopAllSounds;
	si->ClearLoopingSounds = S_AL_ClearLoopingSounds;
	si->AddLoopingSound = S_AL_AddLoopingSound;
	si->AddRealLoopingSound = S_AL_AddRealLoopingSound;
	si->StopLoopingSound = S_AL_StopLoopingSound;
	si->Respatialize = S_AL_Respatialize;
	si->UpdateEntityPosition = S_AL_UpdateEntityPosition;
	si->Update = S_AL_Update;
	si->DisableSounds = S_AL_DisableSounds;
	si->BeginRegistration = S_AL_BeginRegistration;
	si->RegisterSound = S_AL_RegisterSound;
	si->ClearSoundBuffer = S_AL_ClearSoundBuffer;
	si->SoundInfo = S_AL_SoundInfo;
	si->SoundList = S_AL_SoundList;

#ifdef USE_VOIP
	si->StartCapture = S_AL_StartCapture;
	si->AvailableCaptureSamples = S_AL_AvailableCaptureSamples;
	si->Capture = S_AL_Capture;
	si->StopCapture = S_AL_StopCapture;
	si->MasterGain = S_AL_MasterGain;
#endif

	return qtrue;
#else
	return qfalse;
#endif
}
