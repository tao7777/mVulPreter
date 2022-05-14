 native_handle_t* native_handle_create(int numFds, int numInts)
 {
    native_handle_t* h = malloc(
            sizeof(native_handle_t) + sizeof(int)*(numFds+numInts));
 
     if (h) {
         h->version = sizeof(native_handle_t);
         h->numFds = numFds;
        h->numInts = numInts;
 }
 return h;
}
