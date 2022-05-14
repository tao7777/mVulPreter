int bzrtp_packetParser(bzrtpContext_t *zrtpContext, bzrtpChannelContext_t *zrtpChannelContext, const uint8_t * input, uint16_t inputLength, bzrtpPacket_t *zrtpPacket) {

	int i;

	/* now allocate and fill the correct message structure according to the message type */
	/* messageContent points to the begining of the ZRTP message */
	uint8_t *messageContent = (uint8_t *)(input+ZRTP_PACKET_HEADER_LENGTH+ZRTP_MESSAGE_HEADER_LENGTH);

	switch (zrtpPacket->messageType) {
		case MSGTYPE_HELLO : 
			{
				/* allocate a Hello message structure */
				bzrtpHelloMessage_t *messageData;
				messageData = (bzrtpHelloMessage_t *)malloc(sizeof(bzrtpHelloMessage_t));

				/* fill it */
				memcpy(messageData->version, messageContent, 4);
				messageContent +=4;
				memcpy(messageData->clientIdentifier, messageContent, 16);
				messageContent +=16;
				memcpy(messageData->H3, messageContent, 32);
				messageContent +=32;
				memcpy(messageData->ZID, messageContent, 12);
				messageContent +=12;
				messageData->S = ((*messageContent)>>6)&0x01;
				messageData->M = ((*messageContent)>>5)&0x01;
				messageData->P = ((*messageContent)>>4)&0x01;
				messageContent +=1;
				messageData->hc = MIN((*messageContent)&0x0F, 7);
				messageContent +=1;
				messageData->cc = MIN(((*messageContent)>>4)&0x0F, 7);
				messageData->ac = MIN((*messageContent)&0x0F, 7);
				messageContent +=1;
				messageData->kc = MIN(((*messageContent)>>4)&0x0F, 7);
				messageData->sc = MIN((*messageContent)&0x0F, 7);
				messageContent +=1;

				/* Check message length according to value in hc, cc, ac, kc and sc */
				if (zrtpPacket->messageLength != ZRTP_HELLOMESSAGE_FIXED_LENGTH + 4*((uint16_t)(messageData->hc)+(uint16_t)(messageData->cc)+(uint16_t)(messageData->ac)+(uint16_t)(messageData->kc)+(uint16_t)(messageData->sc))) {
					free(messageData);
					return BZRTP_PARSER_ERROR_INVALIDMESSAGE;
				}

				/* parse the variable length part: algorithms types */
				for (i=0; i<messageData->hc; i++) {
					messageData->supportedHash[i] = cryptoAlgoTypeStringToInt(messageContent, ZRTP_HASH_TYPE);
					messageContent +=4;
				}
				for (i=0; i<messageData->cc; i++) {
					messageData->supportedCipher[i] = cryptoAlgoTypeStringToInt(messageContent, ZRTP_CIPHERBLOCK_TYPE);
					messageContent +=4;
				}
				for (i=0; i<messageData->ac; i++) {
					messageData->supportedAuthTag[i] = cryptoAlgoTypeStringToInt(messageContent, ZRTP_AUTHTAG_TYPE);
					messageContent +=4;
				}
				for (i=0; i<messageData->kc; i++) {
					messageData->supportedKeyAgreement[i] = cryptoAlgoTypeStringToInt(messageContent, ZRTP_KEYAGREEMENT_TYPE);
					messageContent +=4;
				}
				for (i=0; i<messageData->sc; i++) {
					messageData->supportedSas[i] = cryptoAlgoTypeStringToInt(messageContent, ZRTP_SAS_TYPE);
					messageContent +=4;
				}

				addMandatoryCryptoTypesIfNeeded(ZRTP_HASH_TYPE, messageData->supportedHash, &messageData->hc);
				addMandatoryCryptoTypesIfNeeded(ZRTP_CIPHERBLOCK_TYPE, messageData->supportedCipher, &messageData->cc);
				addMandatoryCryptoTypesIfNeeded(ZRTP_AUTHTAG_TYPE, messageData->supportedAuthTag, &messageData->ac);
				addMandatoryCryptoTypesIfNeeded(ZRTP_KEYAGREEMENT_TYPE, messageData->supportedKeyAgreement, &messageData->kc);
				addMandatoryCryptoTypesIfNeeded(ZRTP_SAS_TYPE, messageData->supportedSas, &messageData->sc);

				memcpy(messageData->MAC, messageContent, 8);
				
				/* attach the message structure to the packet one */
				zrtpPacket->messageData = (void *)messageData;

				/* the parsed Hello packet must be saved as it may be used to generate commit message or the total_hash */
				zrtpPacket->packetString = (uint8_t *)malloc(inputLength*sizeof(uint8_t));
				memcpy(zrtpPacket->packetString, input, inputLength); /* store the whole packet even if we may use the message only */
			}
			break; /* MSGTYPE_HELLO */

		case MSGTYPE_HELLOACK :
			{
				/* check message length */
				if (zrtpPacket->messageLength != ZRTP_HELLOACKMESSAGE_FIXED_LENGTH) {
					return BZRTP_PARSER_ERROR_INVALIDMESSAGE;
				}


			}
			break; /* MSGTYPE_HELLOACK */

		case MSGTYPE_COMMIT:
			{
				uint8_t checkH3[32];
				uint8_t checkMAC[32];
				bzrtpHelloMessage_t *peerHelloMessageData;
				uint16_t variableLength = 0;

				/* allocate a commit message structure */
				bzrtpCommitMessage_t *messageData;
				messageData = (bzrtpCommitMessage_t *)malloc(sizeof(bzrtpCommitMessage_t));

				/* fill the structure */
				memcpy(messageData->H2, messageContent, 32);
				messageContent +=32;

				/* We have now H2, check it matches the H3 we had in the hello message H3=SHA256(H2) and that the Hello message MAC is correct */
				if (zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID] == NULL) {
					free (messageData);
					/* we have no Hello message in this channel, this commit shall never have arrived, discard it as invalid */
					return BZRTP_PARSER_ERROR_UNEXPECTEDMESSAGE;
				}
				peerHelloMessageData = (bzrtpHelloMessage_t *)zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->messageData;
				/* Check H3 = SHA256(H2) */
				bctoolbox_sha256(messageData->H2, 32, 32, checkH3);
				if (memcmp(checkH3, peerHelloMessageData->H3, 32) != 0) {
					free (messageData);
					return BZRTP_PARSER_ERROR_UNMATCHINGHASHCHAIN;
				}
				/* Check the hello MAC message. 
				 * MAC is 8 bytes long and is computed on the message(skip the ZRTP_PACKET_HEADER) and exclude the mac itself (-8 bytes from message Length) */
				bctoolbox_hmacSha256(messageData->H2, 32, zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->packetString+ZRTP_PACKET_HEADER_LENGTH, zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->messageLength-8, 8, checkMAC);
				if (memcmp(checkMAC, peerHelloMessageData->MAC, 8) != 0) {
					free (messageData);
					return BZRTP_PARSER_ERROR_UNMATCHINGMAC;
				}

				memcpy(messageData->ZID, messageContent, 12);
				messageContent +=12;
				messageData->hashAlgo = cryptoAlgoTypeStringToInt(messageContent, ZRTP_HASH_TYPE);
				messageContent += 4;
				messageData->cipherAlgo = cryptoAlgoTypeStringToInt(messageContent, ZRTP_CIPHERBLOCK_TYPE);
				messageContent += 4;
				messageData->authTagAlgo = cryptoAlgoTypeStringToInt(messageContent, ZRTP_AUTHTAG_TYPE);
				messageContent += 4;
				messageData->keyAgreementAlgo = cryptoAlgoTypeStringToInt(messageContent, ZRTP_KEYAGREEMENT_TYPE);
				messageContent += 4;
				/* commit message length depends on the key agreement type choosen (and set in the zrtpContext->keyAgreementAlgo) */
				switch(messageData->keyAgreementAlgo) {
					case ZRTP_KEYAGREEMENT_DH2k :
					case ZRTP_KEYAGREEMENT_EC25 :
					case ZRTP_KEYAGREEMENT_DH3k :
					case ZRTP_KEYAGREEMENT_EC38 :
					case ZRTP_KEYAGREEMENT_EC52 :
						variableLength = 32; /* hvi is 32 bytes length in DH Commit message format */
						break;
					case ZRTP_KEYAGREEMENT_Prsh :
						variableLength = 24; /* nonce (16 bytes) and keyID(8 bytes) are 24 bytes length in preshared Commit message format */
						break;
					case ZRTP_KEYAGREEMENT_Mult :
						variableLength = 16; /* nonce is 24 bytes length in multistream Commit message format */
						break;
					default:
						free(messageData);
						return BZRTP_PARSER_ERROR_INVALIDMESSAGE;
				}

				if (zrtpPacket->messageLength != ZRTP_COMMITMESSAGE_FIXED_LENGTH + variableLength) {
					free(messageData);
					return BZRTP_PARSER_ERROR_INVALIDMESSAGE;
				}
				messageData->sasAlgo = cryptoAlgoTypeStringToInt(messageContent, ZRTP_SAS_TYPE);
				messageContent += 4;

				/* if it is a multistream or preshared commit, get the 16 bytes nonce */
				if ((messageData->keyAgreementAlgo == ZRTP_KEYAGREEMENT_Prsh) || (messageData->keyAgreementAlgo == ZRTP_KEYAGREEMENT_Mult)) {
					memcpy(messageData->nonce, messageContent, 16);
					messageContent +=16;

					/* and the keyID for preshared commit only */
					if (messageData->keyAgreementAlgo == ZRTP_KEYAGREEMENT_Prsh) {
						memcpy(messageData->keyID, messageContent, 8);
						messageContent +=8;
					}
				} else { /* it's a DH commit message, get the hvi */
					memcpy(messageData->hvi, messageContent, 32);
					messageContent +=32;
				}

				/* get the MAC and attach the message data to the packet structure */
				memcpy(messageData->MAC, messageContent, 8);
				zrtpPacket->messageData = (void *)messageData;

				/* the parsed commit packet must be saved as it is used to generate the total_hash */
				zrtpPacket->packetString = (uint8_t *)malloc(inputLength*sizeof(uint8_t));
				memcpy(zrtpPacket->packetString, input, inputLength); /* store the whole packet even if we may use the message only */
			}
			break; /* MSGTYPE_COMMIT */
		case MSGTYPE_DHPART1 :
		case MSGTYPE_DHPART2 :
			{
				bzrtpDHPartMessage_t *messageData;

				/*check message length, depends on the selected key agreement algo set in zrtpContext */
				uint16_t pvLength = computeKeyAgreementPrivateValueLength(zrtpChannelContext->keyAgreementAlgo);
				if (pvLength == 0) {
					return BZRTP_PARSER_ERROR_INVALIDCONTEXT;
				}

				if (zrtpPacket->messageLength != ZRTP_DHPARTMESSAGE_FIXED_LENGTH+pvLength) {
					return BZRTP_PARSER_ERROR_INVALIDMESSAGE;
				}

				/* allocate a DHPart message structure and pv */
				messageData = (bzrtpDHPartMessage_t *)malloc(sizeof(bzrtpDHPartMessage_t));
				messageData->pv = (uint8_t *)malloc(pvLength*sizeof(uint8_t));

				/* fill the structure */
				memcpy(messageData->H1, messageContent, 32);
				messageContent +=32;

				/* We have now H1, check it matches the H2 we had in the commit message H2=SHA256(H1) and that the Commit message MAC is correct */
				if ( zrtpChannelContext->role == RESPONDER) { /* do it only if we are responder (we received a commit packet) */
					uint8_t checkH2[32];
					uint8_t checkMAC[32];
					bzrtpCommitMessage_t *peerCommitMessageData;

					if (zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID] == NULL) {
						free (messageData);
						/* we have no Commit message in this channel, this DHPart2 shall never have arrived, discard it as invalid */
						return BZRTP_PARSER_ERROR_UNEXPECTEDMESSAGE;
					}
					peerCommitMessageData = (bzrtpCommitMessage_t *)zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID]->messageData;
					/* Check H2 = SHA256(H1) */
					bctoolbox_sha256(messageData->H1, 32, 32, checkH2);
					if (memcmp(checkH2, peerCommitMessageData->H2, 32) != 0) {
						free (messageData);
						return BZRTP_PARSER_ERROR_UNMATCHINGHASHCHAIN;
					}
					/* Check the Commit MAC message. 
					 * MAC is 8 bytes long and is computed on the message(skip the ZRTP_PACKET_HEADER) and exclude the mac itself (-8 bytes from message Length) */
					bctoolbox_hmacSha256(messageData->H1, 32, zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID]->packetString+ZRTP_PACKET_HEADER_LENGTH, zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID]->messageLength-8, 8, checkMAC);
					if (memcmp(checkMAC, peerCommitMessageData->MAC, 8) != 0) {
 						free (messageData);
 						return BZRTP_PARSER_ERROR_UNMATCHINGMAC;
 					}
 				} else { /* if we are initiator(we didn't received any commit message and then no H2), we must check that H3=SHA256(SHA256(H1)) and the Hello message MAC */
 					uint8_t checkH2[32];
 					uint8_t checkH3[32];
					uint8_t checkMAC[32];
					bzrtpHelloMessage_t *peerHelloMessageData;

					if (zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID] == NULL) {
						free (messageData);
						/* we have no Hello message in this channel, this DHPart1 shall never have arrived, discard it as invalid */
						return BZRTP_PARSER_ERROR_UNEXPECTEDMESSAGE;
					}
					peerHelloMessageData = (bzrtpHelloMessage_t *)zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->messageData;
					/* Check H3 = SHA256(SHA256(H1)) */
					bctoolbox_sha256(messageData->H1, 32, 32, checkH2);
					bctoolbox_sha256(checkH2, 32, 32, checkH3);
					if (memcmp(checkH3, peerHelloMessageData->H3, 32) != 0) {
						free (messageData);
						return BZRTP_PARSER_ERROR_UNMATCHINGHASHCHAIN;
					}
					/* Check the hello MAC message. 
					 * MAC is 8 bytes long and is computed on the message(skip the ZRTP_PACKET_HEADER) and exclude the mac itself (-8 bytes from message Length) */
					bctoolbox_hmacSha256(checkH2, 32, zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->packetString+ZRTP_PACKET_HEADER_LENGTH, zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->messageLength-8, 8, checkMAC);
					if (memcmp(checkMAC, peerHelloMessageData->MAC, 8) != 0) {
						free (messageData);
						return BZRTP_PARSER_ERROR_UNMATCHINGMAC;
					}

				}

				memcpy(messageData->rs1ID, messageContent, 8);
				messageContent +=8;
				memcpy(messageData->rs2ID, messageContent, 8);
				messageContent +=8;
				memcpy(messageData->auxsecretID, messageContent, 8);
				messageContent +=8;
				memcpy(messageData->pbxsecretID, messageContent, 8);
				messageContent +=8;
				memcpy(messageData->pv, messageContent, pvLength);
				messageContent +=pvLength;
				memcpy(messageData->MAC, messageContent, 8);

 				/* attach the message structure to the packet one */
 				zrtpPacket->messageData = (void *)messageData;
 
				/* the parsed commit packet must be saved as it is used to generate the total_hash */
 				zrtpPacket->packetString = (uint8_t *)malloc(inputLength*sizeof(uint8_t));
 				memcpy(zrtpPacket->packetString, input, inputLength); /* store the whole packet even if we may use the message only */
 			}
			break; /* MSGTYPE_DHPART1 and MSGTYPE_DHPART2 */
		case MSGTYPE_CONFIRM1:
		case MSGTYPE_CONFIRM2:
			{
				uint8_t *confirmMessageKey = NULL;
				uint8_t *confirmMessageMacKey = NULL;
				bzrtpConfirmMessage_t *messageData;
				uint16_t cipherTextLength;
				uint8_t computedHmac[8];
				uint8_t *confirmPlainMessageBuffer;
				uint8_t *confirmPlainMessage;

				/* we shall first decrypt and validate the message, check we have the keys to do it */
				if (zrtpChannelContext->role == RESPONDER) { /* responder uses initiator's keys to decrypt */
					if ((zrtpChannelContext->zrtpkeyi == NULL) || (zrtpChannelContext->mackeyi == NULL)) {
						return BZRTP_PARSER_ERROR_INVALIDCONTEXT;
					}
					confirmMessageKey = zrtpChannelContext->zrtpkeyi;
					confirmMessageMacKey = zrtpChannelContext->mackeyi;
				}

				if (zrtpChannelContext->role == INITIATOR) { /* the iniator uses responder's keys to decrypt */
					if ((zrtpChannelContext->zrtpkeyr == NULL) || (zrtpChannelContext->mackeyr == NULL)) {
						return BZRTP_PARSER_ERROR_INVALIDCONTEXT;
					}
					confirmMessageKey = zrtpChannelContext->zrtpkeyr;
					confirmMessageMacKey = zrtpChannelContext->mackeyr;
				}
				
				/* allocate a confirm message structure */
				messageData = (bzrtpConfirmMessage_t *)malloc(sizeof(bzrtpConfirmMessage_t));

				/* get the mac and the IV */
				memcpy(messageData->confirm_mac, messageContent, 8);
				messageContent +=8;
				memcpy(messageData->CFBIV, messageContent, 16);
				messageContent +=16;


				
				/* get the cipher text length */
				cipherTextLength = zrtpPacket->messageLength - ZRTP_MESSAGE_HEADER_LENGTH - 24; /* confirm message is header, confirm_mac(8 bytes), CFB IV(16 bytes), encrypted part */

				/* validate the mac over the cipher text */
				zrtpChannelContext->hmacFunction(confirmMessageMacKey, zrtpChannelContext->hashLength, messageContent, cipherTextLength, 8, computedHmac);
				
				if (memcmp(computedHmac, messageData->confirm_mac, 8) != 0) { /* confirm_mac doesn't match */
					free(messageData);
					return BZRTP_PARSER_ERROR_UNMATCHINGCONFIRMMAC;
				}

				/* get plain message */
				confirmPlainMessageBuffer = (uint8_t *)malloc(cipherTextLength*sizeof(uint8_t));
				zrtpChannelContext->cipherDecryptionFunction(confirmMessageKey, messageData->CFBIV, messageContent, cipherTextLength, confirmPlainMessageBuffer);
				confirmPlainMessage = confirmPlainMessageBuffer; /* point into the allocated buffer */

				/* parse it */
				memcpy(messageData->H0, confirmPlainMessage, 32);
				confirmPlainMessage +=33; /* +33 because next 8 bits are unused */

				/* Hash chain checking: if we are in multichannel or shared mode, we had not DHPart and then no H1 */
				if (zrtpChannelContext->keyAgreementAlgo == ZRTP_KEYAGREEMENT_Prsh || zrtpChannelContext->keyAgreementAlgo == ZRTP_KEYAGREEMENT_Mult) {
					/* compute the H1=SHA256(H0) we never received */
					uint8_t checkH1[32];
					bctoolbox_sha256(messageData->H0, 32, 32, checkH1);

					/* if we are responder, we received a commit packet with H2 then check that H2=SHA256(H1) and that the commit message MAC keyed with H1 match */
					if ( zrtpChannelContext->role == RESPONDER) {
						uint8_t checkH2[32];
						uint8_t checkMAC[32];
						bzrtpCommitMessage_t *peerCommitMessageData;

						if (zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID] == NULL) {
							free (messageData);
							/* we have no Commit message in this channel, this Confirm2 shall never have arrived, discard it as invalid */
							return BZRTP_PARSER_ERROR_UNEXPECTEDMESSAGE;
						}
						peerCommitMessageData = (bzrtpCommitMessage_t *)zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID]->messageData;
						/* Check H2 = SHA256(H1) */
						bctoolbox_sha256(checkH1, 32, 32, checkH2);
						if (memcmp(checkH2, peerCommitMessageData->H2, 32) != 0) {
							free (messageData);
							return BZRTP_PARSER_ERROR_UNMATCHINGHASHCHAIN;
						}
						/* Check the Commit MAC message. 
						 * MAC is 8 bytes long and is computed on the message(skip the ZRTP_PACKET_HEADER) and exclude the mac itself (-8 bytes from message Length) */
						bctoolbox_hmacSha256(checkH1, 32, zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID]->packetString+ZRTP_PACKET_HEADER_LENGTH, zrtpChannelContext->peerPackets[COMMIT_MESSAGE_STORE_ID]->messageLength-8, 8, checkMAC);
						if (memcmp(checkMAC, peerCommitMessageData->MAC, 8) != 0) {
							free (messageData);
							return BZRTP_PARSER_ERROR_UNMATCHINGMAC;
						}
					} else { /* if we are initiator(we didn't received any commit message and then no H2), we must check that H3=SHA256(SHA256(H1)) and the Hello message MAC */
						uint8_t checkH2[32];
						uint8_t checkH3[32];
						uint8_t checkMAC[32];
						bzrtpHelloMessage_t *peerHelloMessageData;

						if (zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID] == NULL) {
							free (messageData);
							/* we have no Hello message in this channel, this Confirm1 shall never have arrived, discard it as invalid */
							return BZRTP_PARSER_ERROR_UNEXPECTEDMESSAGE;
						}
						peerHelloMessageData = (bzrtpHelloMessage_t *)zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->messageData;
						/* Check H3 = SHA256(SHA256(H1)) */
						bctoolbox_sha256(checkH1, 32, 32, checkH2);
						bctoolbox_sha256(checkH2, 32, 32, checkH3);
						if (memcmp(checkH3, peerHelloMessageData->H3, 32) != 0) {
							free (messageData);
							return BZRTP_PARSER_ERROR_UNMATCHINGHASHCHAIN;
						}
						/* Check the hello MAC message. 
						 * MAC is 8 bytes long and is computed on the message(skip the ZRTP_PACKET_HEADER) and exclude the mac itself (-8 bytes from message Length) */
						bctoolbox_hmacSha256(checkH2, 32, zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->packetString+ZRTP_PACKET_HEADER_LENGTH, zrtpChannelContext->peerPackets[HELLO_MESSAGE_STORE_ID]->messageLength-8, 8, checkMAC);
						if (memcmp(checkMAC, peerHelloMessageData->MAC, 8) != 0) {
							free (messageData);
							return BZRTP_PARSER_ERROR_UNMATCHINGMAC;
						}
	
					}
				} else { /* we are in DHM mode */
					/* We have now H0, check it matches the H1 we had in the DHPart message H1=SHA256(H0) and that the DHPart message MAC is correct */
					uint8_t checkH1[32];
					uint8_t checkMAC[32];
					bzrtpDHPartMessage_t *peerDHPartMessageData;

					if (zrtpChannelContext->peerPackets[DHPART_MESSAGE_STORE_ID] == NULL) {
						free (messageData);
						/* we have no DHPART message in this channel, this confirm shall never have arrived, discard it as invalid */
						return BZRTP_PARSER_ERROR_UNEXPECTEDMESSAGE;
					}
					peerDHPartMessageData = (bzrtpDHPartMessage_t *)zrtpChannelContext->peerPackets[DHPART_MESSAGE_STORE_ID]->messageData;
					/* Check H1 = SHA256(H0) */
					bctoolbox_sha256(messageData->H0, 32, 32, checkH1);
					if (memcmp(checkH1, peerDHPartMessageData->H1, 32) != 0) {
						free (messageData);
						return BZRTP_PARSER_ERROR_UNMATCHINGHASHCHAIN;
					}
					/* Check the DHPart message. 
					 * MAC is 8 bytes long and is computed on the message(skip the ZRTP_PACKET_HEADER) and exclude the mac itself (-8 bytes from message Length) */
					bctoolbox_hmacSha256(messageData->H0, 32, zrtpChannelContext->peerPackets[DHPART_MESSAGE_STORE_ID]->packetString+ZRTP_PACKET_HEADER_LENGTH, zrtpChannelContext->peerPackets[DHPART_MESSAGE_STORE_ID]->messageLength-8, 8, checkMAC);
					if (memcmp(checkMAC, peerDHPartMessageData->MAC, 8) != 0) {
						free (messageData);
						return BZRTP_PARSER_ERROR_UNMATCHINGMAC;
					}
				}

				messageData->sig_len = ((uint16_t)(confirmPlainMessage[0]&0x01))<<8 | (((uint16_t)confirmPlainMessage[1])&0x00FF);
				confirmPlainMessage += 2;
				messageData->E = ((*confirmPlainMessage)&0x08)>>3;
				messageData->V = ((*confirmPlainMessage)&0x04)>>2;
				messageData->A = ((*confirmPlainMessage)&0x02)>>1;
				messageData->D = (*confirmPlainMessage)&0x01;
				confirmPlainMessage += 1;

				messageData->cacheExpirationInterval = (((uint32_t)confirmPlainMessage[0])<<24) | (((uint32_t)confirmPlainMessage[1])<<16) | (((uint32_t)confirmPlainMessage[2])<<8) | ((uint32_t)confirmPlainMessage[3]);
				confirmPlainMessage += 4;


				/* if sig_len indicate a signature, parse it */
				if (messageData->sig_len>0) {
					memcpy(messageData->signatureBlockType, confirmPlainMessage, 4);
					confirmPlainMessage += 4;
					/* allocate memory for the signature block, sig_len is in words(32 bits) and includes the signature block type word */
					messageData->signatureBlock = (uint8_t *)malloc(4*(messageData->sig_len-1)*sizeof(uint8_t));
					memcpy(messageData->signatureBlock, confirmPlainMessage, 4*(messageData->sig_len-1));
				} else {
					messageData->signatureBlock  = NULL;
				}

				/* free plain buffer */
				free(confirmPlainMessageBuffer);

				/* the parsed commit packet must be saved as it is used to check correct packet repetition */
				zrtpPacket->packetString = (uint8_t *)malloc(inputLength*sizeof(uint8_t));
				memcpy(zrtpPacket->packetString, input, inputLength); /* store the whole packet even if we may use the message only */

				/* attach the message structure to the packet one */
				zrtpPacket->messageData = (void *)messageData;
			}
			break; /* MSGTYPE_CONFIRM1 and MSGTYPE_CONFIRM2 */

		case MSGTYPE_CONF2ACK:
			/* nothing to do for this one */
			break; /* MSGTYPE_CONF2ACK */
		
		case MSGTYPE_PING:
			{
				/* allocate a ping message structure */
				bzrtpPingMessage_t *messageData;
				messageData = (bzrtpPingMessage_t *)malloc(sizeof(bzrtpPingMessage_t));

				/* fill the structure */
				memcpy(messageData->version, messageContent, 4);
				messageContent +=4;
				memcpy(messageData->endpointHash, messageContent, 8);

				/* attach the message structure to the packet one */
				zrtpPacket->messageData = (void *)messageData;
			}
			break; /* MSGTYPE_PING */

	}

	return 0;
}
