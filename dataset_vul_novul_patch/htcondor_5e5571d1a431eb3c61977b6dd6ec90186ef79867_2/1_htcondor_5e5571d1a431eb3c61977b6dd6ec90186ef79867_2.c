int SafeSock::handle_incoming_packet()
{

	/* SOCKET_ALTERNATE_LENGTH_TYPE is void on this platform, and
		since noone knows what that void* is supposed to point to
		in recvfrom, I'm going to predict the "fromlen" variable
		the recvfrom uses is a size_t sized quantity since
		size_t is how you count bytes right?  Stupid Solaris. */
	bool last;
	int seqNo, length;
	_condorMsgID mID;
	void* data;
	int index;
	int received;
	_condorInMsg *tempMsg, *delMsg, *prev = NULL;
	time_t curTime;

	if( _msgReady ) {
		char const *existing_msg_type;
		bool existing_consumed;
		if( _longMsg ) {
			existing_msg_type = "long";
			existing_consumed = _longMsg->consumed();
		}
		else {
			existing_msg_type = "short";
			existing_consumed = _shortMsg.consumed();
		}
		dprintf( D_ALWAYS,
				 "ERROR: receiving new UDP message but found a %s "
				 "message still waiting to be closed (consumed=%d). "
				 "Closing it now.\n",
				 existing_msg_type, existing_consumed );


		stream_coding saved_coding = _coding;
		_coding = stream_decode;
		end_of_message();
		_coding = saved_coding;
	}


	received = condor_recvfrom(_sock, _shortMsg.dataGram, 
							   SAFE_MSG_MAX_PACKET_SIZE, 0, _who);

	if(received < 0) {
		dprintf(D_NETWORK, "recvfrom failed: errno = %d\n", errno);
                return FALSE;
        }
     char str[50];
    sprintf(str, sock_to_string(_sock));
     dprintf( D_NETWORK, "RECV %d bytes at %s from %s\n",
                         received, str, _who.to_sinful().Value());

	length = received;
    _shortMsg.reset(); // To be sure
	
	bool is_full_message = _shortMsg.getHeader(received, last, seqNo, length, mID, data);
	if ( length <= 0 || length > SAFE_MSG_MAX_PACKET_SIZE ) {
		dprintf(D_ALWAYS,"IO: Incoming datagram improperly sized\n");
		return FALSE;
	}

    if ( is_full_message ) {
        _shortMsg.curIndex = 0;
        _msgReady = true;
        _whole++;
        if(_whole == 1)
            _avgSwhole = length;
        else
            _avgSwhole = ((_whole - 1) * _avgSwhole + length) / _whole;
        
        _noMsgs++;
        dprintf( D_NETWORK, "\tFull msg [%d bytes]\n", length);
        return TRUE;
    }

    dprintf( D_NETWORK, "\tFrag [%d bytes]\n", length);
    
    /* long message */
    curTime = (unsigned long)time(NULL);
    index = labs(mID.ip_addr + mID.time + mID.msgNo) % SAFE_SOCK_HASH_BUCKET_SIZE;
    tempMsg = _inMsgs[index];
    while(tempMsg != NULL && !same(tempMsg->msgID, mID)) {
        prev = tempMsg;
        tempMsg = tempMsg->nextMsg;
        if(curTime - prev->lastTime > _tOutBtwPkts) {
            dprintf(D_NETWORK, "found timed out msg: cur=%lu, msg=%lu\n",
                    curTime, prev->lastTime);
            delMsg = prev;
            prev = delMsg->prevMsg;
            if(prev)
                prev->nextMsg = delMsg->nextMsg;
            else  // delMsg is the 1st message in the chain
                _inMsgs[index] = tempMsg;
            if(tempMsg)
                tempMsg->prevMsg = prev;
            _deleted++;
            if(_deleted == 1)
                _avgSdeleted = delMsg->msgLen;
            else     {
                _avgSdeleted = ((_deleted - 1) * _avgSdeleted + delMsg->msgLen) / _deleted;
            }   
            dprintf(D_NETWORK, "Deleting timeouted message:\n");
            delMsg->dumpMsg();
            delete delMsg;
        }   
    }   
    if(tempMsg != NULL) { // found
        if (seqNo == 0) {
            tempMsg->set_sec(_shortMsg.isDataMD5ed(),
                    _shortMsg.md(),
                    _shortMsg.isDataEncrypted());
        }
        bool rst = tempMsg->addPacket(last, seqNo, length, data);
        if (rst) {
            _longMsg = tempMsg;
            _msgReady = true;
            _whole++;
            if(_whole == 1)
                _avgSwhole = _longMsg->msgLen;
            else
                _avgSwhole = ((_whole - 1) * _avgSwhole + _longMsg->msgLen) / _whole;
            return TRUE;
        }
        return FALSE;
    } else { // not found
        if(prev) { // add a new message at the end of the chain
            prev->nextMsg = new _condorInMsg(mID, last, seqNo, length, data, 
                                             _shortMsg.isDataMD5ed(), 
                                             _shortMsg.md(), 
                                             _shortMsg.isDataEncrypted(), prev);
            if(!prev->nextMsg) {    
                EXCEPT("Error:handle_incomming_packet: Out of Memory");
            }
        } else { // first message in the bucket
            _inMsgs[index] = new _condorInMsg(mID, last, seqNo, length, data, 
                                              _shortMsg.isDataMD5ed(), 
                                              _shortMsg.md(), 
                                              _shortMsg.isDataEncrypted(), NULL);
            if(!_inMsgs[index]) {
                EXCEPT("Error:handle_incomming_packet: Out of Memory");
            }
        }
        _noMsgs++;
        return FALSE;
    }   
}
