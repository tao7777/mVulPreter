static void unix_copy_addr(struct msghdr *msg, struct sock *sk)
 {
 	struct unix_sock *u = unix_sk(sk);
 
 	if (u->addr) {
 		msg->msg_namelen = u->addr->len;
 		memcpy(msg->msg_name, u->addr->name, u->addr->len);
	}
}
