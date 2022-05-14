static void __iov_iter_advance_iov(struct iov_iter *i, size_t bytes)
{
	if (likely(i->nr_segs == 1)) {
		i->iov_offset += bytes;
	} else {
 		const struct iovec *iov = i->iov;
 		size_t base = i->iov_offset;
 
		/*
		 * The !iov->iov_len check ensures we skip over unlikely
		 * zero-length segments.
		 */
		while (bytes || !iov->iov_len) {
 			int copy = min(bytes, iov->iov_len - base);
 
 			bytes -= copy;
			base += copy;
			if (iov->iov_len == base) {
				iov++;
				base = 0;
			}
		}
		i->iov = iov;
		i->iov_offset = base;
	}
}
