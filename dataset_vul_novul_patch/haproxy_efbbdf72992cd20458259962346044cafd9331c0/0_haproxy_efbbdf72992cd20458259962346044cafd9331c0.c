static int dns_validate_dns_response(unsigned char *resp, unsigned char *bufend,
				     struct dns_resolution *resolution, int max_answer_records)
{
	unsigned char *reader;
	char *previous_dname, tmpname[DNS_MAX_NAME_SIZE];
	int len, flags, offset;
	int dns_query_record_id;
	int nb_saved_records;
	struct dns_query_item *dns_query;
	struct dns_answer_item *dns_answer_record, *tmp_record;
	struct dns_response_packet *dns_p;
	int i, found = 0;

	reader         = resp;
	len            = 0;
	previous_dname = NULL;
	dns_query      = NULL;

	/* Initialization of response buffer and structure */
	dns_p = &resolution->response;

	/* query id */
	if (reader + 2 >= bufend)
		return DNS_RESP_INVALID;
	dns_p->header.id = reader[0] * 256 + reader[1];
	reader += 2;

	/* Flags and rcode are stored over 2 bytes
	 * First byte contains:
	 *  - response flag (1 bit)
	 *  - opcode (4 bits)
	 *  - authoritative (1 bit)
	 *  - truncated (1 bit)
	 *  - recursion desired (1 bit)
	 */
	if (reader + 2 >= bufend)
		return DNS_RESP_INVALID;

	flags = reader[0] * 256 + reader[1];

	if ((flags & DNS_FLAG_REPLYCODE) != DNS_RCODE_NO_ERROR) {
		if ((flags & DNS_FLAG_REPLYCODE) == DNS_RCODE_NX_DOMAIN)
			return DNS_RESP_NX_DOMAIN;
		else if ((flags & DNS_FLAG_REPLYCODE) == DNS_RCODE_REFUSED)
			return DNS_RESP_REFUSED;
		return DNS_RESP_ERROR;
	}

	/* Move forward 2 bytes for flags */
	reader += 2;

	/* 2 bytes for question count */
	if (reader + 2 >= bufend)
		return DNS_RESP_INVALID;
	dns_p->header.qdcount = reader[0] * 256 + reader[1];
	/* (for now) we send one query only, so we expect only one in the
	 * response too */
	if (dns_p->header.qdcount != 1)
		return DNS_RESP_QUERY_COUNT_ERROR;
	if (dns_p->header.qdcount > DNS_MAX_QUERY_RECORDS)
		return DNS_RESP_INVALID;
	reader += 2;

	/* 2 bytes for answer count */
	if (reader + 2 >= bufend)
		return DNS_RESP_INVALID;
	dns_p->header.ancount = reader[0] * 256 + reader[1];
	if (dns_p->header.ancount == 0)
		return DNS_RESP_ANCOUNT_ZERO;
	/* Check if too many records are announced */
	if (dns_p->header.ancount > max_answer_records)
		return DNS_RESP_INVALID;
	reader += 2;

	/* 2 bytes authority count */
	if (reader + 2 >= bufend)
		return DNS_RESP_INVALID;
	dns_p->header.nscount = reader[0] * 256 + reader[1];
	reader += 2;

	/* 2 bytes additional count */
	if (reader + 2 >= bufend)
		return DNS_RESP_INVALID;
	dns_p->header.arcount = reader[0] * 256 + reader[1];
	reader += 2;

	/* Parsing dns queries */
	LIST_INIT(&dns_p->query_list);
	for (dns_query_record_id = 0; dns_query_record_id < dns_p->header.qdcount; dns_query_record_id++) {
		/* Use next pre-allocated dns_query_item after ensuring there is
		 * still one available.
		 * It's then added to our packet query list. */
		if (dns_query_record_id > DNS_MAX_QUERY_RECORDS)
			return DNS_RESP_INVALID;
		dns_query = &resolution->response_query_records[dns_query_record_id];
		LIST_ADDQ(&dns_p->query_list, &dns_query->list);

		/* Name is a NULL terminated string in our case, since we have
		 * one query per response and the first one can't be compressed
		 * (using the 0x0c format) */
		offset = 0;
		len = dns_read_name(resp, bufend, reader, dns_query->name, DNS_MAX_NAME_SIZE, &offset, 0);

		if (len == 0)
			return DNS_RESP_INVALID;

		reader += offset;
		previous_dname = dns_query->name;

		/* move forward 2 bytes for question type */
		if (reader + 2 >= bufend)
			return DNS_RESP_INVALID;
		dns_query->type = reader[0] * 256 + reader[1];
		reader += 2;

		/* move forward 2 bytes for question class */
		if (reader + 2 >= bufend)
			return DNS_RESP_INVALID;
		dns_query->class = reader[0] * 256 + reader[1];
		reader += 2;
	}

