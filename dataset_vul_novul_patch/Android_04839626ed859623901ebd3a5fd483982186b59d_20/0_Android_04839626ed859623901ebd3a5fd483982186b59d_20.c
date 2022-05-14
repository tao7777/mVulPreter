long Cluster::CreateBlockGroup(
    const long status = Parse(pos, len);
 
    if (status < 0) {  // error
      pLast = NULL;
      return status;
     }
 
    if (status > 0)  // no new block
      break;
  }
 
