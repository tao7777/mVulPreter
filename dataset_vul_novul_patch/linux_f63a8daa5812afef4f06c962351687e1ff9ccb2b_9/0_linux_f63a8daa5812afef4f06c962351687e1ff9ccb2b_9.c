static long perf_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
static long _perf_ioctl(struct perf_event *event, unsigned int cmd, unsigned long arg)
 {
 	void (*func)(struct perf_event *);
 	u32 flags = arg;
 
 	switch (cmd) {
 	case PERF_EVENT_IOC_ENABLE:
		func = _perf_event_enable;
 		break;
 	case PERF_EVENT_IOC_DISABLE:
		func = _perf_event_disable;
 		break;
 	case PERF_EVENT_IOC_RESET:
		func = _perf_event_reset;
 		break;
 
 	case PERF_EVENT_IOC_REFRESH:
		return _perf_event_refresh(event, arg);
 
 	case PERF_EVENT_IOC_PERIOD:
 		return perf_event_period(event, (u64 __user *)arg);

	case PERF_EVENT_IOC_ID:
	{
		u64 id = primary_event_id(event);

		if (copy_to_user((void __user *)arg, &id, sizeof(id)))
			return -EFAULT;
		return 0;
	}

	case PERF_EVENT_IOC_SET_OUTPUT:
	{
		int ret;
		if (arg != -1) {
			struct perf_event *output_event;
			struct fd output;
			ret = perf_fget_light(arg, &output);
			if (ret)
				return ret;
			output_event = output.file->private_data;
			ret = perf_event_set_output(event, output_event);
			fdput(output);
		} else {
			ret = perf_event_set_output(event, NULL);
		}
		return ret;
	}

	case PERF_EVENT_IOC_SET_FILTER:
		return perf_event_set_filter(event, (void __user *)arg);

	default:
		return -ENOTTY;
	}

	if (flags & PERF_IOC_FLAG_GROUP)
		perf_event_for_each(event, func);
	else
		perf_event_for_each_child(event, func);

 	return 0;
 }