	/* TRUNCATED flag must be checked after we could read the query type
	 * because a TRUNCATED SRV query type response can still be exploited */
	if (dns_query->type != DNS_RTYPE_SRV && flags & DNS_FLAG_TRUNCATED)
		return DNS_RESP_TRUNCATED;

	/* now parsing response records */
	nb_saved_records = 0;
	for (i = 0; i < dns_p->header.ancount; i++) {
		if (reader >= bufend)
			return DNS_RESP_INVALID;

		dns_answer_record = pool_alloc(dns_answer_item_pool);
		if (dns_answer_record == NULL)
			return (DNS_RESP_INVALID);

		offset = 0;
		len = dns_read_name(resp, bufend, reader, tmpname, DNS_MAX_NAME_SIZE, &offset, 0);

		if (len == 0) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			return DNS_RESP_INVALID;
		}

		/* Check if the current record dname is valid.  previous_dname
		 * points either to queried dname or last CNAME target */
		if (dns_query->type != DNS_RTYPE_SRV && memcmp(previous_dname, tmpname, len) != 0) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			if (i == 0) {
				/* First record, means a mismatch issue between
				 * queried dname and dname found in the first
				 * record */
				return DNS_RESP_INVALID;
			}
			else {
				/* If not the first record, this means we have a
				 * CNAME resolution error */
				return DNS_RESP_CNAME_ERROR;
			}

		}

		memcpy(dns_answer_record->name, tmpname, len);
		dns_answer_record->name[len] = 0;

		reader += offset;
		if (reader >= bufend) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			return DNS_RESP_INVALID;
		}

		/* 2 bytes for record type (A, AAAA, CNAME, etc...) */
		if (reader + 2 > bufend) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			return DNS_RESP_INVALID;
		}
		dns_answer_record->type = reader[0] * 256 + reader[1];
		reader += 2;

		/* 2 bytes for class (2) */
		if (reader + 2 > bufend) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			return DNS_RESP_INVALID;
		}
		dns_answer_record->class = reader[0] * 256 + reader[1];
		reader += 2;

		/* 4 bytes for ttl (4) */
		if (reader + 4 > bufend) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			return DNS_RESP_INVALID;
		}
		dns_answer_record->ttl =   reader[0] * 16777216 + reader[1] * 65536
			                 + reader[2] * 256 + reader[3];
		reader += 4;

		/* Now reading data len */
		if (reader + 2 > bufend) {
			pool_free(dns_answer_item_pool, dns_answer_record);
			return DNS_RESP_INVALID;
		}
		dns_answer_record->data_len = reader[0] * 256 + reader[1];

                /* Move forward 2 bytes for data len */
                reader += 2;
 
               if (reader + dns_answer_record->data_len >= bufend) {
                       pool_free(dns_answer_item_pool, dns_answer_record);
                       return DNS_RESP_INVALID;
               }

                /* Analyzing record content */
                switch (dns_answer_record->type) {
                        case DNS_RTYPE_A:
				dns_answer_record->address.sa_family = AF_INET;
				memcpy(&(((struct sockaddr_in *)&dns_answer_record->address)->sin_addr),
						reader, dns_answer_record->data_len);
				break;

			case DNS_RTYPE_CNAME:
				/* Check if this is the last record and update the caller about the status:
				 * no IP could be found and last record was a CNAME. Could be triggered
				 * by a wrong query type
				 *
				 * + 1 because dns_answer_record_id starts at 0
				 * while number of answers is an integer and
				 * starts at 1.
				 */
				if (i + 1 == dns_p->header.ancount) {
					pool_free(dns_answer_item_pool, dns_answer_record);
					return DNS_RESP_CNAME_ERROR;
				}

				offset = 0;
				len = dns_read_name(resp, bufend, reader, tmpname, DNS_MAX_NAME_SIZE, &offset, 0);
				if (len == 0) {
					pool_free(dns_answer_item_pool, dns_answer_record);
					return DNS_RESP_INVALID;
				}

				memcpy(dns_answer_record->target, tmpname, len);
				dns_answer_record->target[len] = 0;
				previous_dname = dns_answer_record->target;
				break;


			case DNS_RTYPE_SRV:
				/* Answer must contain :
				 * - 2 bytes for the priority
				 * - 2 bytes for the weight
				 * - 2 bytes for the port
				 * - the target hostname
				 */
				if (dns_answer_record->data_len <= 6) {
					pool_free(dns_answer_item_pool, dns_answer_record);
					return DNS_RESP_INVALID;
				}
				dns_answer_record->priority = read_n16(reader);
				reader += sizeof(uint16_t);
				dns_answer_record->weight = read_n16(reader);
				reader += sizeof(uint16_t);
				dns_answer_record->port = read_n16(reader);
				reader += sizeof(uint16_t);
				offset = 0;
				len = dns_read_name(resp, bufend, reader, tmpname, DNS_MAX_NAME_SIZE, &offset, 0);
				if (len == 0) {
					pool_free(dns_answer_item_pool, dns_answer_record);
					return DNS_RESP_INVALID;
				}
				dns_answer_record->data_len = len;
				memcpy(dns_answer_record->target, tmpname, len);
				dns_answer_record->target[len] = 0;
				break;

			case DNS_RTYPE_AAAA:
				/* ipv6 is stored on 16 bytes */
				if (dns_answer_record->data_len != 16) {
					pool_free(dns_answer_item_pool, dns_answer_record);
					return DNS_RESP_INVALID;
				}
				dns_answer_record->address.sa_family = AF_INET6;
				memcpy(&(((struct sockaddr_in6 *)&dns_answer_record->address)->sin6_addr),
						reader, dns_answer_record->data_len);
				break;

		} /* switch (record type) */

		/* Increment the counter for number of records saved into our
		 * local response */
		nb_saved_records++;

		/* Move forward dns_answer_record->data_len for analyzing next
		 * record in the response */
		reader += ((dns_answer_record->type == DNS_RTYPE_SRV)
			   ? offset
			   : dns_answer_record->data_len);

		/* Lookup to see if we already had this entry */
		found = 0;
		list_for_each_entry(tmp_record, &dns_p->answer_list, list) {
			if (tmp_record->type != dns_answer_record->type)
				continue;

			switch(tmp_record->type) {
				case DNS_RTYPE_A:
					if (!memcmp(&((struct sockaddr_in *)&dns_answer_record->address)->sin_addr,
						    &((struct sockaddr_in *)&tmp_record->address)->sin_addr,
						    sizeof(in_addr_t)))
						found = 1;
					break;

				case DNS_RTYPE_AAAA:
					if (!memcmp(&((struct sockaddr_in6 *)&dns_answer_record->address)->sin6_addr,
						    &((struct sockaddr_in6 *)&tmp_record->address)->sin6_addr,
						    sizeof(struct in6_addr)))
						found = 1;
					break;

			case DNS_RTYPE_SRV:
                                if (dns_answer_record->data_len == tmp_record->data_len &&
				    !memcmp(dns_answer_record->target, tmp_record->target, dns_answer_record->data_len) &&
				    dns_answer_record->port == tmp_record->port) {
					tmp_record->weight = dns_answer_record->weight;
                                        found = 1;
				}
                                break;

			default:
				break;
			}

			if (found == 1)
				break;
		}

		if (found == 1) {
			tmp_record->last_seen = now.tv_sec;
			pool_free(dns_answer_item_pool, dns_answer_record);
		}
		else {
			dns_answer_record->last_seen = now.tv_sec;
			LIST_ADDQ(&dns_p->answer_list, &dns_answer_record->list);
		}
	} /* for i 0 to ancount */

	/* Save the number of records we really own */
	dns_p->header.ancount = nb_saved_records;
	dns_check_dns_response(resolution);
	return DNS_RESP_VALID;
}
