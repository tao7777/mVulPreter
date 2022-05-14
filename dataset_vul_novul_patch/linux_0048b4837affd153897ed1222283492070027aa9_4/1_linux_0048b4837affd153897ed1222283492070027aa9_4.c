static inline bool is_flush_request(struct request *rq,
		struct blk_flush_queue *fq, unsigned int tag)
{
	return ((rq->cmd_flags & REQ_FLUSH_SEQ) &&
			fq->flush_rq->tag == tag);
}
