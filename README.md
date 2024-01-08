# Serial-Ports-in-VBA-Arduino-2023
Arduino Uno R3 serial version
# Serial Port functions in VBA Simple 2022

This is the VBA Serial Port function set adapted for use with Arduino devices.

No other files, licences, payments or registrations are required.  

Functions can be used directly in Excel cells as Formulas where appropriate. 

No Arduino reboot on com port connection by design.

Separate Arduino reboot command if required. 


<details>

<summary>Com Port</summary>

<p>
  
- Requires that the Arduino COM Port Driver is correctly installed and configured.
- Correct COM Port Number is defined at start of VBA module.


</p>

</details>

<details>

<summary>Read Functions</summary>

<p>
  
_Assume that all data has already been sent by the Arduino is ready waiting to be read_

- `check_arduino` can be used to confirm expected number of characters are waiting before committing read 

- No pre or post read delays for any in-flight data reception to complete are provided.
  
- Data will be read in one synchronous API call.
  
- Maximum characters per read call = `READ_BUFFER_LENGTH`
  
- `check_com_port` function can be used again to check for any new or remaining characters. 
    
</p>

</details>

<details>
  
<summary>Write Functions</summary>
 
<p>

Writes are synchronous and functions can block until outgoing data is processed or write timer expires 
    
- Short strings will return quickly as data is buffered for transmission    
- Maximum number of characters sent is limited by write timer value in milliseconds
  
</p>

</details>

<details>
  
<summary>Control Functions</summary>

<p>

### Com Port Start, Stop ###
  
- Allow a few MilliSeconds for functions to return and for any attached hardware to stabilise   
- Functions return `True` or `False` to indicate success or failure  
  
### Data Waiting Check ###
  
- Function returns number of characters waiting to be read   
- Return number can be zero if no data waiting  
- Return value of -1 indicates error, including port not started 
     
</p>  
  
</details>

<details><summary>Optional Customisations</summary>
<p> 

* [COM Port Enumeration](https://github.com/Serialcomms/COM-Port-Enumeration-VBA)
* [COM Port Configuration](https://github.com/Serialcomms/COM-Port-Configuration-VBA)

</p>
</details>

<details><summary>Function List Table</summary>
<p> 

* [Function List Table](Functions.md)

</p>
</details>  


