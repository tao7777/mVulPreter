SYSCALL_DEFINE5(perf_event_open,
		struct perf_event_attr __user *, attr_uptr,
		pid_t, pid, int, cpu, int, group_fd, unsigned long, flags)
{
	struct perf_event *group_leader = NULL, *output_event = NULL;
	struct perf_event *event, *sibling;
	struct perf_event_attr attr;
	struct perf_event_context *ctx, *uninitialized_var(gctx);
	struct file *event_file = NULL;
	struct fd group = {NULL, 0};
	struct task_struct *task = NULL;
	struct pmu *pmu;
	int event_fd;
	int move_group = 0;
	int err;
	int f_flags = O_RDWR;
	int cgroup_fd = -1;

	/* for future expandability... */
	if (flags & ~PERF_FLAG_ALL)
		return -EINVAL;

	err = perf_copy_attr(attr_uptr, &attr);
	if (err)
		return err;

	if (!attr.exclude_kernel) {
		if (perf_paranoid_kernel() && !capable(CAP_SYS_ADMIN))
			return -EACCES;
	}

	if (attr.freq) {
		if (attr.sample_freq > sysctl_perf_event_sample_rate)
			return -EINVAL;
	} else {
		if (attr.sample_period & (1ULL << 63))
			return -EINVAL;
	}

	if (!attr.sample_max_stack)
		attr.sample_max_stack = sysctl_perf_event_max_stack;

	/*
	 * In cgroup mode, the pid argument is used to pass the fd
	 * opened to the cgroup directory in cgroupfs. The cpu argument
	 * designates the cpu on which to monitor threads from that
	 * cgroup.
	 */
	if ((flags & PERF_FLAG_PID_CGROUP) && (pid == -1 || cpu == -1))
		return -EINVAL;

	if (flags & PERF_FLAG_FD_CLOEXEC)
		f_flags |= O_CLOEXEC;

	event_fd = get_unused_fd_flags(f_flags);
	if (event_fd < 0)
		return event_fd;

	if (group_fd != -1) {
		err = perf_fget_light(group_fd, &group);
		if (err)
			goto err_fd;
		group_leader = group.file->private_data;
		if (flags & PERF_FLAG_FD_OUTPUT)
			output_event = group_leader;
		if (flags & PERF_FLAG_FD_NO_GROUP)
			group_leader = NULL;
	}

	if (pid != -1 && !(flags & PERF_FLAG_PID_CGROUP)) {
		task = find_lively_task_by_vpid(pid);
		if (IS_ERR(task)) {
			err = PTR_ERR(task);
			goto err_group_fd;
		}
	}

	if (task && group_leader &&
	    group_leader->attr.inherit != attr.inherit) {
		err = -EINVAL;
		goto err_task;
	}

	get_online_cpus();

	if (task) {
		err = mutex_lock_interruptible(&task->signal->cred_guard_mutex);
		if (err)
			goto err_cpus;

		/*
		 * Reuse ptrace permission checks for now.
		 *
		 * We must hold cred_guard_mutex across this and any potential
		 * perf_install_in_context() call for this new event to
		 * serialize against exec() altering our credentials (and the
		 * perf_event_exit_task() that could imply).
		 */
		err = -EACCES;
		if (!ptrace_may_access(task, PTRACE_MODE_READ_REALCREDS))
			goto err_cred;
	}

	if (flags & PERF_FLAG_PID_CGROUP)
		cgroup_fd = pid;

	event = perf_event_alloc(&attr, cpu, task, group_leader, NULL,
				 NULL, NULL, cgroup_fd);
	if (IS_ERR(event)) {
		err = PTR_ERR(event);
		goto err_cred;
	}

	if (is_sampling_event(event)) {
		if (event->pmu->capabilities & PERF_PMU_CAP_NO_INTERRUPT) {
			err = -EOPNOTSUPP;
			goto err_alloc;
		}
	}

	/*
	 * Special case software events and allow them to be part of
	 * any hardware group.
	 */
	pmu = event->pmu;

	if (attr.use_clockid) {
		err = perf_event_set_clock(event, attr.clockid);
		if (err)
			goto err_alloc;
	}

	if (pmu->task_ctx_nr == perf_sw_context)
		event->event_caps |= PERF_EV_CAP_SOFTWARE;

	if (group_leader &&
	    (is_software_event(event) != is_software_event(group_leader))) {
		if (is_software_event(event)) {
			/*
			 * If event and group_leader are not both a software
			 * event, and event is, then group leader is not.
			 *
			 * Allow the addition of software events to !software
			 * groups, this is safe because software events never
			 * fail to schedule.
			 */
			pmu = group_leader->pmu;
		} else if (is_software_event(group_leader) &&
			   (group_leader->group_caps & PERF_EV_CAP_SOFTWARE)) {
			/*
			 * In case the group is a pure software group, and we
			 * try to add a hardware event, move the whole group to
			 * the hardware context.
			 */
			move_group = 1;
		}
	}

	/*
	 * Get the target context (task or percpu):
	 */
	ctx = find_get_context(pmu, task, event);
	if (IS_ERR(ctx)) {
		err = PTR_ERR(ctx);
		goto err_alloc;
	}

	if ((pmu->capabilities & PERF_PMU_CAP_EXCLUSIVE) && group_leader) {
		err = -EBUSY;
		goto err_context;
	}

	/*
	 * Look up the group leader (we will attach this event to it):
	 */
	if (group_leader) {
		err = -EINVAL;

		/*
		 * Do not allow a recursive hierarchy (this new sibling
		 * becoming part of another group-sibling):
		 */
		if (group_leader->group_leader != group_leader)
			goto err_context;

		/* All events in a group should have the same clock */
		if (group_leader->clock != event->clock)
			goto err_context;

		/*
		 * Do not allow to attach to a group in a different
		 * task or CPU context:
		 */
		if (move_group) {
			/*
			 * Make sure we're both on the same task, or both
			 * per-cpu events.
			 */
			if (group_leader->ctx->task != ctx->task)
				goto err_context;

			/*
			 * Make sure we're both events for the same CPU;
			 * grouping events for different CPUs is broken; since
			 * you can never concurrently schedule them anyhow.
			 */
			if (group_leader->cpu != event->cpu)
				goto err_context;
		} else {
			if (group_leader->ctx != ctx)
				goto err_context;
		}

		/*
		 * Only a group leader can be exclusive or pinned
		 */
		if (attr.exclusive || attr.pinned)
			goto err_context;
	}

	if (output_event) {
		err = perf_event_set_output(event, output_event);
		if (err)
			goto err_context;
	}

	event_file = anon_inode_getfile("[perf_event]", &perf_fops, event,
					f_flags);
	if (IS_ERR(event_file)) {
		err = PTR_ERR(event_file);
		event_file = NULL;
		goto err_context;
 	}
 
 	if (move_group) {
		gctx = __perf_event_ctx_lock_double(group_leader, ctx);

 		if (gctx->task == TASK_TOMBSTONE) {
 			err = -ESRCH;
 			goto err_locked;
 		}

		/*
		 * Check if we raced against another sys_perf_event_open() call
		 * moving the software group underneath us.
		 */
		if (!(group_leader->group_caps & PERF_EV_CAP_SOFTWARE)) {
			/*
			 * If someone moved the group out from under us, check
			 * if this new event wound up on the same ctx, if so
			 * its the regular !move_group case, otherwise fail.
			 */
			if (gctx != ctx) {
				err = -EINVAL;
				goto err_locked;
			} else {
				perf_event_ctx_unlock(group_leader, gctx);
				move_group = 0;
			}
		}
 	} else {
 		mutex_lock(&ctx->mutex);
 	}

	if (ctx->task == TASK_TOMBSTONE) {
		err = -ESRCH;
		goto err_locked;
	}

	if (!perf_event_validate_size(event)) {
		err = -E2BIG;
		goto err_locked;
	}

	/*
	 * Must be under the same ctx::mutex as perf_install_in_context(),
	 * because we need to serialize with concurrent event creation.
	 */
	if (!exclusive_event_installable(event, ctx)) {
		/* exclusive and group stuff are assumed mutually exclusive */
		WARN_ON_ONCE(move_group);

		err = -EBUSY;
		goto err_locked;
	}

	WARN_ON_ONCE(ctx->parent_ctx);

	/*
	 * This is the point on no return; we cannot fail hereafter. This is
	 * where we start modifying current state.
	 */

	if (move_group) {
		/*
		 * See perf_event_ctx_lock() for comments on the details
		 * of swizzling perf_event::ctx.
		 */
		perf_remove_from_context(group_leader, 0);

		list_for_each_entry(sibling, &group_leader->sibling_list,
				    group_entry) {
			perf_remove_from_context(sibling, 0);
			put_ctx(gctx);
		}

		/*
		 * Wait for everybody to stop referencing the events through
		 * the old lists, before installing it on new lists.
		 */
		synchronize_rcu();

		/*
		 * Install the group siblings before the group leader.
		 *
		 * Because a group leader will try and install the entire group
		 * (through the sibling list, which is still in-tact), we can
		 * end up with siblings installed in the wrong context.
		 *
		 * By installing siblings first we NO-OP because they're not
		 * reachable through the group lists.
		 */
		list_for_each_entry(sibling, &group_leader->sibling_list,
				    group_entry) {
			perf_event__state_init(sibling);
			perf_install_in_context(ctx, sibling, sibling->cpu);
			get_ctx(ctx);
		}

		/*
		 * Removing from the context ends up with disabled
		 * event. What we want here is event in the initial
		 * startup state, ready to be add into new context.
		 */
		perf_event__state_init(group_leader);
		perf_install_in_context(ctx, group_leader, group_leader->cpu);
		get_ctx(ctx);

		/*
		 * Now that all events are installed in @ctx, nothing
		 * references @gctx anymore, so drop the last reference we have
		 * on it.
		 */
		put_ctx(gctx);
	}

	/*
	 * Precalculate sample_data sizes; do while holding ctx::mutex such
	 * that we're serialized against further additions and before
	 * perf_install_in_context() which is the point the event is active and
	 * can use these values.
	 */
	perf_event__header_size(event);
	perf_event__id_header_size(event);

	event->owner = current;

	perf_install_in_context(ctx, event, event->cpu);
 	perf_unpin_context(ctx);
 
 	if (move_group)
		perf_event_ctx_unlock(group_leader, gctx);
 	mutex_unlock(&ctx->mutex);
 
 	if (task) {
		mutex_unlock(&task->signal->cred_guard_mutex);
		put_task_struct(task);
	}

	put_online_cpus();

	mutex_lock(&current->perf_event_mutex);
	list_add_tail(&event->owner_entry, &current->perf_event_list);
	mutex_unlock(&current->perf_event_mutex);

	/*
	 * Drop the reference on the group_event after placing the
	 * new event on the sibling_list. This ensures destruction
	 * of the group leader will find the pointer to itself in
	 * perf_group_detach().
	 */
	fdput(group);
	fd_install(event_fd, event_file);
	return event_fd;
 
 err_locked:
 	if (move_group)
		perf_event_ctx_unlock(group_leader, gctx);
 	mutex_unlock(&ctx->mutex);
 /* err_file: */
 	fput(event_file);
err_context:
	perf_unpin_context(ctx);
	put_ctx(ctx);
err_alloc:
	/*
	 * If event_file is set, the fput() above will have called ->release()
	 * and that will take care of freeing the event.
	 */
	if (!event_file)
		free_event(event);
err_cred:
	if (task)
		mutex_unlock(&task->signal->cred_guard_mutex);
err_cpus:
	put_online_cpus();
err_task:
	if (task)
		put_task_struct(task);
err_group_fd:
	fdput(group);
err_fd:
	put_unused_fd(event_fd);
	return err;
}
