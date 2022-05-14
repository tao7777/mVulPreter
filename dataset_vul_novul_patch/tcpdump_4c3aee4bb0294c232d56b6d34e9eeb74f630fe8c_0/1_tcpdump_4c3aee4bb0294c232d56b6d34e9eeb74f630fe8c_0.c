static u_int mp_dss_len(const  struct mp_dss *m, int csum)
{
        u_int len;
        len = 4;
        if (m->flags & MP_DSS_A) {
                /* Ack present - 4 or 8 octets */
                len += (m->flags & MP_DSS_a) ? 8 : 4;
        }
        if (m->flags & MP_DSS_M) {
                /*
                 * Data Sequence Number (DSN), Subflow Sequence Number (SSN),
                 * Data-Level Length present, and Checksum possibly present.
                 * All but the Checksum are 10 bytes if the m flag is
                 * clear (4-byte DSN) and 14 bytes if the m flag is set
                 * (8-byte DSN).
                 */
                len += (m->flags & MP_DSS_m) ? 14 : 10;
                /*
                 * The Checksum is present only if negotiated.
                 */
                if (csum)
                        len += 2;
	}
	return len;
}
