static void rng_egd_request_entropy(RngBackend *b, size_t size,
                                    EntropyReceiveFunc *receive_entropy,
                                    void *opaque)
 {
     RngEgd *s = RNG_EGD(b);
    RngRequest *req;
    req = g_malloc(sizeof(*req));
    req->offset = 0;
    req->size = size;
    req->receive_entropy = receive_entropy;
    req->opaque = opaque;
    req->data = g_malloc(req->size);
 
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
    s->parent.requests = g_slist_append(s->parent.requests, req);
 }
