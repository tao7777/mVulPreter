static void frag_kfree_skb(struct netns_frags *nf, struct sk_buff *skb)
{
	atomic_sub(skb->truesize, &nf->mem);
	kfree_skb(skb);
}
