# Selective_Repeat
Selective Repeat Assignment in Computer Network class 2023 

## 📡 Selective Repeat


### 📘 Overview


---

### 📤 1. Output
- **Compare sequence state with window boundary:**
  - A.next_seqnum
  - A.send_base + window_size
- **Determine action:**
  - InWindow: build and send packet
  - OutOfWindow: skip and print "Out of Window"
- **If InWindow:**
  - Set seqnum and acknum
  - Copy payload into packet
  - Compute checksum
- **Start transmission:**
  - Send packet to layer3
  - Start timer for this seqnum
  - Mark timer active
  - Increment A.next_seqnum

---

### 📥 2. Input
- **Checksum Verification**
  - Compare the checksum in the receive packet with the calculated checksum
  - Equal: packet is not corrupted
  - Not Equal: packet is corrupted
- **Deliver Payload**
  - Packet is Valid: send the payload to layer 5 using tolayer5
- **Ack handling**
  - Set ack_num - packet.seqnum: the received sequence number becomes the ACK.
  - ACK is not a duplicate:
    - Stop the timer for that ACK
    - Reset the timer array
    - Update the base array for sliding window management
- **Sliding Window Update**
  - send_base == packet.acknum:
    - Loop for window size:
      - Increment send_base
      - If the sequence is broken, stop the loop
    - This moves the send base for all consecutive ACKed packets
- **Corrupted Packet Handling**
  - If checksums do not match, ignore the packet
  - The timer will handle timeout later
    
---

### 📨 3. Timer Interrupt
- **Retransmission on Timeout**
  - Send the packet again to B using tolayer3
- **Restart Timer**
  - Restart the timer for the retransmitted packet with starttimer
- **Logging**
  - Print information about the retransmitted packet: ACK number, seq number and payload

---

## 📦 Structure
- `Output`
<img width="2206" height="3105" alt="SR_output" src="https://github.com/user-attachments/assets/6ba935e5-82ea-432c-9813-a3b1e740033c" />

- `Input`
<img width="2111" height="4215" alt="SR_input" src="https://github.com/user-attachments/assets/d209192e-eb72-47eb-bb2f-6f5331149383" />

- `Timer Interrupt`
<img width="477" height="944" alt="SR_interrupt" src="https://github.com/user-attachments/assets/33b9fa4a-7807-4162-90f0-3ca2f26536d0" />



## 📊 Results
<img width="3881" height="2261" alt="SR_result1" src="https://github.com/user-attachments/assets/51ff992e-17cc-420c-8dd4-b6596b87178a" />
<img width="4307" height="2197" alt="SR_result2" src="https://github.com/user-attachments/assets/6badd1e1-5d49-4b82-bc36-c110b4dc7b98" />
