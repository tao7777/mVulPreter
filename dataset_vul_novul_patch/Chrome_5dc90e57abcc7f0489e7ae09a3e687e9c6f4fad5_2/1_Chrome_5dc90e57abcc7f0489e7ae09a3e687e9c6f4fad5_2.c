WebSocketJob::WebSocketJob(SocketStream::Delegate* delegate)
    : delegate_(delegate),
      state_(INITIALIZED),
      waiting_(false),
      callback_(NULL),
      handshake_request_(new WebSocketHandshakeRequestHandler),
      handshake_response_(new WebSocketHandshakeResponseHandler),
      started_to_send_handshake_request_(false),
       handshake_request_sent_(0),
       response_cookies_save_index_(0),
       send_frame_handler_(new WebSocketFrameHandler),
      receive_frame_handler_(new WebSocketFrameHandler) {
 }
