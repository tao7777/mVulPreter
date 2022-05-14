void MockWebRTCPeerConnectionHandler::createOffer(const WebRTCSessionDescriptionRequest& request, const WebMediaConstraints& constraints)
{
    WebString shouldSucceed;
     if (constraints.getMandatoryConstraintValue("succeed", shouldSucceed) && shouldSucceed == "true") {
         WebRTCSessionDescriptionDescriptor sessionDescription;
         sessionDescription.initialize("offer", "Some SDP here");
        postTask(new RTCSessionDescriptionRequestSuccededTask(this, request, sessionDescription));
    } else
        postTask(new RTCSessionDescriptionRequestFailedTask(this, request));
}
