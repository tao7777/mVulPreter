sctp_chunk_length_valid(struct sctp_chunk *chunk,
			   __u16 required_length)
 {
 	__u16 chunk_length = ntohs(chunk->chunk_hdr->length);
 
 	if (unlikely(chunk_length < required_length))
 		return 0;
 
	return 1;
}
