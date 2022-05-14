static void rng_egd_request_entropy(RngBackend *b, size_t size,
static void rng_egd_request_entropy(RngBackend *b, RngRequest *req)
 {
     RngEgd *s = RNG_EGD(b);
    size_t size = req->size;
 
     while (size > 0) {
         uint8_t header[2];
    req = g_malloc(sizeof(*req));

    req->offset = 0;
    req->size = size;
    req->receive_entropy = receive_entropy;
    req->opaque = opaque;
    req->data = g_malloc(req->size);
 
         size -= len;
     }
 }
