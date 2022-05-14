dns_stricmp(const char* str1, const char* str2)
{
  char c1, c2;
  *----------------------------------------------------------------------------*/
 
 /* DNS variables */
static struct udp_pcb        *dns_pcbs[DNS_MAX_SOURCE_PORTS];
#if ((LWIP_DNS_SECURE & LWIP_DNS_SECURE_RAND_SRC_PORT) != 0)
static u8_t                   dns_last_pcb_idx;
#endif
 static u8_t                   dns_seqno;
 static struct dns_table_entry dns_table[DNS_TABLE_SIZE];
 static struct dns_req_entry   dns_requests[DNS_MAX_REQUESTS];
        if (c1_upc != c2_upc) {
          /* still not equal */
          /* don't care for < or > */
          return 1;
        }
      } else {
        /* characters are not equal but none is in the alphabet range */
        return 1;
      }
