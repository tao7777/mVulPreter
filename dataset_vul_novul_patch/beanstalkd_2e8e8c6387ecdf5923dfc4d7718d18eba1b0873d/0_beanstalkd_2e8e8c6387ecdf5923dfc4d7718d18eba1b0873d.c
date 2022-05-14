dispatch_cmd(conn c)
{
    int r, i, timeout = -1;
    size_t z;
    unsigned int count;
    job j;
    unsigned char type;
    char *size_buf, *delay_buf, *ttr_buf, *pri_buf, *end_buf, *name;
    unsigned int pri, body_size;
    usec delay, ttr;
    uint64_t id;
    tube t = NULL;

    /* NUL-terminate this string so we can use strtol and friends */
    c->cmd[c->cmd_len - 2] = '\0';

    /* check for possible maliciousness */
    if (strlen(c->cmd) != c->cmd_len - 2) {
        return reply_msg(c, MSG_BAD_FORMAT);
    }

    type = which_cmd(c);
    dprintf("got %s command: \"%s\"\n", op_names[(int) type], c->cmd);

    switch (type) {
    case OP_PUT:
        r = read_pri(&pri, c->cmd + 4, &delay_buf);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);

        r = read_delay(&delay, delay_buf, &ttr_buf);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);

        r = read_ttr(&ttr, ttr_buf, &size_buf);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);

        errno = 0;
        body_size = strtoul(size_buf, &end_buf, 10);
         if (errno) return reply_msg(c, MSG_BAD_FORMAT);
 
         if (body_size > job_data_size_limit) {
            /* throw away the job body and respond with JOB_TOO_BIG */
            return skip(c, body_size + 2, MSG_JOB_TOO_BIG);
         }
 
         /* don't allow trailing garbage */
        if (end_buf[0] != '\0') return reply_msg(c, MSG_BAD_FORMAT);

        conn_set_producer(c);

        c->in_job = make_job(pri, delay, ttr ? : 1, body_size + 2, c->use);

        /* OOM? */
        if (!c->in_job) {
            /* throw away the job body and respond with OUT_OF_MEMORY */
            twarnx("server error: " MSG_OUT_OF_MEMORY);
            return skip(c, body_size + 2, MSG_OUT_OF_MEMORY);
        }

        fill_extra_data(c);

        /* it's possible we already have a complete job */
        maybe_enqueue_incoming_job(c);

        break;
    case OP_PEEK_READY:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_PEEK_READY_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }
        op_ct[type]++;

        j = job_copy(pq_peek(&c->use->ready));

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        reply_job(c, j, MSG_FOUND);
        break;
    case OP_PEEK_DELAYED:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_PEEK_DELAYED_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }
        op_ct[type]++;

        j = job_copy(pq_peek(&c->use->delay));

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        reply_job(c, j, MSG_FOUND);
        break;
    case OP_PEEK_BURIED:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_PEEK_BURIED_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }
        op_ct[type]++;

        j = job_copy(buried_job_p(c->use)? j = c->use->buried.next : NULL);

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        reply_job(c, j, MSG_FOUND);
        break;
    case OP_PEEKJOB:
        errno = 0;
        id = strtoull(c->cmd + CMD_PEEKJOB_LEN, &end_buf, 10);
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        /* So, peek is annoying, because some other connection might free the
         * job while we are still trying to write it out. So we copy it and
         * then free the copy when it's done sending. */
        j = job_copy(peek_job(id));

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        reply_job(c, j, MSG_FOUND);
        break;
    case OP_RESERVE_TIMEOUT:
        errno = 0;
        timeout = strtol(c->cmd + CMD_RESERVE_TIMEOUT_LEN, &end_buf, 10);
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);
    case OP_RESERVE: /* FALLTHROUGH */
        /* don't allow trailing garbage */
        if (type == OP_RESERVE && c->cmd_len != CMD_RESERVE_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }

        op_ct[type]++;
        conn_set_worker(c);

        if (conn_has_close_deadline(c) && !conn_ready(c)) {
            return reply_msg(c, MSG_DEADLINE_SOON);
        }

        /* try to get a new job for this guy */
        wait_for_job(c, timeout);
        process_queue();
        break;
    case OP_DELETE:
        errno = 0;
        id = strtoull(c->cmd + CMD_DELETE_LEN, &end_buf, 10);
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        j = job_find(id);
        j = remove_reserved_job(c, j) ? :
            remove_ready_job(j) ? :
            remove_buried_job(j);

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        j->state = JOB_STATE_INVALID;
        r = binlog_write_job(j);
        job_free(j);

        if (!r) return reply_serr(c, MSG_INTERNAL_ERROR);

        reply(c, MSG_DELETED, MSG_DELETED_LEN, STATE_SENDWORD);
        break;
    case OP_RELEASE:
        errno = 0;
        id = strtoull(c->cmd + CMD_RELEASE_LEN, &pri_buf, 10);
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);

        r = read_pri(&pri, pri_buf, &delay_buf);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);

        r = read_delay(&delay, delay_buf, NULL);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        j = remove_reserved_job(c, job_find(id));

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        /* We want to update the delay deadline on disk, so reserve space for
         * that. */
        if (delay) {
            z = binlog_reserve_space_update(j);
            if (!z) return reply_serr(c, MSG_OUT_OF_MEMORY);
            j->reserved_binlog_space += z;
        }

        j->pri = pri;
        j->delay = delay;
        j->release_ct++;

        r = enqueue_job(j, delay, !!delay);
        if (r < 0) return reply_serr(c, MSG_INTERNAL_ERROR);
        if (r == 1) {
            return reply(c, MSG_RELEASED, MSG_RELEASED_LEN, STATE_SENDWORD);
        }

        /* out of memory trying to grow the queue, so it gets buried */
        bury_job(j, 0);
        reply(c, MSG_BURIED, MSG_BURIED_LEN, STATE_SENDWORD);
        break;
    case OP_BURY:
        errno = 0;
        id = strtoull(c->cmd + CMD_BURY_LEN, &pri_buf, 10);
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);

        r = read_pri(&pri, pri_buf, NULL);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        j = remove_reserved_job(c, job_find(id));

        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        j->pri = pri;
        r = bury_job(j, 1);
        if (!r) return reply_serr(c, MSG_INTERNAL_ERROR);
        reply(c, MSG_BURIED, MSG_BURIED_LEN, STATE_SENDWORD);
        break;
    case OP_KICK:
        errno = 0;
        count = strtoul(c->cmd + CMD_KICK_LEN, &end_buf, 10);
        if (end_buf == c->cmd + CMD_KICK_LEN) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);

        op_ct[type]++;

        i = kick_jobs(c->use, count);

        return reply_line(c, STATE_SENDWORD, "KICKED %u\r\n", i);
    case OP_TOUCH:
        errno = 0;
        id = strtoull(c->cmd + CMD_TOUCH_LEN, &end_buf, 10);
        if (errno) return twarn("strtoull"), reply_msg(c, MSG_BAD_FORMAT);

        op_ct[type]++;

        j = touch_job(c, job_find(id));

        if (j) {
            reply(c, MSG_TOUCHED, MSG_TOUCHED_LEN, STATE_SENDWORD);
        } else {
            return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);
        }
        break;
    case OP_STATS:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_STATS_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }

        op_ct[type]++;

        do_stats(c, fmt_stats, NULL);
        break;
    case OP_JOBSTATS:
        errno = 0;
        id = strtoull(c->cmd + CMD_JOBSTATS_LEN, &end_buf, 10);
        if (errno) return reply_msg(c, MSG_BAD_FORMAT);

        op_ct[type]++;

        j = peek_job(id);
        if (!j) return reply(c, MSG_NOTFOUND, MSG_NOTFOUND_LEN, STATE_SENDWORD);

        if (!j->tube) return reply_serr(c, MSG_INTERNAL_ERROR);
        do_stats(c, (fmt_fn) fmt_job_stats, j);
        break;
    case OP_STATS_TUBE:
        name = c->cmd + CMD_STATS_TUBE_LEN;
        if (!name_is_ok(name, 200)) return reply_msg(c, MSG_BAD_FORMAT);

        op_ct[type]++;

        t = tube_find(name);
        if (!t) return reply_msg(c, MSG_NOTFOUND);

        do_stats(c, (fmt_fn) fmt_stats_tube, t);
        t = NULL;
        break;
    case OP_LIST_TUBES:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_LIST_TUBES_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }

        op_ct[type]++;
        do_list_tubes(c, &tubes);
        break;
    case OP_LIST_TUBE_USED:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_LIST_TUBE_USED_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }

        op_ct[type]++;
        reply_line(c, STATE_SENDWORD, "USING %s\r\n", c->use->name);
        break;
    case OP_LIST_TUBES_WATCHED:
        /* don't allow trailing garbage */
        if (c->cmd_len != CMD_LIST_TUBES_WATCHED_LEN + 2) {
            return reply_msg(c, MSG_BAD_FORMAT);
        }

        op_ct[type]++;
        do_list_tubes(c, &c->watch);
        break;
    case OP_USE:
        name = c->cmd + CMD_USE_LEN;
        if (!name_is_ok(name, 200)) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        TUBE_ASSIGN(t, tube_find_or_make(name));
        if (!t) return reply_serr(c, MSG_OUT_OF_MEMORY);

        c->use->using_ct--;
        TUBE_ASSIGN(c->use, t);
        TUBE_ASSIGN(t, NULL);
        c->use->using_ct++;

        reply_line(c, STATE_SENDWORD, "USING %s\r\n", c->use->name);
        break;
    case OP_WATCH:
        name = c->cmd + CMD_WATCH_LEN;
        if (!name_is_ok(name, 200)) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        TUBE_ASSIGN(t, tube_find_or_make(name));
        if (!t) return reply_serr(c, MSG_OUT_OF_MEMORY);

        r = 1;
        if (!ms_contains(&c->watch, t)) r = ms_append(&c->watch, t);
        TUBE_ASSIGN(t, NULL);
        if (!r) return reply_serr(c, MSG_OUT_OF_MEMORY);

        reply_line(c, STATE_SENDWORD, "WATCHING %d\r\n", c->watch.used);
        break;
    case OP_IGNORE:
        name = c->cmd + CMD_IGNORE_LEN;
        if (!name_is_ok(name, 200)) return reply_msg(c, MSG_BAD_FORMAT);
        op_ct[type]++;

        t = NULL;
        for (i = 0; i < c->watch.used; i++) {
            t = c->watch.items[i];
            if (strncmp(t->name, name, MAX_TUBE_NAME_LEN) == 0) break;
            t = NULL;
        }

        if (t && c->watch.used < 2) return reply_msg(c, MSG_NOT_IGNORED);

        if (t) ms_remove(&c->watch, t); /* may free t if refcount => 0 */
        t = NULL;

        reply_line(c, STATE_SENDWORD, "WATCHING %d\r\n", c->watch.used);
        break;
    case OP_QUIT:
        conn_close(c);
        break;
    case OP_PAUSE_TUBE:
        op_ct[type]++;

        r = read_tube_name(&name, c->cmd + CMD_PAUSE_TUBE_LEN, &delay_buf);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);

        r = read_delay(&delay, delay_buf, NULL);
        if (r) return reply_msg(c, MSG_BAD_FORMAT);

        *delay_buf = '\0';
        t = tube_find(name);
        if (!t) return reply_msg(c, MSG_NOTFOUND);

        t->deadline_at = now_usec() + delay;
        t->pause = delay;
        t->stat.pause_ct++;
        set_main_delay_timeout();

        reply_line(c, STATE_SENDWORD, "PAUSED\r\n");
        break;
    default:
        return reply_msg(c, MSG_UNKNOWN_COMMAND);
    }
}
