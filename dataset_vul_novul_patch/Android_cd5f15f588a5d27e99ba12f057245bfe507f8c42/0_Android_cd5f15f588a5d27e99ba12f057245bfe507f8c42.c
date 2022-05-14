static void debugCallback (int fd, short flags, void *param) {
 int acceptFD, option;
 struct sockaddr_un peeraddr;
 socklen_t socklen = sizeof (peeraddr);

     int data;
     unsigned int qxdm_data[6];
     const char *deactData[1] = {"1"};
     RIL_Dial dialData;
     int hangupData[1] = {1};
     int number;
     char **args;
     RIL_SOCKET_ID socket_id = RIL_SOCKET_1;
    int MAX_DIAL_ADDRESS = 128;
     int sim_id = 0;
 
     RLOGI("debugCallback for socket %s", rilSocketIdToString(socket_id));

    acceptFD = accept (fd, (sockaddr *) &peeraddr, &socklen);

 if (acceptFD < 0) {
        RLOGE ("error accepting on debug port: %d\n", errno);
 return;
 }

 if (recv(acceptFD, &number, sizeof(int), 0) != sizeof(int)) {
        RLOGE ("error reading on socket: number of Args: \n");
        close(acceptFD);
 return;
 }

 if (number < 0) {
        RLOGE ("Invalid number of arguments: \n");
        close(acceptFD);
 return;
 }

    args = (char **) calloc(number, sizeof(char*));
 if (args == NULL) {
        RLOGE("Memory allocation failed for debug args");
        close(acceptFD);
 return;
 }

 for (int i = 0; i < number; i++) {
 int len;
 if (recv(acceptFD, &len, sizeof(int), 0) != sizeof(int)) {
            RLOGE ("error reading on socket: Len of Args: \n");
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
 return;
 }
 if (len == INT_MAX || len < 0) {
            RLOGE("Invalid value of len: \n");
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
 return;
 }

        args[i] = (char *) calloc(len + 1, sizeof(char));
 if (args[i] == NULL) {
            RLOGE("Memory allocation failed for debug args");
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
 return;
 }
 if (recv(acceptFD, args[i], sizeof(char) * len, 0)
 != (int)sizeof(char) * len) {
            RLOGE ("error reading on socket: Args[%d] \n", i);
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
 return;
 }
 char * buf = args[i];
        buf[len] = 0;
 if ((i+1) == number) {
 /* The last argument should be sim id 0(SIM1)~3(SIM4) */
            sim_id = atoi(args[i]);
 switch (sim_id) {
 case 0:
                    socket_id = RIL_SOCKET_1;
 break;
 #if (SIM_COUNT >= 2)
 case 1:
                    socket_id = RIL_SOCKET_2;
 break;
 #endif
 #if (SIM_COUNT >= 3)
 case 2:
                    socket_id = RIL_SOCKET_3;
 break;
 #endif
 #if (SIM_COUNT >= 4)
 case 3:
                    socket_id = RIL_SOCKET_4;
 break;
 #endif
 default:
                    socket_id = RIL_SOCKET_1;
 break;
 }
 }
 }

 switch (atoi(args[0])) {
 case 0:
            RLOGI ("Connection on debug port: issuing reset.");
            issueLocalRequest(RIL_REQUEST_RESET_RADIO, NULL, 0, socket_id);
 break;
 case 1:
            RLOGI ("Connection on debug port: issuing radio power off.");
            data = 0;
            issueLocalRequest(RIL_REQUEST_RADIO_POWER, &data, sizeof(int), socket_id);
 if (socket_id == RIL_SOCKET_1 && s_ril_param_socket.fdCommand > 0) {
                close(s_ril_param_socket.fdCommand);
                s_ril_param_socket.fdCommand = -1;
 }
 #if (SIM_COUNT == 2)
 else if (socket_id == RIL_SOCKET_2 && s_ril_param_socket2.fdCommand > 0) {
                close(s_ril_param_socket2.fdCommand);
                s_ril_param_socket2.fdCommand = -1;
 }
 #endif
 break;
 case 2:
            RLOGI ("Debug port: issuing unsolicited voice network change.");
            RIL_UNSOL_RESPONSE(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0, socket_id);
 break;
 case 3:
            RLOGI ("Debug port: QXDM log enable.");
            qxdm_data[0] = 65536; // head.func_tag
            qxdm_data[1] = 16; // head.len
            qxdm_data[2] = 1; // mode: 1 for 'start logging'
            qxdm_data[3] = 32; // log_file_size: 32megabytes
            qxdm_data[4] = 0; // log_mask
            qxdm_data[5] = 8; // log_max_fileindex
            issueLocalRequest(RIL_REQUEST_OEM_HOOK_RAW, qxdm_data,
 6 * sizeof(int), socket_id);
 break;
 case 4:
            RLOGI ("Debug port: QXDM log disable.");
            qxdm_data[0] = 65536;
            qxdm_data[1] = 16;
            qxdm_data[2] = 0; // mode: 0 for 'stop logging'
            qxdm_data[3] = 32;
            qxdm_data[4] = 0;
            qxdm_data[5] = 8;
            issueLocalRequest(RIL_REQUEST_OEM_HOOK_RAW, qxdm_data,
 6 * sizeof(int), socket_id);
 break;
 case 5:
            RLOGI("Debug port: Radio On");
            data = 1;
            issueLocalRequest(RIL_REQUEST_RADIO_POWER, &data, sizeof(int), socket_id);
            sleep(2);

             issueLocalRequest(RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC, NULL, 0, socket_id);
             break;
         case 7:
             RLOGI("Debug port: Deactivate Data Call");
             issueLocalRequest(RIL_REQUEST_DEACTIVATE_DATA_CALL, &deactData,
 sizeof(deactData), socket_id);
 break;

         case 8:
             RLOGI("Debug port: Dial Call");
             dialData.clir = 0;
            if (strlen(args[1]) > MAX_DIAL_ADDRESS) {
                RLOGE("Debug port: Error calling Dial");
                freeDebugCallbackArgs(number, args);
                close(acceptFD);
                return;
            }
             dialData.address = args[1];
             issueLocalRequest(RIL_REQUEST_DIAL, &dialData, sizeof(dialData), socket_id);
             break;
 case 9:
            RLOGI("Debug port: Answer Call");
            issueLocalRequest(RIL_REQUEST_ANSWER, NULL, 0, socket_id);
 break;
 case 10:
            RLOGI("Debug port: End Call");
            issueLocalRequest(RIL_REQUEST_HANGUP, &hangupData,
 sizeof(hangupData), socket_id);
 break;
 default:
            RLOGE ("Invalid request");
 break;
 }
    freeDebugCallbackArgs(number, args);
    close(acceptFD);
}
