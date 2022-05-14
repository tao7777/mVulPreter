Bool PVDecodeVopBody(VideoDecControls *decCtrl, int32 buffer_size[])
{
    PV_STATUS status = PV_FAIL;
 VideoDecData *video = (VideoDecData *) decCtrl->videoDecoderData;
 int target_layer = video->currLayer;
 Vol *currVol = video->vol[target_layer];
 Vop *currVop = video->currVop;
 Vop *prevVop = video->prevVop;
 Vop *tempVopPtr;
 int bytes_consumed = 0; /* Record how many bits we used in the buffer.   04/24/2001 */

 int idx;

 if (currVop->vopCoded == 0) /*  07/03/2001 */
 {
        PV_BitstreamByteAlign(currVol->bitstream);
 /* We should always clear up bitstream buffer.   10/10/2000 */
        bytes_consumed = (getPointer(currVol->bitstream) + 7) >> 3;

 if (bytes_consumed > currVol->bitstream->data_end_pos)
 {
            bytes_consumed = currVol->bitstream->data_end_pos;
 }

 if (bytes_consumed < buffer_size[target_layer])
 {
 /* If we only consume part of the bits in the buffer, take those */
 /*  out.     04/24/2001 */
 /*          oscl_memcpy(buffer[target_layer], buffer[target_layer]+bytes_consumed,
                            (buffer_size[target_layer]-=bytes_consumed)); */
            buffer_size[target_layer] -= bytes_consumed;
 }
 else
 {
            buffer_size[target_layer] = 0;
 }
#ifdef PV_MEMORY_POOL

 if (target_layer)
 {
 if (video->prevEnhcVop->timeStamp > video->prevVop->timeStamp)
 {
                video->prevVop = video->prevEnhcVop;

             }
         }
 
        if (!video->prevVop->yChan) {
            ALOGE("b/35269635");
            android_errorWriteLog(0x534e4554, "35269635");
            return PV_FALSE;
        }
         oscl_memcpy(currVop->yChan, video->prevVop->yChan, (decCtrl->size*3) / 2);
 
         video->prevVop = prevVop;

        video->concealFrame = currVop->yChan; /*  07/07/2001 */

        video->vop_coding_type = currVop->predictionType; /*  07/09/01 */

        decCtrl->outputFrame = currVop->yChan;

 /* Swap VOP pointers.  No enhc. frame oscl_memcpy() anymore!   04/24/2001 */
 if (target_layer)
 {
            tempVopPtr = video->prevEnhcVop;
            video->prevEnhcVop = video->currVop;
            video->currVop = tempVopPtr;
 }
 else
 {
            tempVopPtr = video->prevVop;
            video->prevVop = video->currVop;
            video->currVop = tempVopPtr;
 }
#else
 if (target_layer) /* this is necessary to avoid flashback problems   06/21/2002*/
 {
            video->prevEnhcVop->timeStamp = currVop->timeStamp;
 }
 else
 {
            video->prevVop->timeStamp = currVop->timeStamp;
 }
#endif
        video->vop_coding_type = currVop->predictionType; /*  07/09/01 */
 /* the following is necessary to avoid displaying an notCoded I-VOP at the beginning of a session
        or after random positioning  07/03/02*/
 if (currVop->predictionType == I_VOP)
 {
            video->vop_coding_type = P_VOP;
 }


 return PV_TRUE;
 }
 /* ======================================================= */
 /*  Decode vop body (if there is no error in the header!)  */
 /* ======================================================= */

 /* first, we need to select a reference frame */
 if (decCtrl->nLayers > 1)
 {
 if (currVop->predictionType == I_VOP)
 {
 /* do nothing here */
 }
 else if (currVop->predictionType == P_VOP)
 {
 switch (currVop->refSelectCode)
 {
 case 0 : /* most recently decoded enhancement vop */
 /* Setup video->prevVop before we call PV_DecodeVop().   04/24/2001 */
 if (video->prevEnhcVop->timeStamp >= video->prevVop->timeStamp)
                        video->prevVop = video->prevEnhcVop;
 break;

 case 1 : /* most recently displayed base-layer vop */
 if (target_layer)
 {
 if (video->prevEnhcVop->timeStamp > video->prevVop->timeStamp)
                            video->prevVop = video->prevEnhcVop;
 }
 break;

 case 2 : /* next base-layer vop in display order */
 break;

 case 3 : /* temporally coincident base-layer vop (no MV's) */
 break;
 }
 }
 else /* we have a B-Vop */
 {
            mp4dec_log("DecodeVideoFrame(): B-VOP not supported.\n");
 }
 }

 /* This is for the calculation of the frame rate and bitrate. */
    idx = ++video->frame_idx % BITRATE_AVERAGE_WINDOW;

 /* Calculate bitrate for this layer.   08/23/2000 */
    status = PV_DecodeVop(video);
    video->nBitsPerVop[idx] = getPointer(currVol->bitstream);
    video->prevTimestamp[idx] = currVop->timeStamp;

 /* restore video->prevVop after PV_DecodeVop().   04/24/2001 */
    video->prevVop = prevVop;

 /* Estimate the frame rate.   08/23/2000 */
    video->duration = video->prevTimestamp[idx];
    video->duration -= video->prevTimestamp[(++idx)%BITRATE_AVERAGE_WINDOW];
 if (video->duration > 0)
 { /* Only update framerate when the timestamp is right */
        video->frameRate = (int)(FRAMERATE_SCALE) / video->duration;
 }

 /* We should always clear up bitstream buffer.   10/10/2000 */
    bytes_consumed = (getPointer(currVol->bitstream) + 7) >> 3; /*  11/4/03 */

 if (bytes_consumed > currVol->bitstream->data_end_pos)
 {
        bytes_consumed = currVol->bitstream->data_end_pos;
 }

 if (bytes_consumed < buffer_size[target_layer])
 {
 /* If we only consume part of the bits in the buffer, take those */
 /*  out.     04/24/2001 */
 /*      oscl_memcpy(buffer[target_layer], buffer[target_layer]+bytes_consumed,
                    (buffer_size[target_layer]-=bytes_consumed)); */
        buffer_size[target_layer] -= bytes_consumed;
 }
 else
 {
        buffer_size[target_layer] = 0;
 }
 switch (status)
 {
 case PV_FAIL :
 return PV_FALSE; /* this will take care of concealment if we lose whole frame  */

 case PV_END_OF_VOP :
 /* we may want to differenciate PV_END_OF_VOP and PV_SUCCESS */
 /*    in the future.     05/10/2000                      */

 case PV_SUCCESS :
 /* Nohting is wrong :). */


            video->concealFrame = video->currVop->yChan; /*  07/07/2001 */

            video->vop_coding_type = video->currVop->predictionType; /*  07/09/01 */

            decCtrl->outputFrame = video->currVop->yChan;

 /* Swap VOP pointers.  No enhc. frame oscl_memcpy() anymore!   04/24/2001 */
 if (target_layer)
 {
                tempVopPtr = video->prevEnhcVop;
                video->prevEnhcVop = video->currVop;
                video->currVop = tempVopPtr;
 }
 else
 {
                tempVopPtr = video->prevVop;
                video->prevVop = video->currVop;
                video->currVop = tempVopPtr;
 }
 break;

 default :
 /* This will never happen */
 break;
 }

 return PV_TRUE;
}
