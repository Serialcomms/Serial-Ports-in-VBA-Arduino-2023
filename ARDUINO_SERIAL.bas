Attribute VB_Name = "ARDUINO_SERIAL"
' Attribute VB_Name = "SERIAL_PORT_ARDUINO_V1"
'
' https://github.com/Serialcomms/Serial-Ports-in-VBA-Arduino-2023
'
Option Explicit

'-----------------------------------------------------------------------------------------------------------------
Public Const COM_PORT_NUMBER As Long = 4
Public Const ARDUINO As String = "Baud=115200 To=Off Data=8 Parity=N Stop=1 DTR=Off RTS=Off Xon=Off "
'                                 ^^^^^^^^^^^ Change COM Port Settings for Arduino above ^^^^^^^^^^^
' ----------------------------------------------------------------------------------------------------------------
'
' COM Port settings format as per command-line Mode command
' https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/mode
'
Private Const LONG_0  As Long = 0
Private Const LONG_1  As Long = 1
Private Const LONG_2  As Long = 2
Private Const LONG_3  As Long = 3
Private Const LONG_4  As Long = 4

Private Const QUEUE_ERROR As Long = -1
Private Const HANDLE_INVALID As LongPtr = -1
Private Const READ_BUFFER_LENGTH As Long = 1024

Private Type DEVICE_CONTROL_BLOCK

             LENGTH_DCB As Long
             BAUD_RATE  As Long
             BIT_FIELD  As Long
             RESERVED   As Integer
             LIMIT_XON  As Integer
             LIMIT_XOFF As Integer
             BYTE_SIZE  As Byte
             PARITY     As Byte
             STOP_BITS  As Byte
             CHAR_XON   As Byte
             CHAR_XOFF  As Byte
             CHAR_ERROR As Byte
             CHAR_EOF   As Byte
             CHAR_EVENT As Byte
             RESERVED_1 As Integer
End Type

Private Type COM_PORT_STATUS

             BIT_FIELD As Long
             QUEUE_IN  As Long
             QUEUE_OUT As Long
End Type

Private Type COM_PORT_TIMEOUTS

             Read_Interval_Timeout          As Long
             Read_Total_Timeout_Multiplier  As Long
             Read_Total_Timeout_Constant    As Long
             Write_Total_Timeout_Multiplier As Long
             Write_Total_Timeout_Constant   As Long
End Type

Private Type COM_PORT_PROFILE

             Handle     As LongPtr
             Errors     As Long
             Signal     As Long
             Started    As Boolean
             Status     As COM_PORT_STATUS
             Timeouts   As COM_PORT_TIMEOUTS
             DCB        As DEVICE_CONTROL_BLOCK
End Type

Public COM_PORT As COM_PORT_PROFILE

Public Declare PtrSafe Sub Kernel_Sleep_MilliSeconds Lib "Kernel32.dll" Alias "Sleep" (ByVal Sleep_MilliSeconds As Long)
Private Declare PtrSafe Function Query_Port_DCB Lib "Kernel32.dll" Alias "GetCommState" (ByVal Port_Handle As LongPtr, ByRef Port_DCB As DEVICE_CONTROL_BLOCK) As Boolean
Private Declare PtrSafe Function Apply_Port_DCB Lib "Kernel32.dll" Alias "SetCommState" (ByVal Port_Handle As LongPtr, ByRef Port_DCB As DEVICE_CONTROL_BLOCK) As Boolean
Private Declare PtrSafe Function Build_Port_DCB Lib "Kernel32.dll" Alias "BuildCommDCBA" (ByVal Port_Config As String, ByRef Port_DCB As DEVICE_CONTROL_BLOCK) As Boolean
Private Declare PtrSafe Function Set_Com_Timers Lib "Kernel32.dll" Alias "SetCommTimeouts" (ByVal Port_Handle As LongPtr, ByRef Timeouts As COM_PORT_TIMEOUTS) As Boolean
Private Declare PtrSafe Function Set_Com_Signal Lib "Kernel32.dll" Alias "EscapeCommFunction" (ByVal Port_Handle As LongPtr, ByVal Signal_Function As Long) As Boolean
Private Declare PtrSafe Function Get_Port_Modem Lib "Kernel32.dll" Alias "GetCommModemStatus" (ByVal Port_Handle As LongPtr, ByRef Modem_Status As Long) As Boolean
Private Declare PtrSafe Function Com_Port_Purge Lib "Kernel32.dll" Alias "PurgeComm" (ByVal Port_Handle As LongPtr, ByVal Purge_Flags As Long) As Boolean
Private Declare PtrSafe Function Com_Port_Close Lib "Kernel32.dll" Alias "CloseHandle" (ByVal Port_Handle As LongPtr) As Boolean

Private Declare PtrSafe Function Com_Port_Clear Lib "Kernel32.dll" Alias "ClearCommError" _
(ByVal Port_Handle As LongPtr, ByRef Port_Error_Mask As Long, ByRef Port_Status As COM_PORT_STATUS) As Boolean

