 choose_filters(struct archive_read *a)
 {
	int number_bidders, i, bid, best_bid, n;
 	struct archive_read_filter_bidder *bidder, *best_bidder;
 	struct archive_read_filter *filter;
 	ssize_t avail;
 	int r;
 
	for (n = 0; n < 25; ++n) {
 		number_bidders = sizeof(a->bidders) / sizeof(a->bidders[0]);
 
 		best_bid = 0;
		best_bidder = NULL;

		bidder = a->bidders;
		for (i = 0; i < number_bidders; i++, bidder++) {
			if (bidder->bid != NULL) {
				bid = (bidder->bid)(bidder, a->filter);
				if (bid > best_bid) {
					best_bid = bid;
					best_bidder = bidder;
				}
			}
		}

		/* If no bidder, we're done. */
		if (best_bidder == NULL) {
			/* Verify the filter by asking it for some data. */
			__archive_read_filter_ahead(a->filter, 1, &avail);
			if (avail < 0) {
				__archive_read_close_filters(a);
				__archive_read_free_filters(a);
				return (ARCHIVE_FATAL);
			}
			a->archive.compression_name = a->filter->name;
			a->archive.compression_code = a->filter->code;
			return (ARCHIVE_OK);
		}

		filter
		    = (struct archive_read_filter *)calloc(1, sizeof(*filter));
		if (filter == NULL)
			return (ARCHIVE_FATAL);
		filter->bidder = best_bidder;
		filter->archive = a;
		filter->upstream = a->filter;
		a->filter = filter;
		r = (best_bidder->init)(a->filter);
		if (r != ARCHIVE_OK) {
			__archive_read_close_filters(a);
			__archive_read_free_filters(a);
 			return (ARCHIVE_FATAL);
 		}
 	}
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Input requires too many filters for decoding");
	return (ARCHIVE_FATAL);
 }
