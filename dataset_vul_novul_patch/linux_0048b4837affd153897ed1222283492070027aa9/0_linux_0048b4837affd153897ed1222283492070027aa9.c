static bool blk_kick_flush(struct request_queue *q, struct blk_flush_queue *fq)
{
	struct list_head *pending = &fq->flush_queue[fq->flush_pending_idx];
	struct request *first_rq =
		list_first_entry(pending, struct request, flush.list);
	struct request *flush_rq = fq->flush_rq;

	/* C1 described at the top of this file */
	if (fq->flush_pending_idx != fq->flush_running_idx || list_empty(pending))
		return false;

	/* C2 and C3 */
	if (!list_empty(&fq->flush_data_in_flight) &&
	    time_before(jiffies,
			fq->flush_pending_since + FLUSH_PENDING_TIMEOUT))
		return false;

	/*
	 * Issue flush and toggle pending_idx.  This makes pending_idx
	 * different from running_idx, which means flush is in flight.
	 */
	fq->flush_pending_idx ^= 1;

	blk_rq_init(q, flush_rq);
 
 	/*
 	 * Borrow tag from the first request since they can't
	 * be in flight at the same time. And acquire the tag's
	 * ownership for flush req.
 	 */
 	if (q->mq_ops) {
		struct blk_mq_hw_ctx *hctx;

 		flush_rq->mq_ctx = first_rq->mq_ctx;
 		flush_rq->tag = first_rq->tag;
		fq->orig_rq = first_rq;

		hctx = q->mq_ops->map_queue(q, first_rq->mq_ctx->cpu);
		blk_mq_tag_set_rq(hctx, first_rq->tag, flush_rq);
 	}
 
 	flush_rq->cmd_type = REQ_TYPE_FS;
	flush_rq->cmd_flags = WRITE_FLUSH | REQ_FLUSH_SEQ;
	flush_rq->rq_disk = first_rq->rq_disk;
	flush_rq->end_io = flush_end_io;

	return blk_flush_queue_rq(flush_rq, false);
}
