void MockWebRTCPeerConnectionHandler::createOffer(const WebRTCSessionDescriptionRequest& request, const WebMediaConstraints& constraints)
{
    WebString shouldSucceed;
     if (constraints.getMandatoryConstraintValue("succeed", shouldSucceed) && shouldSucceed == "true") {
         WebRTCSessionDescriptionDescriptor sessionDescription;
         sessionDescription.initialize("offer", "Some SDP here");
