Public Class Converter
  'int gsapi_revision (gsapi_revision_t *pr, int len);
  Private Declare Function gsapi_revision Lib "gsdll32.dll" (pr As IntPtr, len As Integer) As Integer '0 = OK

  'int gsapi_new_instance (void **pinstance, void *caller_handle);
  Private Declare Function gsapi_new_instance Lib "gsdll32.dll" (ByRef pinstance As IntPtr, caller_handle As IntPtr) As Integer '0 = OK

  'void gsapi_delete_instance (void *instance);
  Private Declare Function gsapi_delete_instance Lib "gsdll32.dll" (pinstance As IntPtr) As Integer '0 = OK

  'int gsapi_set_stdio (void *instance, int(*stdin_fn)(void *caller_handle, char *buf, int len), int(*stdout_fn)(void *caller_handle, const char *str, int len), int(*stderr_fn)(void *caller_handle, const char *str, int len));
  'int gsapi_set_poll (void *instance, int(*poll_fn)(void *caller_handle));
  'int gsapi_set_display_callback (void *instance, display_callback *callback);

  'int gsapi_init_with_args (void *instance, int argc, char **argv);
  Private Declare Function gsapi_init_with_args Lib "gsdll32.dll" (pinstance As IntPtr, argc As Integer, argv As String()) As Integer '0 = OK

  'int gsapi_run_string_begin (void *instance, int user_errors, int *pexit_code);
  'int gsapi_run_string_continue (void *instance, const char *str, unsigned int length, int user_errors, int *pexit_code);
  'int gsapi_run_string_end (void *instance, int user_errors, int *pexit_code);
  'int gsapi_run_string_with_length (void *instance, const char *str, unsigned int length, int user_errors, int *pexit_code);
  'int gsapi_run_string (void *instance, const char *str, int user_errors, int *pexit_code);
  'int gsapi_run_file (void *instance, const char *file_name, int user_errors, int *pexit_code);
  'int gsapi_exit (void *instance);
  Private Declare Function gsapi_exit Lib "gsdll32.dll" (pinstance As IntPtr) As Integer '0 = OK
  'int gsapi_set_visual_tracer (gstruct vd_trace_interface_s *I);

  Private Shared _instance As IntPtr = IntPtr.Zero

  Private Shared _standardArgs As String() = {
    "",
    "-q",
    "-dQUIET",
    "-dPARANOIDSAFER",
    "-dBATCH",
    "-dNOPAUSE",
    "-dNOPROMPT",
    "-dMaxBitmap=500000000",
    "-dNumRenderingThreads=4",
    "-dAlignToPixels=0",
    "-dGridFitTT=0",
    "-dTextAlphaBits=4",
    "-dGraphicsAlphaBits=4"
  }

  Public Enum GhostScriptDeviceEnum
    UNDEFINED
    png16m
    pnggray
    png256
    png16
    pngmono
    pngalpha
    jpeg
    jpeggray
    tiffgray
    tiff12nc
    tiff24nc
    tiff32nc
    tiffsep
    tiffcrle
    tiffg3
    tiffg32d
    tiffg4
    tifflzw
    tiffpack
    faxg3
    faxg32d
    faxg4
    bmpmono
    bmpgray
    bmpsep1
    bmpsep8
    bmp16
    bmp256
    bmp16m
    bmp32b
    pcxmono
    pcxgray
    pcx16
    pcx256
    pcx24b
    pcxcmyk
    psdcmyk
    psdrgb
    pdfwrite
    pswrite
    epswrite
    pxlmono
    pxlcolor
  End Enum

  Public Enum GhostscriptPageSizeEnum
    UNDEFINED
    ledger
    legal
    letter
    lettersmall
    archE
    archD
    archC
    archB
    archA
    a0
    a1
    a2
    a3
    a4
    a4small
    a5
    a6
    a7
    a8
    a9
    a10
    isob0
    isob1
    isob2
    isob3
    isob4
    isob5
    isob6
    c0
    c1
    c2
    c3
    c4
    c5
    c6
    jisb0
    jisb1
    jisb2
    jisb3
    jisb4
    jisb5
    jisb6
    b0
    b1
    b2
    b3
    b4
    b5
    flsa
    flse
    halfletter
  End Enum

  Private Shared Function HandleCall(returnCode As Integer, message As String) As Integer
    Debug.WriteLine(message + " - " + CStr(returnCode))

    If returnCode <> 0 Then
      Throw New ApplicationException("Ghostscript: " + message + " (" + CStr(returnCode) + ")")
    End If
    Return returnCode
  End Function

  Public Shared Sub Convert(inputFile As String, outputFile As String, device As GhostScriptDeviceEnum, pageNumber As Integer)
    If _instance = IntPtr.Zero Then
      HandleCall(gsapi_new_instance(_instance, IntPtr.Zero), "Failed to create instance")
    End If

    'Try
    Dim arguments As New List(Of String)(_standardArgs)

    arguments.Add("-sDEVICE=" + device.ToString())
    arguments.Add("-sOutputFile=" + outputFile)
    arguments.Add("-sPAPERSIZE=a4")
    arguments.Add("-dFirstPage=" + CStr(pageNumber))
    arguments.Add("-dLastPage=" + CStr(pageNumber))
    arguments.Add("-dDEVICEXRESOLUTION=200")
    arguments.Add("-dDEVICEYRESOLUTION=200")
    arguments.Add(inputFile)

    HandleCall(gsapi_init_with_args(_instance, arguments.Count, arguments.ToArray()), "Error during conversion")
    HandleCall(gsapi_exit(_instance), "Failed to clean up")
    'Finally
    '  Try
    '    gsapi_delete_instance(instance)
    '  Catch
    '  End Try
    'End Try
  End Sub
End Class
