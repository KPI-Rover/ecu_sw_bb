# **TCP/UART Communication Protocol Specification**

## **1. Overview**  
This protocol is designed for **communication between a ROS2 system running on platforms such as RPI and a chassis controller operating on hardware like BeagleBone Blue or STM32****. It supports **both TCP and UART**, allowing for a seamless future transition between the two communication methods.

---

# **2. Protocol Architecture**  
The protocol consists of **two layers**:  
1. **Frame Layer (Layer 1)** – Ensures data integrity, required only for UART.  
2. **Protocol Layer (Layer 2)** – Defines the core communication commands.  

✅ **For TCP:** Only **Layer 2** is used, since TCP already provides reliability.  
✅ **For UART:** **Both Layer 1 and Layer 2** are used to handle message framing and integrity.

---

# **3. Frame Layer (Layer 1) – UART Only**
**(Not used in TCP mode)**  
The frame layer is **only applicable when using UART**, as UART does not guarantee message integrity or completeness.

### **Frame Format (UART Only)**
| Offset | Size (bytes) | Field         | Description |
|--------|-------------|---------------|-------------|
| 0      | 1           | `frame_length` | Total packet length (including CRC16). Ensures packet boundary detection over UART |
| 1      | N           | `payload`      | Protocol Layer (Layer 2) data |
| N+1    | 2           | `crc16`        | CRC16 checksum. Provides error detection for unreliable UART communication |

  

**For TCP:**  
- Layer 1 **is skipped**, as TCP already ensures **packet boundaries** and **error handling**.

---

# **4. Protocol Layer (Layer 2) – Used in TCP & UART**
This is the **core protocol** used in both **TCP and UART** modes. It contains **commands for motor control, encoder data retrieval, and more**.

### **Protocol Format**
| Offset | Size (bytes) | Field         | Description |
|--------|-------------|---------------|-------------|
| 0      | 1           | `command_id`   | Unique command identifier |
| 1      | 1           | `payload_size` | Number of bytes in the payload |
| 2      | N           | `payload`      | Command-specific data |

---

# **5. Command Set**
| Command ID | Command Name       | Description          |
|------------|-------------------|----------------------|
| `0x01`     | [`get_api_version`](#51-get_api_version-0x01) | Retrieves the API version of the system |
| `0x02`     | [`set_motor_speed`](#52-set_motor_speed-0x02) | Sets the speed of a motor |
| `0x03`     | [`set_all_motors_speed`](#53-set_all_motors_speed-0x03) | Sets the speed for all four motors in a single command |

---

## **5.1 get_api_version (0x01)**
Retrieves the firmware/API version.

### Request
| Offset | Size (bytes) | Field Description | Values |
|--------|-------------|------------------|--------|
| 0        | 1           | `command_id` | `0x01` |
| 1        | 1           | ROS2 Driver Version | 1-255 |

### Response
| Offset | Size (bytes) | Field Description | Values |
|--------|-------------|------------------|--------|
| 0        | 1           | `command_id` | `0x01` |
| 1        | 1           | API Version | **1-255** |

---

## **5.2 set_motor_speed (0x02)**
Sets the speed of a specific motor.

### Request
| Offset | Size (bytes) | Field Description | Values |
|--------|-------------|------------------|--------|
| 0        | 1           | `command_id` | `0x02` |
| 1        | 1           | `motor_id` | Motor ID (0-3) |
| 2        | 4           | `speed` | Speed in RPM multiplied by 100 |

### Response
| Offset | Size (bytes) | Field Description | Values |
|--------|-------------|------------------|--------|
| 0        | 1           | `command_id` | `0x02` |
| 1        | 1           | Status Code | `0` for OK, non-zero for error (error codes defined later) |

---

## **5.3 set_all_motors_speed (0x03)**
Sets the speed for all four motors in a single command.

### Request
| Offset | Size (bytes) | Field Description | Values |
|--------|-------------|------------------|--------|
| 0        | 1           | `command_id` | `0x03` |
| 1        | 4           | `speed_motor_0` | Speed in RPM multiplied by 100 |
| 5        | 4           | `speed_motor_1` | Speed in RPM multiplied by 100 |
| 9        | 4           | `speed_motor_2` | Speed in RPM multiplied by 100 |
| 13       | 4           | `speed_motor_3` | Speed in RPM multiplied by 100 |

### Response
| Offset | Size (bytes) | Field Description | Values |
|--------|-------------|------------------|--------|
| 0        | 1           | `command_id` | `0x03` |
| 1        | 1           | Status Code | `0` for OK, non-zero for error (error codes defined later) |



---


