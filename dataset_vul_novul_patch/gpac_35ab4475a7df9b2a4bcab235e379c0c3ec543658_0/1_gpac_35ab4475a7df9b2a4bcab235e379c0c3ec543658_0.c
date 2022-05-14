Bool GPAC_EventProc(void *ptr, GF_Event *evt)
{
	if (!term) return 0;

	if (gui_mode==1) {
		if (evt->type==GF_EVENT_QUIT) {
			Run = 0;
		} else if (evt->type==GF_EVENT_KEYDOWN) {
			switch (evt->key.key_code) {
			case GF_KEY_C:
				if (evt->key.flags & (GF_KEY_MOD_CTRL|GF_KEY_MOD_ALT)) {
					hide_shell(shell_visible ? 1 : 0);
					if (shell_visible) gui_mode=2;
				}
				break;
			default:
				break;
			}
		}
		return 0;
	}

	switch (evt->type) {
	case GF_EVENT_DURATION:
		Duration = (u64) ( 1000 * (s64) evt->duration.duration);
		CanSeek = evt->duration.can_seek;
		break;
	case GF_EVENT_MESSAGE:
	{
		const char *servName;
		if (!evt->message.service || !strcmp(evt->message.service, the_url)) {
			servName = "";
		} else if (!strnicmp(evt->message.service, "data:", 5)) {
			servName = "(embedded data)";
		} else {
			servName = evt->message.service;
		}


		if (!evt->message.message) return 0;

		if (evt->message.error) {
			if (!is_connected) last_error = evt->message.error;
			if (evt->message.error==GF_SCRIPT_INFO) {
				GF_LOG(GF_LOG_INFO, GF_LOG_CONSOLE, ("%s\n", evt->message.message));
			} else {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONSOLE, ("%s %s: %s\n", servName, evt->message.message, gf_error_to_string(evt->message.error)));
			}
		} else if (!be_quiet)
			GF_LOG(GF_LOG_INFO, GF_LOG_CONSOLE, ("%s %s\n", servName, evt->message.message));
	}
	break;
	case GF_EVENT_PROGRESS:
	{
		char *szTitle = "";
		if (evt->progress.progress_type==0) {
			szTitle = "Buffer ";
			if (bench_mode && (bench_mode!=3) ) {
				if (evt->progress.done >= evt->progress.total) bench_buffer = 0;
				else bench_buffer = 1 + 100*evt->progress.done / evt->progress.total;
				break;
			}
		}
		else if (evt->progress.progress_type==1) {
			if (bench_mode) break;
			szTitle = "Download ";
		}
		else if (evt->progress.progress_type==2) szTitle = "Import ";
		gf_set_progress(szTitle, evt->progress.done, evt->progress.total);
	}
	break;


	case GF_EVENT_DBLCLICK:
		gf_term_set_option(term, GF_OPT_FULLSCREEN, !gf_term_get_option(term, GF_OPT_FULLSCREEN));
		return 0;

	case GF_EVENT_MOUSEDOWN:
		if (evt->mouse.button==GF_MOUSE_RIGHT) {
			right_down = 1;
			last_x = evt->mouse.x;
			last_y = evt->mouse.y;
		}
		return 0;
	case GF_EVENT_MOUSEUP:
		if (evt->mouse.button==GF_MOUSE_RIGHT) {
			right_down = 0;
			last_x = evt->mouse.x;
			last_y = evt->mouse.y;
		}
		return 0;
	case GF_EVENT_MOUSEMOVE:
		if (right_down && (user.init_flags & GF_TERM_WINDOWLESS) ) {
			GF_Event move;
			move.move.x = evt->mouse.x - last_x;
			move.move.y = last_y-evt->mouse.y;
			move.type = GF_EVENT_MOVE;
			move.move.relative = 1;
			gf_term_user_event(term, &move);
		}
		return 0;

	case GF_EVENT_KEYUP:
		switch (evt->key.key_code) {
		case GF_KEY_SPACE:
			if (evt->key.flags & GF_KEY_MOD_CTRL) switch_bench(!bench_mode);
			break;
		}
		break;
	case GF_EVENT_KEYDOWN:
		gf_term_process_shortcut(term, evt);
		switch (evt->key.key_code) {
		case GF_KEY_SPACE:
			if (evt->key.flags & GF_KEY_MOD_CTRL) {
				/*ignore key repeat*/
				if (!bench_mode) switch_bench(!bench_mode);
			}
			break;
		case GF_KEY_PAGEDOWN:
		case GF_KEY_MEDIANEXTTRACK:
			request_next_playlist_item = 1;
			break;
		case GF_KEY_MEDIAPREVIOUSTRACK:
			break;
		case GF_KEY_ESCAPE:
			gf_term_set_option(term, GF_OPT_FULLSCREEN, !gf_term_get_option(term, GF_OPT_FULLSCREEN));
			break;
		case GF_KEY_C:
			if (evt->key.flags & (GF_KEY_MOD_CTRL|GF_KEY_MOD_ALT)) {
				hide_shell(shell_visible ? 1 : 0);
				if (!shell_visible) gui_mode=1;
			}
			break;
		case GF_KEY_F:
			if (evt->key.flags & GF_KEY_MOD_CTRL) fprintf(stderr, "Rendering rate: %f FPS\n", gf_term_get_framerate(term, 0));
			break;
		case GF_KEY_T:
			if (evt->key.flags & GF_KEY_MOD_CTRL) fprintf(stderr, "Scene Time: %f \n", gf_term_get_time_in_ms(term)/1000.0);
			break;
		case GF_KEY_D:
			if (evt->key.flags & GF_KEY_MOD_CTRL) gf_term_set_option(term, GF_OPT_DRAW_MODE, (gf_term_get_option(term, GF_OPT_DRAW_MODE)==GF_DRAW_MODE_DEFER) ? GF_DRAW_MODE_IMMEDIATE : GF_DRAW_MODE_DEFER );
			break;
		case GF_KEY_4:
			if (evt->key.flags & GF_KEY_MOD_CTRL)
				gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_4_3);
			break;
		case GF_KEY_5:
			if (evt->key.flags & GF_KEY_MOD_CTRL)
				gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_16_9);
			break;
		case GF_KEY_6:
			if (evt->key.flags & GF_KEY_MOD_CTRL)
				gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_FILL_SCREEN);
			break;
		case GF_KEY_7:
			if (evt->key.flags & GF_KEY_MOD_CTRL)
				gf_term_set_option(term, GF_OPT_ASPECT_RATIO, GF_ASPECT_RATIO_KEEP);
			break;
		case GF_KEY_O:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected) {
				if (gf_term_get_option(term, GF_OPT_MAIN_ADDON)) {
					fprintf(stderr, "Resuming to main content\n");
					gf_term_set_option(term, GF_OPT_PLAY_STATE, GF_STATE_PLAY_LIVE);
				} else {
					fprintf(stderr, "Main addon not enabled\n");
				}
			}
			break;
		case GF_KEY_P:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected) {
				u32 pause_state = gf_term_get_option(term, GF_OPT_PLAY_STATE) ;
				fprintf(stderr, "[Status: %s]\n", pause_state ? "Playing" : "Paused");
				if ((pause_state == GF_STATE_PAUSED) && (evt->key.flags & GF_KEY_MOD_SHIFT)) {
					gf_term_set_option(term, GF_OPT_PLAY_STATE, GF_STATE_PLAY_LIVE);
				} else {
					gf_term_set_option(term, GF_OPT_PLAY_STATE, (pause_state==GF_STATE_PAUSED) ? GF_STATE_PLAYING : GF_STATE_PAUSED);
				}
			}
			break;
		case GF_KEY_S:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected) {
				gf_term_set_option(term, GF_OPT_PLAY_STATE, GF_STATE_STEP_PAUSE);
				fprintf(stderr, "Step time: ");
				PrintTime(gf_term_get_time_in_ms(term));
				fprintf(stderr, "\n");
			}
			break;
		case GF_KEY_B:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected)
				ViewODs(term, 1);
			break;
		case GF_KEY_M:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected)
				ViewODs(term, 0);
			break;
		case GF_KEY_H:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected) {
				gf_term_switch_quality(term, 1);
			}
			break;
		case GF_KEY_L:
			if ((evt->key.flags & GF_KEY_MOD_CTRL) && is_connected) {
				gf_term_switch_quality(term, 0);
			}
			break;
		case GF_KEY_F5:
			if (is_connected)
				reload = 1;
			break;
		case GF_KEY_A:
			addon_visible = !addon_visible;
			gf_term_toggle_addons(term, addon_visible);
			break;
		case GF_KEY_UP:
			if ((evt->key.flags & VK_MOD) && is_connected) {
				do_set_speed(playback_speed * 2);
			}
			break;
		case GF_KEY_DOWN:
			if ((evt->key.flags & VK_MOD) && is_connected) {
				do_set_speed(playback_speed / 2);
			}
			break;
		case GF_KEY_LEFT:
			if ((evt->key.flags & VK_MOD) && is_connected) {
				do_set_speed(-1 * playback_speed );
			}
			break;

		}
		break;

	case GF_EVENT_CONNECT:
		if (evt->connect.is_connected) {
			is_connected = 1;
			fprintf(stderr, "Service Connected\n");
			eos_seen = GF_FALSE;
			if (playback_speed != FIX_ONE)
				gf_term_set_speed(term, playback_speed);

		} else if (is_connected) {
			fprintf(stderr, "Service %s\n", is_connected ? "Disconnected" : "Connection Failed");
			is_connected = 0;
			Duration = 0;
		}
		if (init_w && init_h) {
			gf_term_set_size(term, init_w, init_h);
		}
		ResetCaption();
		break;
	case GF_EVENT_EOS:
		eos_seen = GF_TRUE;
		if (playlist) {
			if (Duration>1500)
				request_next_playlist_item = GF_TRUE;
		}
		else if (loop_at_end) {
			restart = 1;
		}
		break;
	case GF_EVENT_SIZE:
		if (user.init_flags & GF_TERM_WINDOWLESS) {
			GF_Event move;
			move.type = GF_EVENT_MOVE;
			move.move.align_x = align_mode & 0xFF;
			move.move.align_y = (align_mode>>8) & 0xFF;
			move.move.relative = 2;
			gf_term_user_event(term, &move);
		}
		break;
	case GF_EVENT_SCENE_SIZE:
		if (forced_width && forced_height) {
			GF_Event size;
			size.type = GF_EVENT_SIZE;
			size.size.width = forced_width;
			size.size.height = forced_height;
			gf_term_user_event(term, &size);
		}
		break;

	case GF_EVENT_METADATA:
		ResetCaption();
		break;

	case GF_EVENT_RELOAD:
		if (is_connected)
			reload = 1;
		break;
	case GF_EVENT_DROPFILE:
	{
		u32 i, pos;
		/*todo - force playlist mode*/
		if (readonly_playlist) {
			gf_fclose(playlist);
			playlist = NULL;
		}
		readonly_playlist = 0;
		if (!playlist) {
			readonly_playlist = 0;
			playlist = gf_temp_file_new(NULL);
		}
		pos = ftell(playlist);
		i=0;
		while (i<evt->open_file.nb_files) {
			if (evt->open_file.files[i] != NULL) {
				fprintf(playlist, "%s\n", evt->open_file.files[i]);
			}
			i++;
		}
		fseek(playlist, pos, SEEK_SET);
		request_next_playlist_item = 1;
	}
	return 1;

	case GF_EVENT_QUIT:
		if (evt->message.error)  {
			fprintf(stderr, "A fatal error was encoutered: %s (%s) - exiting ...\n", evt->message.message ? evt->message.message : "no details", gf_error_to_string(evt->message.error) );
		}
		Run = 0;
		break;
	case GF_EVENT_DISCONNECT:
		gf_term_disconnect(term);
		break;
	case GF_EVENT_MIGRATE:
	{
	}
	break;
	case GF_EVENT_NAVIGATE_INFO:
		if (evt->navigate.to_url) fprintf(stderr, "Go to URL: \"%s\"\r", evt->navigate.to_url);
 		break;
 	case GF_EVENT_NAVIGATE:
 		if (gf_term_is_supported_url(term, evt->navigate.to_url, 1, no_mime_check)) {
			strcpy(the_url, evt->navigate.to_url);
 			fprintf(stderr, "Navigating to URL %s\n", the_url);
 			gf_term_navigate_to(term, evt->navigate.to_url);
 			return 1;
		} else {
			fprintf(stderr, "Navigation destination not supported\nGo to URL: %s\n", evt->navigate.to_url);
		}
		break;
	case GF_EVENT_SET_CAPTION:
		gf_term_user_event(term, evt);
		break;
	case GF_EVENT_AUTHORIZATION:
	{
		int maxTries = 1;
		assert( evt->type == GF_EVENT_AUTHORIZATION);
		assert( evt->auth.user);
		assert( evt->auth.password);
		assert( evt->auth.site_url);
		while ((!strlen(evt->auth.user) || !strlen(evt->auth.password)) && (maxTries--) >= 0) {
			fprintf(stderr, "**** Authorization required for site %s ****\n", evt->auth.site_url);
			fprintf(stderr, "login   : ");
			read_line_input(evt->auth.user, 50, 1);
			fprintf(stderr, "\npassword: ");
			read_line_input(evt->auth.password, 50, 0);
			fprintf(stderr, "*********\n");
		}
		if (maxTries < 0) {
			fprintf(stderr, "**** No User or password has been filled, aborting ***\n");
			return 0;
		}
		return 1;
	}
	case GF_EVENT_ADDON_DETECTED:
		if (enable_add_ons) {
			fprintf(stderr, "Media Addon %s detected - enabling it\n", evt->addon_connect.addon_url);
			addon_visible = 1;
		}
		return enable_add_ons;
	}
	return 0;
}
