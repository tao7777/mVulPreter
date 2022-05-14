sctp_chunk_length_valid(struct sctp_chunk *chunk,
			   __u16 required_length)
 {
 	__u16 chunk_length = ntohs(chunk->chunk_hdr->length);
 
	/* Previously already marked? */
	if (unlikely(chunk->pdiscard))
		return 0;
 	if (unlikely(chunk_length < required_length))
 		return 0;
 
	return 1;
}
