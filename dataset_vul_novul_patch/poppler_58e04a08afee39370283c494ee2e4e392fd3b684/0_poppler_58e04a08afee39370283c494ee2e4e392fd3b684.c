void JBIG2Stream::readSegments() {
  Guint segNum, segFlags, segType, page, segLength;
  Guint refFlags, nRefSegs;
  Guint *refSegs;
  Goffset segDataPos;
  int c1, c2, c3;
  Guint i;

  while (readULong(&segNum)) {

    if (!readUByte(&segFlags)) {
      goto eofError1;
    }
    segType = segFlags & 0x3f;

    if (!readUByte(&refFlags)) {
      goto eofError1;
    }
    nRefSegs = refFlags >> 5;
    if (nRefSegs == 7) {
      if ((c1 = curStr->getChar()) == EOF ||
	  (c2 = curStr->getChar()) == EOF ||
	  (c3 = curStr->getChar()) == EOF) {
	goto eofError1;
      }
      refFlags = (refFlags << 24) | (c1 << 16) | (c2 << 8) | c3;
      nRefSegs = refFlags & 0x1fffffff;
      for (i = 0; i < (nRefSegs + 9) >> 3; ++i) {
	if ((c1 = curStr->getChar()) == EOF) {
	  goto eofError1;
	}
      }
    }

    refSegs = (Guint *)gmallocn(nRefSegs, sizeof(Guint));
    if (segNum <= 256) {
      for (i = 0; i < nRefSegs; ++i) {
	if (!readUByte(&refSegs[i])) {
	  goto eofError2;
	}
      }
    } else if (segNum <= 65536) {
      for (i = 0; i < nRefSegs; ++i) {
	if (!readUWord(&refSegs[i])) {
	  goto eofError2;
	}
      }
    } else {
      for (i = 0; i < nRefSegs; ++i) {
	if (!readULong(&refSegs[i])) {
	  goto eofError2;
	}
      }
    }

    if (segFlags & 0x40) {
      if (!readULong(&page)) {
	goto eofError2;
      }
    } else {
      if (!readUByte(&page)) {
	goto eofError2;
      }
    }

    if (!readULong(&segLength)) {
      goto eofError2;
    }

    segDataPos = curStr->getPos();

    if (!pageBitmap && ((segType >= 4 && segType <= 7) ||
			(segType >= 20 && segType <= 43))) {
      error(errSyntaxError, curStr->getPos(), "First JBIG2 segment associated with a page must be a page information segment");
      goto syntaxError;
    }

    switch (segType) {
    case 0:
      if (!readSymbolDictSeg(segNum, segLength, refSegs, nRefSegs)) {
	goto syntaxError;
      }
      break;
    case 4:
      readTextRegionSeg(segNum, gFalse, gFalse, segLength, refSegs, nRefSegs);
      break;
    case 6:
      readTextRegionSeg(segNum, gTrue, gFalse, segLength, refSegs, nRefSegs);
      break;
    case 7:
      readTextRegionSeg(segNum, gTrue, gTrue, segLength, refSegs, nRefSegs);
      break;
    case 16:
      readPatternDictSeg(segNum, segLength);
      break;
    case 20:
      readHalftoneRegionSeg(segNum, gFalse, gFalse, segLength,
			    refSegs, nRefSegs);
      break;
    case 22:
      readHalftoneRegionSeg(segNum, gTrue, gFalse, segLength,
			    refSegs, nRefSegs);
      break;
    case 23:
      readHalftoneRegionSeg(segNum, gTrue, gTrue, segLength,
			    refSegs, nRefSegs);
      break;
    case 36:
      readGenericRegionSeg(segNum, gFalse, gFalse, segLength);
      break;
    case 38:
      readGenericRegionSeg(segNum, gTrue, gFalse, segLength);
      break;
    case 39:
      readGenericRegionSeg(segNum, gTrue, gTrue, segLength);
      break;
    case 40:
      readGenericRefinementRegionSeg(segNum, gFalse, gFalse, segLength,
				     refSegs, nRefSegs);
      break;
    case 42:
      readGenericRefinementRegionSeg(segNum, gTrue, gFalse, segLength,
				     refSegs, nRefSegs);
      break;
    case 43:
      readGenericRefinementRegionSeg(segNum, gTrue, gTrue, segLength,
				     refSegs, nRefSegs);
      break;
    case 48:
      readPageInfoSeg(segLength);
      break;
    case 50:
      readEndOfStripeSeg(segLength);
      break;
    case 52:
      readProfilesSeg(segLength);
      break;
    case 53:
      readCodeTableSeg(segNum, segLength);
      break;
    case 62:
      readExtensionSeg(segLength);
      break;
    default:
      error(errSyntaxError, curStr->getPos(), "Unknown segment type in JBIG2 stream");
      for (i = 0; i < segLength; ++i) {
	if ((c1 = curStr->getChar()) == EOF) {
	  goto eofError2;
	}
      }
      break;
    }


    if (segLength != 0xffffffff) {

      Goffset segExtraBytes = segDataPos + segLength - curStr->getPos();
      if (segExtraBytes > 0) {

	
 	
	error(errSyntaxError, curStr->getPos(), "{0:lld} extraneous byte{1:s} after segment",
 	      segExtraBytes, (segExtraBytes > 1) ? "s" : "");
 	
	
	int trash;
	for (Goffset i = segExtraBytes; i > 0; i--) {
	  readByte(&trash);
	}
	
      } else if (segExtraBytes < 0) {
	
	
	error(errSyntaxError, curStr->getPos(), "Previous segment handler read too many bytes");
	
      }

    }
    
    gfree(refSegs);
  }

  return;

 syntaxError:
  gfree(refSegs);
  return;

 eofError2:
  gfree(refSegs);
 eofError1:
  error(errSyntaxError, curStr->getPos(), "Unexpected EOF in JBIG2 stream");
}
