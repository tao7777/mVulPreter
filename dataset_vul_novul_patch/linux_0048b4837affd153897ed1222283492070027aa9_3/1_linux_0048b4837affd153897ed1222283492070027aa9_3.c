 struct request *blk_mq_tag_to_rq(struct blk_mq_tags *tags, unsigned int tag)
 {
	struct request *rq = tags->rqs[tag];
	/* mq_ctx of flush rq is always cloned from the corresponding req */
	struct blk_flush_queue *fq = blk_get_flush_queue(rq->q, rq->mq_ctx);
	if (!is_flush_request(rq, fq, tag))
		return rq;
	return fq->flush_rq;
 }
