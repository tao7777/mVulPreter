static void audit_log_execve_info(struct audit_context *context,
		/* write as much as we can to the audit log */
		if (len_buf > 0) {
			/* NOTE: some magic numbers here - basically if we
			 *       can't fit a reasonable amount of data into the
			 *       existing audit buffer, flush it and start with
			 *       a new buffer */
			if ((sizeof(abuf) + 8) > len_rem) {
				len_rem = len_max;
				audit_log_end(*ab);
				*ab = audit_log_start(context,
						      GFP_KERNEL, AUDIT_EXECVE);
				if (!*ab)
					goto out;
			}
 
			/* create the non-arg portion of the arg record */
			len_tmp = 0;
			if (require_data || (iter > 0) ||
			    ((len_abuf + sizeof(abuf)) > len_rem)) {
				if (iter == 0) {
					len_tmp += snprintf(&abuf[len_tmp],
							sizeof(abuf) - len_tmp,
							" a%d_len=%lu",
							arg, len_full);
				}
				len_tmp += snprintf(&abuf[len_tmp],
						    sizeof(abuf) - len_tmp,
						    " a%d[%d]=", arg, iter++);
			} else
				len_tmp += snprintf(&abuf[len_tmp],
						    sizeof(abuf) - len_tmp,
						    " a%d=", arg);
			WARN_ON(len_tmp >= sizeof(abuf));
			abuf[sizeof(abuf) - 1] = '\0';

			/* log the arg in the audit record */
			audit_log_format(*ab, "%s", abuf);
			len_rem -= len_tmp;
			len_tmp = len_buf;
			if (encode) {
				if (len_abuf > len_rem)
					len_tmp = len_rem / 2; /* encoding */
				audit_log_n_hex(*ab, buf, len_tmp);
				len_rem -= len_tmp * 2;
				len_abuf -= len_tmp * 2;
			} else {
				if (len_abuf > len_rem)
					len_tmp = len_rem - 2; /* quotes */
				audit_log_n_string(*ab, buf, len_tmp);
				len_rem -= len_tmp + 2;
				/* don't subtract the "2" because we still need
				 * to add quotes to the remaining string */
				len_abuf -= len_tmp;
			}
			len_buf -= len_tmp;
			buf += len_tmp;
		}
 
		/* ready to move to the next argument? */
		if ((len_buf == 0) && !require_data) {
			arg++;
			iter = 0;
			len_full = 0;
			require_data = true;
			encode = false;
		}
	} while (arg < context->execve.argc);
 
	/* NOTE: the caller handles the final audit_log_end() call */
 
out:
	kfree(buf_head);
 }