Private Declare PtrSafe Function Com_Port_Create Lib "Kernel32.dll" Alias "CreateFileA" _
(ByVal Port_Name As String, ByVal PORT_ACCESS As Long, ByVal SHARE_MODE As Long, ByVal SECURITY_ATTRIBUTES_NULL As Any, _
 ByVal CREATE_DISPOSITION As Long, ByVal FLAGS_AND_ATTRIBUTES As Long, Optional TEMPLATE_FILE_HANDLE_NULL) As LongPtr

Private Declare PtrSafe Function Synchronous_Read Lib "Kernel32.dll" Alias "ReadFile" _
(ByVal Port_Handle As LongPtr, ByVal Buffer_Data As String, ByVal Bytes_Requested As Long, ByRef Bytes_Processed As Long, Optional Overlapped_Null) As Boolean

Private Declare PtrSafe Function Synchronous_Write Lib "Kernel32.dll" Alias "WriteFile" _
(ByVal Port_Handle As LongPtr, ByVal Buffer_Data As String, ByVal Bytes_Requested As Long, ByRef Bytes_Processed As Long, Optional Overlapped_Null) As Boolean
'

Private Function OPEN_COM_PORT(Optional Port_Setttings As String) As Boolean

Dim OPEN_RESULT As Boolean

With COM_PORT

 If Not .Started Then

    If CREATE_COM_PORT Then
    
        If CONFIGURE_COM_PORT(Port_Setttings) Then
            
                    .Started = True
            
                 OPEN_RESULT = True
            
        Else
                 CLOSE_ARDUINO
        End If
                       
    End If

 End If

End With

DoEvents

OPEN_COM_PORT = OPEN_RESULT

End Function

Private Function CREATE_COM_PORT() As Boolean

Dim Device_Path As String
Dim Port_Result As Boolean

Const OPEN_EXISTING As Long = LONG_3
Const OPEN_EXCLUSIVE As Long = LONG_0
Const SYNCHRONOUS_MODE As Long = LONG_0

Const GENERIC_RW As Long = &HC0000000
Const DEVICE_PREFIX As String = "\\.\COM"
        
Device_Path = DEVICE_PREFIX & CStr(COM_PORT_NUMBER)

COM_PORT.Handle = Com_Port_Create(Device_Path, GENERIC_RW, OPEN_EXCLUSIVE, LONG_0, OPEN_EXISTING, SYNCHRONOUS_MODE)

Port_Result = Not (COM_PORT.Handle = HANDLE_INVALID)

CREATE_COM_PORT = Port_Result

End Function

Private Function CONFIGURE_COM_PORT(Optional Port_Settings As String) As Boolean

Dim Temp_Result As Boolean

If SET_PORT_CONFIG(Port_Settings) Then Temp_Result = SET_PORT_TIMERS
        
CONFIGURE_COM_PORT = Temp_Result

End Function

Private Function SET_PORT_CONFIG(Optional Port_Settings As String) As Boolean

Dim Temp_Result As Boolean

With COM_PORT

 If Query_Port_DCB(.Handle, .DCB) Then
  
    If Len(Port_Settings) > LONG_4 Then
    
        If Build_Port_DCB(Port_Settings, .DCB) Then
            
            Temp_Result = Apply_Port_DCB(.Handle, .DCB)
        
        End If
        
    Else
    
        Temp_Result = True
        
    End If

 End If

End With

SET_PORT_CONFIG = Temp_Result

End Function

Private Function SET_PORT_TIMERS() As Boolean

Dim Temp_Result As Boolean

Const NO_TIMEOUT As Long = -1
Const WRITE_CONSTANT As Long = 4000                           ' Should be less than approx 5000 to avoid VBA "Not Responding"
                                                              
With COM_PORT

    .Timeouts.Read_Interval_Timeout = NO_TIMEOUT              ' Timeouts not used for file reads.
    .Timeouts.Read_Total_Timeout_Constant = LONG_0            '
    .Timeouts.Read_Total_Timeout_Multiplier = LONG_0          '

    .Timeouts.Write_Total_Timeout_Constant = WRITE_CONSTANT   ' Maximum time in MilliSeconds allowed for each synchronous write
    .Timeouts.Write_Total_Timeout_Multiplier = LONG_0

     Temp_Result = Set_Com_Timers(.Handle, .Timeouts)

End With

SET_PORT_TIMERS = Temp_Result

End Function

Public Function OPEN_ARDUINO() As Boolean

Dim OPEN_RESULT As Boolean

If OPEN_COM_PORT(ARDUINO) Then

    Kernel_Sleep_MilliSeconds 1
    
    CLEAR_ARDUINO
    
    OPEN_RESULT = True

Else

    OPEN_RESULT = False

End If

OPEN_ARDUINO = OPEN_RESULT

End Function

Public Function CLOSE_ARDUINO() As Boolean

Dim Close_Result As Boolean

With COM_PORT

 If .Handle > LONG_0 Then
    
    .Started = False
    
     Close_Result = Com_Port_Close(.Handle)
    
    .Handle = IIf(Close_Result, LONG_0, HANDLE_INVALID)
                      
 End If

End With

DoEvents

CLOSE_ARDUINO = Close_Result

