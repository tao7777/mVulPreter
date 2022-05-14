init_device (u2fh_devs * devs, struct u2fdevice *dev)
{
  unsigned char resp[1024];
  unsigned char nonce[8];
  if (obtain_nonce(nonce) != 0)
    {
      return U2FH_TRANSPORT_ERROR;
    }
  size_t resplen = sizeof (resp);
  dev->cid = CID_BROADCAST;

  if (u2fh_sendrecv
       (devs, dev->id, U2FHID_INIT, nonce, sizeof (nonce), resp,
        &resplen) == U2FH_OK)
     {
      U2FHID_INIT_RESP initresp;
      if (resplen > sizeof (initresp))
 	{
	  return U2FH_MEMORY_ERROR;
 	}
      memcpy (&initresp, resp, resplen);
      dev->cid = initresp.cid;
      dev->versionInterface = initresp.versionInterface;
      dev->versionMajor = initresp.versionMajor;
      dev->versionMinor = initresp.versionMinor;
      dev->capFlags = initresp.capFlags;
     }
   else
     {
      return U2FH_TRANSPORT_ERROR;
    }
  return U2FH_OK;
}
