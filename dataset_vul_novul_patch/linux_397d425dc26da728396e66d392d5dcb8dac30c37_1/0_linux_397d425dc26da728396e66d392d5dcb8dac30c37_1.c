static inline int handle_dots(struct nameidata *nd, int type)
{
	if (type == LAST_DOTDOT) {
 		if (nd->flags & LOOKUP_RCU) {
 			return follow_dotdot_rcu(nd);
 		} else
			return follow_dotdot(nd);
 	}
 	return 0;
 }
