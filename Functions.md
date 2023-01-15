## Serial Port VBA Functions - Arduino set

##### All functions support one pre-defined Com Port only

| VBA Function                   |    Returns     | Description                                                                           |
| -------------------------------|----------------|---------------------------------------------------------------------------------------|
| `start_arduino`                | `Boolean` [^1] | Starts Arduino com port with pre-configured settings (supplied)                             |
| `clear_arduino`                | `Boolean` [^1] | Clears any unread receive data or data awaiting transmission                          |
| `check_arduino`                | `Long`         | Returns number of read characters waiting. -1 indicates error                         |
| `get_arduino`                  | `String`       | Returns a single waiting character string from Arduino                                |
| `read_arduino`                 | `String`  [^3] | Returns waiting character string from Arduino                                         |
| `put_arduino("A")`             | `Boolean` [^1] | Send a single character string to Arduino                                             |
| `send_arduino("QWERTY")`       | `Boolean` [^1] | Sends [^2] supplied character string to Arduino                                       |
| `send_arduino(COMMANDS)`       | `Boolean` [^1] | Sends [^2] character string defined in VBA constant or variable COMMANDS to Arduino   |
| `send_arduino($B$5)`           | `Boolean` [^1] | Sends [^4] contents of Cell $B$5 to Arduino (Excel Worksheet Only)                   |
| `reboot_arduino`               | `Boolean` [^1] | Reboots Arduino by toggling DTR line and clears any waiting data                      |
| `stop_arduino`                 | `Boolean` [^1] | Stops com port and returns its control back to Windows                                |

##### Com Port number defined in declarations section at start of module   
`Private Const COM_PORT_NUMBER as Long = 4`    

[^1]: Function returns `True` if successful, otherwise `False`  

[^2]: Function will block until all characters are sent or write timer expires.  
      Maximum characters sent limited by timer `Write_Total_Timeout_Constant` value.   
      
[^3]: Maximum characters returned = read buffer length (fixed value)    
      More waiting characters beyond buffer length may remain unread.   
      Use `check_arduino` to confirm any remaining character count if required.   
      
[^4]:  Excel will re-send if Cell $B$5 value changes     
      
[^5]: Port settings if supplied should have the same structure as the equivalent command-line Mode arguments for a COM Port
