void test_parser(void) {
 	int i, retval;
 	bzrtpPacket_t *zrtpPacket;
 
	/* Create zrtp Context to use H0-H3 chains and others */
	bzrtpContext_t *context87654321 = bzrtp_createBzrtpContext(0x87654321);
	bzrtpContext_t *context12345678 = bzrtp_createBzrtpContext(0x12345678);


	/* replace created H by the patterns one to be able to generate the correct packet */
	memcpy (context12345678->channelContext[0]->selfH[0], H12345678[0], 32);
	memcpy (context12345678->channelContext[0]->selfH[1], H12345678[1], 32);
	memcpy (context12345678->channelContext[0]->selfH[2], H12345678[2], 32);
	memcpy (context12345678->channelContext[0]->selfH[3], H12345678[3], 32);
	memcpy (context87654321->channelContext[0]->selfH[0], H87654321[0], 32);
	memcpy (context87654321->channelContext[0]->selfH[1], H87654321[1], 32);
	memcpy (context87654321->channelContext[0]->selfH[2], H87654321[2], 32);
	memcpy (context87654321->channelContext[0]->selfH[3], H87654321[3], 32);

	/* preset the key agreement algo in the contexts */
	context87654321->channelContext[0]->keyAgreementAlgo = ZRTP_KEYAGREEMENT_DH3k;
	context12345678->channelContext[0]->keyAgreementAlgo = ZRTP_KEYAGREEMENT_DH3k;
	context87654321->channelContext[0]->cipherAlgo = ZRTP_CIPHER_AES1;
	context12345678->channelContext[0]->cipherAlgo = ZRTP_CIPHER_AES1;
	context87654321->channelContext[0]->hashAlgo = ZRTP_HASH_S256;
	context12345678->channelContext[0]->hashAlgo = ZRTP_HASH_S256;

	updateCryptoFunctionPointers(context87654321->channelContext[0]);
	updateCryptoFunctionPointers(context12345678->channelContext[0]);

	/* set the zrtp and mac keys */
	context87654321->channelContext[0]->mackeyi = (uint8_t *)malloc(32);
	context12345678->channelContext[0]->mackeyi = (uint8_t *)malloc(32);
	context87654321->channelContext[0]->mackeyr = (uint8_t *)malloc(32);
	context12345678->channelContext[0]->mackeyr = (uint8_t *)malloc(32);

	context87654321->channelContext[0]->zrtpkeyi = (uint8_t *)malloc(16);
	context12345678->channelContext[0]->zrtpkeyi = (uint8_t *)malloc(16);
	context87654321->channelContext[0]->zrtpkeyr = (uint8_t *)malloc(16);
	context12345678->channelContext[0]->zrtpkeyr = (uint8_t *)malloc(16);

	memcpy(context12345678->channelContext[0]->mackeyi, mackeyi, 32);
	memcpy(context12345678->channelContext[0]->mackeyr, mackeyr, 32);
	memcpy(context12345678->channelContext[0]->zrtpkeyi, zrtpkeyi, 16);
	memcpy(context12345678->channelContext[0]->zrtpkeyr, zrtpkeyr, 16);
	memcpy(context87654321->channelContext[0]->mackeyi, mackeyi, 32);
	memcpy(context87654321->channelContext[0]->mackeyr, mackeyr, 32);
	memcpy(context87654321->channelContext[0]->zrtpkeyi, zrtpkeyi, 16);
	memcpy(context87654321->channelContext[0]->zrtpkeyr, zrtpkeyr, 16);

	/* set the role: 87654321 is initiator in our exchange pattern */
	context12345678->channelContext[0]->role = RESPONDER;

	for (i=0; i<TEST_PACKET_NUMBER; i++) {
		uint8_t freePacketFlag = 1;
 		/* parse a packet string from patterns */
 		zrtpPacket = bzrtp_packetCheck(patternZRTPPackets[i], patternZRTPMetaData[i][0], (patternZRTPMetaData[i][1])-1, &retval);
 		retval +=  bzrtp_packetParser((patternZRTPMetaData[i][2]==0x87654321)?context12345678:context87654321, (patternZRTPMetaData[i][2]==0x87654321)?context12345678->channelContext[0]:context87654321->channelContext[0], patternZRTPPackets[i], patternZRTPMetaData[i][0], zrtpPacket);
		/*printf("parsing Ret val is %x index is %d\n", retval, i);*/
 		/* We must store some packets in the context if we want to be able to parse further packets */
 		if (zrtpPacket->messageType==MSGTYPE_HELLO) {
 			if (patternZRTPMetaData[i][2]==0x87654321) {
 				context12345678->channelContext[0]->peerPackets[HELLO_MESSAGE_STORE_ID] = zrtpPacket;
 			} else {
 				context87654321->channelContext[0]->peerPackets[HELLO_MESSAGE_STORE_ID] = zrtpPacket;
 			}
 			freePacketFlag = 0;
 		}
		if (zrtpPacket->messageType==MSGTYPE_COMMIT) {
			if (patternZRTPMetaData[i][2]==0x87654321) {
				context12345678->channelContext[0]->peerPackets[COMMIT_MESSAGE_STORE_ID] = zrtpPacket;
			} else {
				context87654321->channelContext[0]->peerPackets[COMMIT_MESSAGE_STORE_ID] = zrtpPacket;
			}
			freePacketFlag = 0;
		}
		if (zrtpPacket->messageType==MSGTYPE_DHPART1 || zrtpPacket->messageType==MSGTYPE_DHPART2) {
			if (patternZRTPMetaData[i][2]==0x87654321) {
				context12345678->channelContext[0]->peerPackets[DHPART_MESSAGE_STORE_ID] = zrtpPacket;
			} else {
				context87654321->channelContext[0]->peerPackets[DHPART_MESSAGE_STORE_ID] = zrtpPacket;
			}
			freePacketFlag = 0;
		}
		/* free the packet string as will be created again by the packetBuild function and might have been copied by packetParser */
		free(zrtpPacket->packetString);
		/* build a packet string from the parser packet*/
		retval = bzrtp_packetBuild((patternZRTPMetaData[i][2]==0x12345678)?context12345678:context87654321, (patternZRTPMetaData[i][2]==0x12345678)?context12345678->channelContext[0]:context87654321->channelContext[0], zrtpPacket, patternZRTPMetaData[i][1]);
		/* if (retval ==0) {
			packetDump(zrtpPacket, 1);
		} else {
			bzrtp_message("Ret val is %x index is %d\n", retval, i);
		}*/

		/* check they are the same */
		if (zrtpPacket->packetString != NULL) {
			CU_ASSERT_TRUE(memcmp(zrtpPacket->packetString, patternZRTPPackets[i], patternZRTPMetaData[i][0]) == 0);
		} else {
			CU_FAIL("Unable to build packet");
		}

 		if (freePacketFlag == 1) {
 			bzrtp_freeZrtpPacket(zrtpPacket);
 		}
 	}
 
 	bzrtp_destroyBzrtpContext(context87654321, 0x87654321);
 	bzrtp_destroyBzrtpContext(context12345678, 0x12345678);
 
 }