End Function

Public Function CHECK_ARDUINO() As Long

Application.Volatile  ' Excel only

Dim Read_Characters_Waiting As Long

Read_Characters_Waiting = QUEUE_ERROR

With COM_PORT
  
 If .Started Then

    If Com_Port_Clear(.Handle, .Errors, .Status) Then
    
        Read_Characters_Waiting = .Status.QUEUE_IN
             
    End If
             
 End If
             
End With
        
DoEvents

CHECK_ARDUINO = Read_Characters_Waiting

End Function

Public Function CLEAR_ARDUINO() As Boolean

' https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-purgecomm

Dim Clear_Result As Boolean
Const PURGE_ALL As Long = 15

With COM_PORT
  
 If .Started Then
 
     Clear_Result = Com_Port_Purge(.Handle, PURGE_ALL)
    
 End If
  
End With

DoEvents

CLEAR_ARDUINO = Clear_Result

End Function

Public Function PUT_ARDUINO(ByVal Send_Character As String) As Boolean

Dim Bytes_Sent As Long
Dim Send_Char As String
Dim Send_Result As Boolean

With COM_PORT
  
 If .Started And LenB(Send_Character) > LONG_0 Then
 
     Send_Char = Left$(Send_Character, LONG_1)
 
     Synchronous_Write .Handle, Send_Char, LONG_1, Bytes_Sent
    
     Send_Result = (Bytes_Sent = LONG_1)
 
 End If
  
End With

DoEvents

PUT_ARDUINO = Send_Result

End Function

Public Function GET_ARDUINO() As String

Dim Bytes_Read As Long
Dim Read_String As String
Dim Read_Buffer As String * LONG_1 ' Important - read buffer must be fixed length.

With COM_PORT
  
 If .Started Then
 
     Synchronous_Read .Handle, Read_Buffer, LONG_1, Bytes_Read
                   
     If Bytes_Read = LONG_1 Then Read_String = Left$(Read_Buffer, LONG_1)
                       
 End If
  
End With

DoEvents

GET_ARDUINO = Read_String

End Function

Public Function INPUT_ARDUINO() As String

Dim Bytes_Read As Long
Dim Read_Char As String
Dim Read_String As String
Dim Read_Buffer As String * LONG_1 ' Important - read buffer must be fixed length.

Const LINE_FEED As Byte = 10
Const ASCII_MAX As Byte = 128
Const CARRIAGE_RETURN As Byte = 13


With COM_PORT
  
 If .Started Then
 
  Do
    
     Synchronous_Read .Handle, Read_Buffer, LONG_1, Bytes_Read
                   
     If Bytes_Read = LONG_1 Then
     
        If AscB(Read_Buffer) < ASCII_MAX Then Read_String = Read_String & Read_Buffer
     
     End If
     
  Loop Until AscB(Read_Buffer) = LINE_FEED Or AscB(Read_Buffer) > ASCII_MAX Or Bytes_Read = LONG_0
  
 End If
  
End With

DoEvents

INPUT_ARDUINO = Read_String

End Function

Public Function READ_ARDUINO() As String

Dim Bytes_Read As Long
Dim Read_String As String
Dim Read_Buffer As String * READ_BUFFER_LENGTH  ' Important - read buffer must be fixed length.

With COM_PORT
  
 If .Started Then
 
     Synchronous_Read .Handle, Read_Buffer, READ_BUFFER_LENGTH, Bytes_Read
                   
     If Bytes_Read > LONG_0 Then Read_String = Left$(Read_Buffer, Bytes_Read)
                       
 End If
  
End With

DoEvents

READ_ARDUINO = Read_String

End Function

Public Function SEND_ARDUINO(ByVal Send_String As String) As Boolean

' Important - maximum characters transmitted may be limited by write constant timer

Dim Bytes_Sent As Long
Dim Send_Result As Boolean
Dim Send_String_Length As Long

With COM_PORT
  
 If .Started Then
 
     Send_String_Length = Len(Send_String)

     Synchronous_Write .Handle, Send_String, Send_String_Length, Bytes_Sent
    
     Send_Result = (Bytes_Sent = Send_String_Length)
 
 End If
  
End With

DoEvents

SEND_ARDUINO = Send_Result

End Function

Public Function REBOOT_ARDUINO() As Boolean

' https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-escapecommfunction
' set/clear BREAK, DTR, RTS port signals from list above.

Const CLEAR_DTR As Long = 5
Const RAISE_DTR As Long = 6

Dim REBOOT_RESULT As Boolean

If COM_PORT.Started Then

    Set_Com_Signal COM_PORT.Handle, CLEAR_DTR

    Kernel_Sleep_MilliSeconds 100

    Set_Com_Signal COM_PORT.Handle, RAISE_DTR
   
    Kernel_Sleep_MilliSeconds 200

    Set_Com_Signal COM_PORT.Handle, CLEAR_DTR
    
    CLEAR_ARDUINO
    
    REBOOT_RESULT = True
    
Else

    REBOOT_RESULT = False
   
End If

REBOOT_ARDUINO = REBOOT_RESULT

End Function

