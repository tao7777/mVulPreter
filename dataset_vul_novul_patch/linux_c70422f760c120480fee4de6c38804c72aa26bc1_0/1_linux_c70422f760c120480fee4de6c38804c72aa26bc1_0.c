static void lockd_down_net(struct svc_serv *serv, struct net *net)
{
	struct lockd_net *ln = net_generic(net, lockd_net_id);

 	if (ln->nlmsvc_users) {
 		if (--ln->nlmsvc_users == 0) {
 			nlm_shutdown_hosts_net(net);
			cancel_delayed_work_sync(&ln->grace_period_end);
			locks_end_grace(&ln->lockd_manager);
 			svc_shutdown_net(serv, net);
 			dprintk("lockd_down_net: per-net data destroyed; net=%p\n", net);
 		}
	} else {
		printk(KERN_ERR "lockd_down_net: no users! task=%p, net=%p\n",
				nlmsvc_task, net);
		BUG();
	}
}
