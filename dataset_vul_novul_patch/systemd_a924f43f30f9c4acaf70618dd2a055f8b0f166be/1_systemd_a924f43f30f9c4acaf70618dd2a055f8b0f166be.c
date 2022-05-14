int dns_packet_is_reply_for(DnsPacket *p, const DnsResourceKey *key) {
        int r;

        assert(p);
        assert(key);

        /* Checks if the specified packet is a reply for the specified
         * key and the specified key is the only one in the question
         * section. */

        if (DNS_PACKET_QR(p) != 1)
                return 0;

        /* Let's unpack the packet, if that hasn't happened yet. */
        r = dns_packet_extract(p);
         if (r < 0)
                 return r;
 
         if (p->question->n_keys != 1)
                 return 0;
 
        return dns_resource_key_equal(p->question->keys[0], key);
}
