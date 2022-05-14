static int srpt_rx_mgmt_fn_tag(struct srpt_send_ioctx *ioctx, u64 tag)
{
	struct srpt_device *sdev;
	struct srpt_rdma_ch *ch;
	struct srpt_send_ioctx *target;
	int ret, i;
	ret = -EINVAL;
	ch = ioctx->ch;
	BUG_ON(!ch);
	BUG_ON(!ch->sport);
	sdev = ch->sport->sdev;
	BUG_ON(!sdev);
	spin_lock_irq(&sdev->spinlock);
	for (i = 0; i < ch->rq_size; ++i) {
		target = ch->ioctx_ring[i];
		if (target->cmd.se_lun == ioctx->cmd.se_lun &&
		    target->cmd.tag == tag &&
		    srpt_get_cmd_state(target) != SRPT_STATE_DONE) {
			ret = 0;
			/* now let the target core abort &target->cmd; */
			break;
		}
	}
	spin_unlock_irq(&sdev->spinlock);
	return ret;
}
