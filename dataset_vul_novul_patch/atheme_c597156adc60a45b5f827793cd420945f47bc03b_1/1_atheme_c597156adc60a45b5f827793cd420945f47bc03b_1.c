 static void cs_cmd_flags(sourceinfo_t *si, int parc, char *parv[])
 {
	chanacs_t *ca;
	mowgli_node_t *n;
	char *channel = parv[0];
	char *target = sstrdup(parv[1]);
	char *flagstr = parv[2];
	const char *str1;
	unsigned int addflags, removeflags, restrictflags;
	hook_channel_acl_req_t req;
	mychan_t *mc;

	if (parc < 1)
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "FLAGS");
		command_fail(si, fault_needmoreparams, _("Syntax: FLAGS <channel> [target] [flags]"));
		return;
	}

	mc = mychan_find(channel);
	if (!mc)
	{
		command_fail(si, fault_nosuch_target, _("Channel \2%s\2 is not registered."), channel);
		return;
	}

	if (metadata_find(mc, "private:close:closer") && (target || !has_priv(si, PRIV_CHAN_AUSPEX)))
	{
		command_fail(si, fault_noprivs, _("\2%s\2 is closed."), channel);
		return;
	}

	if (!target || (target && target[0] == '+' && flagstr == NULL))
	{
		unsigned int flags = (target != NULL) ? flags_to_bitmask(target, 0) : 0;

		do_list(si, mc, flags);
		return;
	}

	/*
	 * following conditions are for compatibility with Anope just to avoid a whole clusterfuck
	 * of confused users caused by their 'innovation.'  yeah, that's a word for it alright.
	 *
	 * anope 1.9's shiny new FLAGS command has:
	 *
	 * FLAGS #channel LIST
	 * FLAGS #channel MODIFY user flagspec
	 * FLAGS #channel CLEAR
	 *
	 * obviously they do not support the atheme syntax, because lets face it, they like to
	 * 'innovate.'  this is, of course, hilarious for obvious reasons.  never mind that we
	 * *invented* the FLAGS system for channel ACLs, so you would think they would find it
	 * worthwhile to be compatible here.  i guess that would have been too obvious or something
	 * about their whole 'stealing our design' thing that they have been doing in 1.9 since the
	 * beginning...  or do i mean 'innovating?'
	 *
	 * anyway we rewrite the commands as appropriate in the two if blocks below so that they
	 * are processed by the flags code as the user would intend.  obviously, we're not really
	 * capable of handling the anope flag model (which makes honestly zero sense to me, and is
	 * extremely complex which kind of misses the entire point of the flags UI design...) so if
	 * some user tries passing anope flags, it will probably be hilarious.  the good news is
	 * most of the anope flags tie up to atheme flags in some weird way anyway (probably because,
	 * i don't know, they copied the entire design and then fucked it up?  yeah.  probably that.)
 	 *
 	 *   --nenolod
 	 */
	else if (!strcasecmp(target, "LIST") && myentity_find_ext(target) == NULL)
 	{
 		do_list(si, mc, 0);
 		free(target);
 
 		return;
 	}
	else if (!strcasecmp(target, "CLEAR") && myentity_find_ext(target) == NULL)
 	{
 		free(target);
 
		if (!chanacs_source_has_flag(mc, si, CA_FOUNDER))
		{
			command_fail(si, fault_noprivs, "You are not authorized to perform this operation.");
			return;
		}

		mowgli_node_t *tn;

		MOWGLI_ITER_FOREACH_SAFE(n, tn, mc->chanacs.head)
		{
			ca = n->data;

			if (ca->level & CA_FOUNDER)
				continue;

			object_unref(ca);
		}

		logcommand(si, CMDLOG_DO, "CLEAR:FLAGS: \2%s\2", mc->name);
 		command_success_nodata(si, _("Cleared flags in \2%s\2."), mc->name);
 		return;
 	}
	else if (!strcasecmp(target, "MODIFY") && myentity_find_ext(target) == NULL)
 	{
 		free(target);
 
		if (parc < 3)
		{
			command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "FLAGS");
			command_fail(si, fault_needmoreparams, _("Syntax: FLAGS <#channel> MODIFY [target] <flags>"));
			return;
		}

		flagstr = strchr(parv[2], ' ');
		if (flagstr)
			*flagstr++ = '\0';

		target = strdup(parv[2]);
	}

	{
		myentity_t *mt;

		if (!si->smu)
		{
			command_fail(si, fault_noprivs, _("You are not logged in."));
			return;
		}

		if (!flagstr)
		{
			if (!(mc->flags & MC_PUBACL) && !chanacs_source_has_flag(mc, si, CA_ACLVIEW))
			{
				command_fail(si, fault_noprivs, _("You are not authorized to execute this command."));
				return;
			}
			if (validhostmask(target))
				ca = chanacs_find_host_literal(mc, target, 0);
			else
			{
				if (!(mt = myentity_find_ext(target)))
				{
					command_fail(si, fault_nosuch_target, _("\2%s\2 is not registered."), target);
					return;
				}
				free(target);
				target = sstrdup(mt->name);
				ca = chanacs_find_literal(mc, mt, 0);
			}
			if (ca != NULL)
			{
				str1 = bitmask_to_flags2(ca->level, 0);
				command_success_string(si, str1, _("Flags for \2%s\2 in \2%s\2 are \2%s\2."),
						target, channel,
						str1);
			}
			else
				command_success_string(si, "", _("No flags for \2%s\2 in \2%s\2."),
						target, channel);
			logcommand(si, CMDLOG_GET, "FLAGS: \2%s\2 on \2%s\2", mc->name, target);
			return;
		}

		/* founder may always set flags -- jilles */
		restrictflags = chanacs_source_flags(mc, si);
		if (restrictflags & CA_FOUNDER)
			restrictflags = ca_all;
		else
		{
			if (!(restrictflags & CA_FLAGS))
			{
				/* allow a user to remove their own access
				 * even without +f */
				if (restrictflags & CA_AKICK ||
						si->smu == NULL ||
						irccasecmp(target, entity(si->smu)->name) ||
						strcmp(flagstr, "-*"))
				{
					command_fail(si, fault_noprivs, _("You are not authorized to execute this command."));
					return;
				}
			}
			if (irccasecmp(target, entity(si->smu)->name))
				restrictflags = allow_flags(mc, restrictflags);
			else
				restrictflags |= allow_flags(mc, restrictflags);
		}

		if (*flagstr == '+' || *flagstr == '-' || *flagstr == '=')
		{
			flags_make_bitmasks(flagstr, &addflags, &removeflags);
			if (addflags == 0 && removeflags == 0)
			{
				command_fail(si, fault_badparams, _("No valid flags given, use /%s%s HELP FLAGS for a list"), ircd->uses_rcommand ? "" : "msg ", chansvs.me->disp);
				return;
			}
		}
		else
		{
			addflags = get_template_flags(mc, flagstr);
			if (addflags == 0)
			{
				/* Hack -- jilles */
				if (*target == '+' || *target == '-' || *target == '=')
					command_fail(si, fault_badparams, _("Usage: FLAGS %s [target] [flags]"), mc->name);
				else
					command_fail(si, fault_badparams, _("Invalid template name given, use /%s%s TEMPLATE %s for a list"), ircd->uses_rcommand ? "" : "msg ", chansvs.me->disp, mc->name);
				return;
			}
			removeflags = ca_all & ~addflags;
		}

		if (!validhostmask(target))
		{
			if (!(mt = myentity_find_ext(target)))
			{
				command_fail(si, fault_nosuch_target, _("\2%s\2 is not registered."), target);
				return;
			}
			free(target);
			target = sstrdup(mt->name);

			ca = chanacs_open(mc, mt, NULL, true, entity(si->smu));

			if (ca->level & CA_FOUNDER && removeflags & CA_FLAGS && !(removeflags & CA_FOUNDER))
			{
				command_fail(si, fault_noprivs, _("You may not remove a founder's +f access."));
				return;
			}
			if (ca->level & CA_FOUNDER && removeflags & CA_FOUNDER && mychan_num_founders(mc) == 1)
			{
				command_fail(si, fault_noprivs, _("You may not remove the last founder."));
				return;
			}
			if (!(ca->level & CA_FOUNDER) && addflags & CA_FOUNDER)
			{
				if (mychan_num_founders(mc) >= chansvs.maxfounders)
				{
					command_fail(si, fault_noprivs, _("Only %d founders allowed per channel."), chansvs.maxfounders);
					chanacs_close(ca);
					return;
				}
				if (!myentity_can_register_channel(mt))
				{
					command_fail(si, fault_toomany, _("\2%s\2 has too many channels registered."), mt->name);
					chanacs_close(ca);
					return;
				}
				if (!myentity_allow_foundership(mt))
				{
					command_fail(si, fault_toomany, _("\2%s\2 cannot take foundership of a channel."), mt->name);
					chanacs_close(ca);
					return;
				}
			}
			if (addflags & CA_FOUNDER)
				addflags |= CA_FLAGS, removeflags &= ~CA_FLAGS;

			/* If NEVEROP is set, don't allow adding new entries
			 * except sole +b. Adding flags if the current level
			 * is +b counts as adding an entry.
			 * -- jilles */
			/* XXX: not all entities are users */
			if (isuser(mt) && (MU_NEVEROP & user(mt)->flags && addflags != CA_AKICK && addflags != 0 && (ca->level == 0 || ca->level == CA_AKICK)))
			{
				command_fail(si, fault_noprivs, _("\2%s\2 does not wish to be added to channel access lists (NEVEROP set)."), mt->name);
				chanacs_close(ca);
				return;
			}

			if (ca->level == 0 && chanacs_is_table_full(ca))
			{
				command_fail(si, fault_toomany, _("Channel %s access list is full."), mc->name);
				chanacs_close(ca);
				return;
			}

			req.ca = ca;
			req.oldlevel = ca->level;

			if (!chanacs_modify(ca, &addflags, &removeflags, restrictflags))
			{
				command_fail(si, fault_noprivs, _("You are not allowed to set \2%s\2 on \2%s\2 in \2%s\2."), bitmask_to_flags2(addflags, removeflags), mt->name, mc->name);
				chanacs_close(ca);
				return;
			}

			req.newlevel = ca->level;

			hook_call_channel_acl_change(&req);
			chanacs_close(ca);
		}
		else
		{
			if (addflags & CA_FOUNDER)
			{
		                command_fail(si, fault_badparams, _("You may not set founder status on a hostmask."));
				return;
			}

			ca = chanacs_open(mc, NULL, target, true, entity(si->smu));
			if (ca->level == 0 && chanacs_is_table_full(ca))
			{
				command_fail(si, fault_toomany, _("Channel %s access list is full."), mc->name);
				chanacs_close(ca);
				return;
			}

			req.ca = ca;
			req.oldlevel = ca->level;

			if (!chanacs_modify(ca, &addflags, &removeflags, restrictflags))
			{
		                command_fail(si, fault_noprivs, _("You are not allowed to set \2%s\2 on \2%s\2 in \2%s\2."), bitmask_to_flags2(addflags, removeflags), target, mc->name);
				chanacs_close(ca);
				return;
			}

			req.newlevel = ca->level;

			hook_call_channel_acl_change(&req);
			chanacs_close(ca);
		}

		if ((addflags | removeflags) == 0)
		{
			command_fail(si, fault_nochange, _("Channel access to \2%s\2 for \2%s\2 unchanged."), channel, target);
			return;
		}
		flagstr = bitmask_to_flags2(addflags, removeflags);
		command_success_nodata(si, _("Flags \2%s\2 were set on \2%s\2 in \2%s\2."), flagstr, target, channel);
		logcommand(si, CMDLOG_SET, "FLAGS: \2%s\2 \2%s\2 \2%s\2", mc->name, target, flagstr);
		verbose(mc, "\2%s\2 set flags \2%s\2 on \2%s\2", get_source_name(si), flagstr, target);
	}

	free(target);
}
