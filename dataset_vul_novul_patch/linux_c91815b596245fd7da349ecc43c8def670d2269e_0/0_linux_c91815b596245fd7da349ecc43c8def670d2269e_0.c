void dwc3_gadget_giveback(struct dwc3_ep *dep, struct dwc3_request *req,
void dwc3_gadget_del_and_unmap_request(struct dwc3_ep *dep,
		struct dwc3_request *req, int status)
 {
 	struct dwc3			*dwc = dep->dwc;
 
	req->started = false;
	list_del(&req->list);
	req->remaining = 0;

	if (req->request.status == -EINPROGRESS)
		req->request.status = status;
 
 	if (req->trb)
 		usb_gadget_unmap_request_by_dev(dwc->sysdev,
				&req->request, req->direction);
 
 	req->trb = NULL;
 	trace_dwc3_gadget_giveback(req);
 
	if (dep->number > 1)
		pm_runtime_put(dwc->dev);
}

/**
 * dwc3_gadget_giveback - call struct usb_request's ->complete callback
 * @dep: The endpoint to whom the request belongs to
 * @req: The request we're giving back
 * @status: completion code for the request
 *
 * Must be called with controller's lock held and interrupts disabled. This
 * function will unmap @req and call its ->complete() callback to notify upper
 * layers that it has completed.
 */
void dwc3_gadget_giveback(struct dwc3_ep *dep, struct dwc3_request *req,
		int status)
{
	struct dwc3			*dwc = dep->dwc;

	dwc3_gadget_del_and_unmap_request(dep, req, status);

 	spin_unlock(&dwc->lock);
 	usb_gadget_giveback_request(&dep->endpoint, &req->request);
 	spin_lock(&dwc->lock);
 }
