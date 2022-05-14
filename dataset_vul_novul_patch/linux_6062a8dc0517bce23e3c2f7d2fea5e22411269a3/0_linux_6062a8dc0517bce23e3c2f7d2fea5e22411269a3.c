long do_msgsnd(int msqid, long mtype, void __user *mtext,
		size_t msgsz, int msgflg)
{
	struct msg_queue *msq;
	struct msg_msg *msg;
	int err;
	struct ipc_namespace *ns;

	ns = current->nsproxy->ipc_ns;

	if (msgsz > ns->msg_ctlmax || (long) msgsz < 0 || msqid < 0)
		return -EINVAL;
	if (mtype < 1)
		return -EINVAL;

	msg = load_msg(mtext, msgsz);
	if (IS_ERR(msg))
		return PTR_ERR(msg);

	msg->m_type = mtype;
	msg->m_ts = msgsz;

	msq = msg_lock_check(ns, msqid);
	if (IS_ERR(msq)) {
		err = PTR_ERR(msq);
		goto out_free;
	}

	for (;;) {
		struct msg_sender s;

		err = -EACCES;
		if (ipcperms(ns, &msq->q_perm, S_IWUGO))
			goto out_unlock_free;

		err = security_msg_queue_msgsnd(msq, msg, msgflg);
		if (err)
			goto out_unlock_free;

		if (msgsz + msq->q_cbytes <= msq->q_qbytes &&
				1 + msq->q_qnum <= msq->q_qbytes) {
			break;
		}

		/* queue full, wait: */
		if (msgflg & IPC_NOWAIT) {
			err = -EAGAIN;
 			goto out_unlock_free;
 		}
 		ss_add(msq, &s);

		if (!ipc_rcu_getref(msq)) {
			err = -EIDRM;
			goto out_unlock_free;
		}

 		msg_unlock(msq);
 		schedule();
 
		ipc_lock_by_ptr(&msq->q_perm);
		ipc_rcu_putref(msq);
		if (msq->q_perm.deleted) {
			err = -EIDRM;
			goto out_unlock_free;
		}
		ss_del(&s);

		if (signal_pending(current)) {
			err = -ERESTARTNOHAND;
			goto out_unlock_free;
		}
	}

	msq->q_lspid = task_tgid_vnr(current);
	msq->q_stime = get_seconds();

	if (!pipelined_send(msq, msg)) {
		/* no one is waiting for this message, enqueue it */
		list_add_tail(&msg->m_list, &msq->q_messages);
		msq->q_cbytes += msgsz;
		msq->q_qnum++;
		atomic_add(msgsz, &ns->msg_bytes);
		atomic_inc(&ns->msg_hdrs);
	}

	err = 0;
	msg = NULL;

out_unlock_free:
	msg_unlock(msq);
out_free:
	if (msg != NULL)
		free_msg(msg);
	return err;
}
