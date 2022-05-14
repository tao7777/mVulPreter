ScriptPromise BluetoothRemoteGATTServer::getPrimaryServicesImpl(
    ScriptState* scriptState,
    mojom::blink::WebBluetoothGATTQueryQuantity quantity,
    String servicesUUID) {
  if (!connected()) {
    return ScriptPromise::rejectWithDOMException(
        scriptState,
        DOMException::create(NetworkError, kGATTServerNotConnected));
  }

  ScriptPromiseResolver* resolver = ScriptPromiseResolver::create(scriptState);
  ScriptPromise promise = resolver->promise();
   AddToActiveAlgorithms(resolver);
 
   mojom::blink::WebBluetoothService* service = m_device->bluetooth()->service();
   service->RemoteServerGetPrimaryServices(
      device()->id(), quantity, servicesUUID,
       convertToBaseCallback(
           WTF::bind(&BluetoothRemoteGATTServer::GetPrimaryServicesCallback,
                     wrapPersistent(this), quantity, wrapPersistent(resolver))));
  return promise;
}
