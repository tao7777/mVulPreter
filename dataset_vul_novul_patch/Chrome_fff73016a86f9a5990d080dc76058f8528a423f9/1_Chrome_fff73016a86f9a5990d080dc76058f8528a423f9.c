    FromMojom(media::mojom::VideoCaptureError input,
              media::VideoCaptureError* output) {
  switch (input) {
    case media::mojom::VideoCaptureError::kNone:
      *output = media::VideoCaptureError::kNone;
      return true;
    case media::mojom::VideoCaptureError::
        kVideoCaptureControllerInvalidOrUnsupportedVideoCaptureParametersRequested:
      *output = media::VideoCaptureError::
          kVideoCaptureControllerInvalidOrUnsupportedVideoCaptureParametersRequested;
      return true;
    case media::mojom::VideoCaptureError::
        kVideoCaptureControllerIsAlreadyInErrorState:
      *output = media::VideoCaptureError::
          kVideoCaptureControllerIsAlreadyInErrorState;
      return true;
    case media::mojom::VideoCaptureError::
        kVideoCaptureManagerDeviceConnectionLost:
      *output =
          media::VideoCaptureError::kVideoCaptureManagerDeviceConnectionLost;
      return true;
    case media::mojom::VideoCaptureError::
        kFrameSinkVideoCaptureDeviceAleradyEndedOnFatalError:
      *output = media::VideoCaptureError::
          kFrameSinkVideoCaptureDeviceAleradyEndedOnFatalError;
      return true;
    case media::mojom::VideoCaptureError::
        kFrameSinkVideoCaptureDeviceEncounteredFatalError:
      *output = media::VideoCaptureError::
          kFrameSinkVideoCaptureDeviceEncounteredFatalError;
      return true;
    case media::mojom::VideoCaptureError::kV4L2FailedToOpenV4L2DeviceDriverFile:
      *output = media::VideoCaptureError::kV4L2FailedToOpenV4L2DeviceDriverFile;
      return true;
    case media::mojom::VideoCaptureError::kV4L2ThisIsNotAV4L2VideoCaptureDevice:
      *output = media::VideoCaptureError::kV4L2ThisIsNotAV4L2VideoCaptureDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kV4L2FailedToFindASupportedCameraFormat:
      *output =
          media::VideoCaptureError::kV4L2FailedToFindASupportedCameraFormat;
      return true;
    case media::mojom::VideoCaptureError::kV4L2FailedToSetVideoCaptureFormat:
      *output = media::VideoCaptureError::kV4L2FailedToSetVideoCaptureFormat;
      return true;
    case media::mojom::VideoCaptureError::kV4L2UnsupportedPixelFormat:
      *output = media::VideoCaptureError::kV4L2UnsupportedPixelFormat;
      return true;
    case media::mojom::VideoCaptureError::kV4L2FailedToSetCameraFramerate:
      *output = media::VideoCaptureError::kV4L2FailedToSetCameraFramerate;
      return true;
    case media::mojom::VideoCaptureError::kV4L2ErrorRequestingMmapBuffers:
      *output = media::VideoCaptureError::kV4L2ErrorRequestingMmapBuffers;
      return true;
    case media::mojom::VideoCaptureError::kV4L2AllocateBufferFailed:
      *output = media::VideoCaptureError::kV4L2AllocateBufferFailed;
      return true;
    case media::mojom::VideoCaptureError::kV4L2VidiocStreamonFailed:
      *output = media::VideoCaptureError::kV4L2VidiocStreamonFailed;
      return true;
    case media::mojom::VideoCaptureError::kV4L2VidiocStreamoffFailed:
      *output = media::VideoCaptureError::kV4L2VidiocStreamoffFailed;
      return true;
    case media::mojom::VideoCaptureError::kV4L2FailedToVidiocReqbufsWithCount0:
      *output = media::VideoCaptureError::kV4L2FailedToVidiocReqbufsWithCount0;
      return true;
    case media::mojom::VideoCaptureError::kV4L2PollFailed:
      *output = media::VideoCaptureError::kV4L2PollFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kV4L2MultipleContinuousTimeoutsWhileReadPolling:
      *output = media::VideoCaptureError::
          kV4L2MultipleContinuousTimeoutsWhileReadPolling;
      return true;
    case media::mojom::VideoCaptureError::kV4L2FailedToDequeueCaptureBuffer:
      *output = media::VideoCaptureError::kV4L2FailedToDequeueCaptureBuffer;
      return true;
    case media::mojom::VideoCaptureError::kV4L2FailedToEnqueueCaptureBuffer:
      *output = media::VideoCaptureError::kV4L2FailedToEnqueueCaptureBuffer;
      return true;
    case media::mojom::VideoCaptureError::
        kSingleClientVideoCaptureHostLostConnectionToDevice:
      *output = media::VideoCaptureError::
          kSingleClientVideoCaptureHostLostConnectionToDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kSingleClientVideoCaptureDeviceLaunchAborted:
      *output = media::VideoCaptureError::
          kSingleClientVideoCaptureDeviceLaunchAborted;
      return true;
    case media::mojom::VideoCaptureError::
        kDesktopCaptureDeviceWebrtcDesktopCapturerHasFailed:
      *output = media::VideoCaptureError::
          kDesktopCaptureDeviceWebrtcDesktopCapturerHasFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kFileVideoCaptureDeviceCouldNotOpenVideoFile:
      *output = media::VideoCaptureError::
          kFileVideoCaptureDeviceCouldNotOpenVideoFile;
      return true;
    case media::mojom::VideoCaptureError::
        kDeviceCaptureLinuxFailedToCreateVideoCaptureDelegate:
      *output = media::VideoCaptureError::
          kDeviceCaptureLinuxFailedToCreateVideoCaptureDelegate;
      return true;
    case media::mojom::VideoCaptureError::
        kErrorFakeDeviceIntentionallyEmittingErrorEvent:
      *output = media::VideoCaptureError::
          kErrorFakeDeviceIntentionallyEmittingErrorEvent;
      return true;
    case media::mojom::VideoCaptureError::kDeviceClientTooManyFramesDroppedY16:
      *output = media::VideoCaptureError::kDeviceClientTooManyFramesDroppedY16;
      return true;
    case media::mojom::VideoCaptureError::
        kDeviceMediaToMojoAdapterEncounteredUnsupportedBufferType:
      *output = media::VideoCaptureError::
          kDeviceMediaToMojoAdapterEncounteredUnsupportedBufferType;
      return true;
    case media::mojom::VideoCaptureError::
        kVideoCaptureManagerProcessDeviceStartQueueDeviceInfoNotFound:
      *output = media::VideoCaptureError::
          kVideoCaptureManagerProcessDeviceStartQueueDeviceInfoNotFound;
      return true;
    case media::mojom::VideoCaptureError::
        kInProcessDeviceLauncherFailedToCreateDeviceInstance:
      *output = media::VideoCaptureError::
          kInProcessDeviceLauncherFailedToCreateDeviceInstance;
      return true;
    case media::mojom::VideoCaptureError::
        kServiceDeviceLauncherLostConnectionToDeviceFactoryDuringDeviceStart:
      *output = media::VideoCaptureError::
          kServiceDeviceLauncherLostConnectionToDeviceFactoryDuringDeviceStart;
      return true;
    case media::mojom::VideoCaptureError::
        kServiceDeviceLauncherServiceRespondedWithDeviceNotFound:
      *output = media::VideoCaptureError::
          kServiceDeviceLauncherServiceRespondedWithDeviceNotFound;
      return true;
    case media::mojom::VideoCaptureError::
        kServiceDeviceLauncherConnectionLostWhileWaitingForCallback:
      *output = media::VideoCaptureError::
          kServiceDeviceLauncherConnectionLostWhileWaitingForCallback;
      return true;
    case media::mojom::VideoCaptureError::kIntentionalErrorRaisedByUnitTest:
      *output = media::VideoCaptureError::kIntentionalErrorRaisedByUnitTest;
      return true;
    case media::mojom::VideoCaptureError::kCrosHalV3FailedToStartDeviceThread:
      *output = media::VideoCaptureError::kCrosHalV3FailedToStartDeviceThread;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateMojoConnectionError:
      *output =
          media::VideoCaptureError::kCrosHalV3DeviceDelegateMojoConnectionError;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToGetCameraInfo:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToGetCameraInfo;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateMissingSensorOrientationInfo:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateMissingSensorOrientationInfo;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToOpenCameraDevice:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToOpenCameraDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToInitializeCameraDevice:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToInitializeCameraDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToConfigureStreams:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToConfigureStreams;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateWrongNumberOfStreamsConfigured:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateWrongNumberOfStreamsConfigured;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3DeviceDelegateFailedToGetDefaultRequestSettings:
      *output = media::VideoCaptureError::
          kCrosHalV3DeviceDelegateFailedToGetDefaultRequestSettings;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerHalRequestedTooManyBuffers:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerHalRequestedTooManyBuffers;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToCreateGpuMemoryBuffer:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToCreateGpuMemoryBuffer;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToMapGpuMemoryBuffer:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToMapGpuMemoryBuffer;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerUnsupportedVideoPixelFormat:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerUnsupportedVideoPixelFormat;
      return true;
    case media::mojom::VideoCaptureError::kCrosHalV3BufferManagerFailedToDupFd:
      *output = media::VideoCaptureError::kCrosHalV3BufferManagerFailedToDupFd;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToWrapGpuMemoryHandle:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToWrapGpuMemoryHandle;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToRegisterBuffer:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToRegisterBuffer;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerProcessCaptureRequestFailed:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerProcessCaptureRequestFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerInvalidPendingResultId:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerInvalidPendingResultId;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedDuplicatedPartialMetadata:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedDuplicatedPartialMetadata;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerIncorrectNumberOfOutputBuffersReceived:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerIncorrectNumberOfOutputBuffersReceived;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerInvalidTypeOfOutputBuffersReceived:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerInvalidTypeOfOutputBuffersReceived;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedMultipleResultBuffersForFrame:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedMultipleResultBuffersForFrame;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerUnknownStreamInCamera3NotifyMsg:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerUnknownStreamInCamera3NotifyMsg;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedInvalidShutterTime:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedInvalidShutterTime;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerFatalDeviceError:
      *output =
          media::VideoCaptureError::kCrosHalV3BufferManagerFatalDeviceError;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerReceivedFrameIsOutOfOrder:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerReceivedFrameIsOutOfOrder;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerFailedToUnwrapReleaseFenceFd:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerFailedToUnwrapReleaseFenceFd;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerSyncWaitOnReleaseFenceTimedOut:
      *output = media::VideoCaptureError::
          kCrosHalV3BufferManagerSyncWaitOnReleaseFenceTimedOut;
      return true;
    case media::mojom::VideoCaptureError::
        kCrosHalV3BufferManagerInvalidJpegBlob:
      *output =
          media::VideoCaptureError::kCrosHalV3BufferManagerInvalidJpegBlob;
      return true;
    case media::mojom::VideoCaptureError::kAndroidFailedToAllocate:
      *output = media::VideoCaptureError::kAndroidFailedToAllocate;
      return true;
    case media::mojom::VideoCaptureError::kAndroidFailedToStartCapture:
      *output = media::VideoCaptureError::kAndroidFailedToStartCapture;
      return true;
    case media::mojom::VideoCaptureError::kAndroidFailedToStopCapture:
      *output = media::VideoCaptureError::kAndroidFailedToStopCapture;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidApi1CameraErrorCallbackReceived:
      *output =
          media::VideoCaptureError::kAndroidApi1CameraErrorCallbackReceived;
      return true;
    case media::mojom::VideoCaptureError::kAndroidApi2CameraDeviceErrorReceived:
      *output = media::VideoCaptureError::kAndroidApi2CameraDeviceErrorReceived;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidApi2CaptureSessionConfigureFailed:
      *output =
          media::VideoCaptureError::kAndroidApi2CaptureSessionConfigureFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidApi2ImageReaderUnexpectedImageFormat:
      *output = media::VideoCaptureError::
          kAndroidApi2ImageReaderUnexpectedImageFormat;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidApi2ImageReaderSizeDidNotMatchImageSize:
      *output = media::VideoCaptureError::
          kAndroidApi2ImageReaderSizeDidNotMatchImageSize;
      return true;
    case media::mojom::VideoCaptureError::kAndroidApi2ErrorRestartingPreview:
      *output = media::VideoCaptureError::kAndroidApi2ErrorRestartingPreview;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidScreenCaptureUnsupportedFormat:
      *output =
          media::VideoCaptureError::kAndroidScreenCaptureUnsupportedFormat;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidScreenCaptureFailedToStartCaptureMachine:
      *output = media::VideoCaptureError::
          kAndroidScreenCaptureFailedToStartCaptureMachine;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidScreenCaptureTheUserDeniedScreenCapture:
      *output = media::VideoCaptureError::
          kAndroidScreenCaptureTheUserDeniedScreenCapture;
      return true;
    case media::mojom::VideoCaptureError::
        kAndroidScreenCaptureFailedToStartScreenCapture:
      *output = media::VideoCaptureError::
          kAndroidScreenCaptureFailedToStartScreenCapture;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowCantGetCaptureFormatSettings:
      *output =
          media::VideoCaptureError::kWinDirectShowCantGetCaptureFormatSettings;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToGetNumberOfCapabilities:
      *output = media::VideoCaptureError::
          kWinDirectShowFailedToGetNumberOfCapabilities;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToGetCaptureDeviceCapabilities:
      *output = media::VideoCaptureError::
          kWinDirectShowFailedToGetCaptureDeviceCapabilities;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToSetCaptureDeviceOutputFormat:
      *output = media::VideoCaptureError::
          kWinDirectShowFailedToSetCaptureDeviceOutputFormat;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToConnectTheCaptureGraph:
      *output = media::VideoCaptureError::
          kWinDirectShowFailedToConnectTheCaptureGraph;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToPauseTheCaptureDevice:
      *output =
          media::VideoCaptureError::kWinDirectShowFailedToPauseTheCaptureDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToStartTheCaptureDevice:
      *output =
          media::VideoCaptureError::kWinDirectShowFailedToStartTheCaptureDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kWinDirectShowFailedToStopTheCaptureGraph:
      *output =
          media::VideoCaptureError::kWinDirectShowFailedToStopTheCaptureGraph;
      return true;
    case media::mojom::VideoCaptureError::kWinMediaFoundationEngineIsNull:
      *output = media::VideoCaptureError::kWinMediaFoundationEngineIsNull;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationEngineGetSourceFailed:
      *output =
          media::VideoCaptureError::kWinMediaFoundationEngineGetSourceFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationFillPhotoCapabilitiesFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationFillPhotoCapabilitiesFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationFillVideoCapabilitiesFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationFillVideoCapabilitiesFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationNoVideoCapabilityFound:
      *output =
          media::VideoCaptureError::kWinMediaFoundationNoVideoCapabilityFound;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationGetAvailableDeviceMediaTypeFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationGetAvailableDeviceMediaTypeFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationSetCurrentDeviceMediaTypeFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationSetCurrentDeviceMediaTypeFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationEngineGetSinkFailed:
      *output =
          media::VideoCaptureError::kWinMediaFoundationEngineGetSinkFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationSinkQueryCapturePreviewInterfaceFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationSinkQueryCapturePreviewInterfaceFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationSinkRemoveAllStreamsFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationSinkRemoveAllStreamsFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationCreateSinkVideoMediaTypeFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationCreateSinkVideoMediaTypeFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationConvertToVideoSinkMediaTypeFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationConvertToVideoSinkMediaTypeFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationSinkAddStreamFailed:
      *output =
          media::VideoCaptureError::kWinMediaFoundationSinkAddStreamFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationSinkSetSampleCallbackFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationSinkSetSampleCallbackFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationEngineStartPreviewFailed:
      *output =
          media::VideoCaptureError::kWinMediaFoundationEngineStartPreviewFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kWinMediaFoundationGetMediaEventStatusFailed:
      *output = media::VideoCaptureError::
          kWinMediaFoundationGetMediaEventStatusFailed;
      return true;
    case media::mojom::VideoCaptureError::kMacSetCaptureDeviceFailed:
      *output = media::VideoCaptureError::kMacSetCaptureDeviceFailed;
      return true;
    case media::mojom::VideoCaptureError::kMacCouldNotStartCaptureDevice:
      *output = media::VideoCaptureError::kMacCouldNotStartCaptureDevice;
      return true;
    case media::mojom::VideoCaptureError::
        kMacReceivedFrameWithUnexpectedResolution:
      *output =
          media::VideoCaptureError::kMacReceivedFrameWithUnexpectedResolution;
      return true;
    case media::mojom::VideoCaptureError::kMacUpdateCaptureResolutionFailed:
      *output = media::VideoCaptureError::kMacUpdateCaptureResolutionFailed;
      return true;
    case media::mojom::VideoCaptureError::
        kMacDeckLinkDeviceIdNotFoundInTheSystem:
      *output =
          media::VideoCaptureError::kMacDeckLinkDeviceIdNotFoundInTheSystem;
      return true;
    case media::mojom::VideoCaptureError::
        kMacDeckLinkErrorQueryingInputInterface:
      *output =
          media::VideoCaptureError::kMacDeckLinkErrorQueryingInputInterface;
      return true;
    case media::mojom::VideoCaptureError::
        kMacDeckLinkErrorCreatingDisplayModeIterator:
      *output = media::VideoCaptureError::
          kMacDeckLinkErrorCreatingDisplayModeIterator;
      return true;
    case media::mojom::VideoCaptureError::kMacDeckLinkCouldNotFindADisplayMode:
      *output = media::VideoCaptureError::kMacDeckLinkCouldNotFindADisplayMode;
      return true;
    case media::mojom::VideoCaptureError::
        kMacDeckLinkCouldNotSelectTheVideoFormatWeLike:
      *output = media::VideoCaptureError::
          kMacDeckLinkCouldNotSelectTheVideoFormatWeLike;
      return true;
    case media::mojom::VideoCaptureError::kMacDeckLinkCouldNotStartCapturing:
      *output = media::VideoCaptureError::kMacDeckLinkCouldNotStartCapturing;
      return true;
    case media::mojom::VideoCaptureError::kMacDeckLinkUnsupportedPixelFormat:
      *output = media::VideoCaptureError::kMacDeckLinkUnsupportedPixelFormat;
      return true;
    case media::mojom::VideoCaptureError::
        kMacAvFoundationReceivedAVCaptureSessionRuntimeErrorNotification:
      *output = media::VideoCaptureError::
          kMacAvFoundationReceivedAVCaptureSessionRuntimeErrorNotification;
      return true;
     case media::mojom::VideoCaptureError::kAndroidApi2ErrorConfiguringCamera:
       *output = media::VideoCaptureError::kAndroidApi2ErrorConfiguringCamera;
       return true;
    case media::mojom::VideoCaptureError::kCrosHalV3DeviceDelegateFailedToFlush:
      *output = media::VideoCaptureError::kCrosHalV3DeviceDelegateFailedToFlush;
      return true;
   }
   NOTREACHED();
   return false;
}
