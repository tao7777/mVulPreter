CastStreamingNativeHandler::CastStreamingNativeHandler(ScriptContext* context)
     : ObjectBackedNativeHandler(context),
       last_transport_id_(1),
       weak_factory_(this) {
  RouteFunction("CreateSession", "cast.streaming.session",
                 base::Bind(&CastStreamingNativeHandler::CreateCastSession,
                            weak_factory_.GetWeakPtr()));
  RouteFunction("DestroyCastRtpStream", "cast.streaming.rtpStream",
                 base::Bind(&CastStreamingNativeHandler::DestroyCastRtpStream,
                            weak_factory_.GetWeakPtr()));
   RouteFunction(
      "GetSupportedParamsCastRtpStream", "cast.streaming.rtpStream",
       base::Bind(&CastStreamingNativeHandler::GetSupportedParamsCastRtpStream,
                  weak_factory_.GetWeakPtr()));
  RouteFunction("StartCastRtpStream", "cast.streaming.rtpStream",
                 base::Bind(&CastStreamingNativeHandler::StartCastRtpStream,
                            weak_factory_.GetWeakPtr()));
  RouteFunction("StopCastRtpStream", "cast.streaming.rtpStream",
                 base::Bind(&CastStreamingNativeHandler::StopCastRtpStream,
                            weak_factory_.GetWeakPtr()));
  RouteFunction("DestroyCastUdpTransport", "cast.streaming.udpTransport",
                 base::Bind(&CastStreamingNativeHandler::DestroyCastUdpTransport,
                            weak_factory_.GetWeakPtr()));
   RouteFunction(
      "SetDestinationCastUdpTransport", "cast.streaming.udpTransport",
       base::Bind(&CastStreamingNativeHandler::SetDestinationCastUdpTransport,
                  weak_factory_.GetWeakPtr()));
   RouteFunction(
      "SetOptionsCastUdpTransport", "cast.streaming.udpTransport",
       base::Bind(&CastStreamingNativeHandler::SetOptionsCastUdpTransport,
                  weak_factory_.GetWeakPtr()));
  RouteFunction("ToggleLogging", "cast.streaming.rtpStream",
                 base::Bind(&CastStreamingNativeHandler::ToggleLogging,
                            weak_factory_.GetWeakPtr()));
  RouteFunction("GetRawEvents", "cast.streaming.rtpStream",
                 base::Bind(&CastStreamingNativeHandler::GetRawEvents,
                            weak_factory_.GetWeakPtr()));
  RouteFunction("GetStats", "cast.streaming.rtpStream",
                base::Bind(&CastStreamingNativeHandler::GetStats,
                           weak_factory_.GetWeakPtr()));
  RouteFunction("StartCastRtpReceiver", "cast.streaming.receiverSession",
                 base::Bind(&CastStreamingNativeHandler::StartCastRtpReceiver,
                            weak_factory_.GetWeakPtr()));
 }
