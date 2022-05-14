call_bind_status(struct rpc_task *task)
{
	int status = -EIO;

	if (task->tk_status >= 0) {
		dprint_status(task);
		task->tk_status = 0;
		task->tk_action = call_connect;
		return;
	}

	switch (task->tk_status) {
	case -ENOMEM:
		dprintk("RPC: %5u rpcbind out of memory\n", task->tk_pid);
		rpc_delay(task, HZ >> 2);
		goto retry_timeout;
	case -EACCES:
		dprintk("RPC: %5u remote rpcbind: RPC program/version "
				"unavailable\n", task->tk_pid);
		/* fail immediately if this is an RPC ping */
		if (task->tk_msg.rpc_proc->p_proc == 0) {
 			status = -EOPNOTSUPP;
 			break;
 		}
 		rpc_delay(task, 3*HZ);
 		goto retry_timeout;
 	case -ETIMEDOUT:
		dprintk("RPC: %5u rpcbind request timed out\n",
				task->tk_pid);
		goto retry_timeout;
	case -EPFNOSUPPORT:
		/* server doesn't support any rpcbind version we know of */
		dprintk("RPC: %5u unrecognized remote rpcbind service\n",
				task->tk_pid);
		break;
	case -EPROTONOSUPPORT:
		dprintk("RPC: %5u remote rpcbind version unavailable, retrying\n",
				task->tk_pid);
		task->tk_status = 0;
		task->tk_action = call_bind;
		return;
	case -ECONNREFUSED:		/* connection problems */
	case -ECONNRESET:
	case -ENOTCONN:
	case -EHOSTDOWN:
	case -EHOSTUNREACH:
	case -ENETUNREACH:
	case -EPIPE:
		dprintk("RPC: %5u remote rpcbind unreachable: %d\n",
				task->tk_pid, task->tk_status);
		if (!RPC_IS_SOFTCONN(task)) {
			rpc_delay(task, 5*HZ);
			goto retry_timeout;
		}
		status = task->tk_status;
		break;
	default:
		dprintk("RPC: %5u unrecognized rpcbind error (%d)\n",
				task->tk_pid, -task->tk_status);
	}

	rpc_exit(task, status);
	return;

retry_timeout:
	task->tk_action = call_timeout;
}
