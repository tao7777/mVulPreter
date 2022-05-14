static ssize_t hfi1_file_write(struct file *fp, const char __user *data,
			       size_t count, loff_t *offset)
{
	const struct hfi1_cmd __user *ucmd;
	struct hfi1_filedata *fd = fp->private_data;
	struct hfi1_ctxtdata *uctxt = fd->uctxt;
	struct hfi1_cmd cmd;
	struct hfi1_user_info uinfo;
	struct hfi1_tid_info tinfo;
	unsigned long addr;
	ssize_t consumed = 0, copy = 0, ret = 0;
	void *dest = NULL;
	__u64 user_val = 0;
 	int uctxt_required = 1;
 	int must_be_root = 0;
 
	/* FIXME: This interface cannot continue out of staging */
	if (WARN_ON_ONCE(!ib_safe_file_access(fp)))
		return -EACCES;

 	if (count < sizeof(cmd)) {
 		ret = -EINVAL;
 		goto bail;
	}

	ucmd = (const struct hfi1_cmd __user *)data;
	if (copy_from_user(&cmd, ucmd, sizeof(cmd))) {
		ret = -EFAULT;
		goto bail;
	}

	consumed = sizeof(cmd);

	switch (cmd.type) {
	case HFI1_CMD_ASSIGN_CTXT:
		uctxt_required = 0;	/* assigned user context not required */
		copy = sizeof(uinfo);
		dest = &uinfo;
		break;
	case HFI1_CMD_SDMA_STATUS_UPD:
	case HFI1_CMD_CREDIT_UPD:
		copy = 0;
		break;
	case HFI1_CMD_TID_UPDATE:
	case HFI1_CMD_TID_FREE:
	case HFI1_CMD_TID_INVAL_READ:
		copy = sizeof(tinfo);
		dest = &tinfo;
		break;
	case HFI1_CMD_USER_INFO:
	case HFI1_CMD_RECV_CTRL:
	case HFI1_CMD_POLL_TYPE:
	case HFI1_CMD_ACK_EVENT:
	case HFI1_CMD_CTXT_INFO:
	case HFI1_CMD_SET_PKEY:
	case HFI1_CMD_CTXT_RESET:
		copy = 0;
		user_val = cmd.addr;
		break;
	case HFI1_CMD_EP_INFO:
	case HFI1_CMD_EP_ERASE_CHIP:
	case HFI1_CMD_EP_ERASE_RANGE:
	case HFI1_CMD_EP_READ_RANGE:
	case HFI1_CMD_EP_WRITE_RANGE:
		uctxt_required = 0;	/* assigned user context not required */
		must_be_root = 1;	/* validate user */
		copy = 0;
		break;
	default:
		ret = -EINVAL;
		goto bail;
	}

	/* If the command comes with user data, copy it. */
	if (copy) {
		if (copy_from_user(dest, (void __user *)cmd.addr, copy)) {
			ret = -EFAULT;
			goto bail;
		}
		consumed += copy;
	}

	/*
	 * Make sure there is a uctxt when needed.
	 */
	if (uctxt_required && !uctxt) {
		ret = -EINVAL;
		goto bail;
	}

	/* only root can do these operations */
	if (must_be_root && !capable(CAP_SYS_ADMIN)) {
		ret = -EPERM;
		goto bail;
	}

	switch (cmd.type) {
	case HFI1_CMD_ASSIGN_CTXT:
		ret = assign_ctxt(fp, &uinfo);
		if (ret < 0)
			goto bail;
		ret = setup_ctxt(fp);
		if (ret)
			goto bail;
		ret = user_init(fp);
		break;
	case HFI1_CMD_CTXT_INFO:
		ret = get_ctxt_info(fp, (void __user *)(unsigned long)
				    user_val, cmd.len);
		break;
	case HFI1_CMD_USER_INFO:
		ret = get_base_info(fp, (void __user *)(unsigned long)
				    user_val, cmd.len);
		break;
	case HFI1_CMD_SDMA_STATUS_UPD:
		break;
	case HFI1_CMD_CREDIT_UPD:
		if (uctxt && uctxt->sc)
			sc_return_credits(uctxt->sc);
		break;
	case HFI1_CMD_TID_UPDATE:
		ret = hfi1_user_exp_rcv_setup(fp, &tinfo);
		if (!ret) {
			/*
			 * Copy the number of tidlist entries we used
			 * and the length of the buffer we registered.
			 * These fields are adjacent in the structure so
			 * we can copy them at the same time.
			 */
			addr = (unsigned long)cmd.addr +
				offsetof(struct hfi1_tid_info, tidcnt);
			if (copy_to_user((void __user *)addr, &tinfo.tidcnt,
					 sizeof(tinfo.tidcnt) +
					 sizeof(tinfo.length)))
				ret = -EFAULT;
		}
		break;
	case HFI1_CMD_TID_INVAL_READ:
		ret = hfi1_user_exp_rcv_invalid(fp, &tinfo);
		if (ret)
			break;
		addr = (unsigned long)cmd.addr +
			offsetof(struct hfi1_tid_info, tidcnt);
		if (copy_to_user((void __user *)addr, &tinfo.tidcnt,
				 sizeof(tinfo.tidcnt)))
			ret = -EFAULT;
		break;
	case HFI1_CMD_TID_FREE:
		ret = hfi1_user_exp_rcv_clear(fp, &tinfo);
		if (ret)
			break;
		addr = (unsigned long)cmd.addr +
			offsetof(struct hfi1_tid_info, tidcnt);
		if (copy_to_user((void __user *)addr, &tinfo.tidcnt,
				 sizeof(tinfo.tidcnt)))
			ret = -EFAULT;
		break;
	case HFI1_CMD_RECV_CTRL:
		ret = manage_rcvq(uctxt, fd->subctxt, (int)user_val);
		break;
	case HFI1_CMD_POLL_TYPE:
		uctxt->poll_type = (typeof(uctxt->poll_type))user_val;
		break;
	case HFI1_CMD_ACK_EVENT:
		ret = user_event_ack(uctxt, fd->subctxt, user_val);
		break;
	case HFI1_CMD_SET_PKEY:
		if (HFI1_CAP_IS_USET(PKEY_CHECK))
			ret = set_ctxt_pkey(uctxt, fd->subctxt, user_val);
		else
			ret = -EPERM;
		break;
	case HFI1_CMD_CTXT_RESET: {
		struct send_context *sc;
		struct hfi1_devdata *dd;

		if (!uctxt || !uctxt->dd || !uctxt->sc) {
			ret = -EINVAL;
			break;
		}
		/*
		 * There is no protection here. User level has to
		 * guarantee that no one will be writing to the send
		 * context while it is being re-initialized.
		 * If user level breaks that guarantee, it will break
		 * it's own context and no one else's.
		 */
		dd = uctxt->dd;
		sc = uctxt->sc;
		/*
		 * Wait until the interrupt handler has marked the
		 * context as halted or frozen. Report error if we time
		 * out.
		 */
		wait_event_interruptible_timeout(
			sc->halt_wait, (sc->flags & SCF_HALTED),
			msecs_to_jiffies(SEND_CTXT_HALT_TIMEOUT));
		if (!(sc->flags & SCF_HALTED)) {
			ret = -ENOLCK;
			break;
		}
		/*
		 * If the send context was halted due to a Freeze,
		 * wait until the device has been "unfrozen" before
		 * resetting the context.
		 */
		if (sc->flags & SCF_FROZEN) {
			wait_event_interruptible_timeout(
				dd->event_queue,
				!(ACCESS_ONCE(dd->flags) & HFI1_FROZEN),
				msecs_to_jiffies(SEND_CTXT_HALT_TIMEOUT));
			if (dd->flags & HFI1_FROZEN) {
				ret = -ENOLCK;
				break;
			}
			if (dd->flags & HFI1_FORCED_FREEZE) {
				/*
				 * Don't allow context reset if we are into
				 * forced freeze
				 */
				ret = -ENODEV;
				break;
			}
			sc_disable(sc);
			ret = sc_enable(sc);
			hfi1_rcvctrl(dd, HFI1_RCVCTRL_CTXT_ENB,
				     uctxt->ctxt);
		} else {
			ret = sc_restart(sc);
		}
		if (!ret)
			sc_return_credits(sc);
		break;
	}
	case HFI1_CMD_EP_INFO:
	case HFI1_CMD_EP_ERASE_CHIP:
	case HFI1_CMD_EP_ERASE_RANGE:
	case HFI1_CMD_EP_READ_RANGE:
	case HFI1_CMD_EP_WRITE_RANGE:
		ret = handle_eprom_command(fp, &cmd);
		break;
	}

	if (ret >= 0)
		ret = consumed;
bail:
	return ret;
}
