# Selective_Repeat
Selective Repeat Assignment in Computer Network class 2023 

## 📡 Selective Repeat


### 📘 Overview


---

### 📤 1. Output
- Compare sequence state with window boundary:
  - A.next_seqnum
  - A.send_base + window_size
- Determine action:
  - InWindow: build and send packet
  - OutOfWindow: skip and print "Out of Window"
- If InWindow:
  - Set seqnum and acknum
  - Copy payload into packet
  - Compute checksum
- Start transmission:
  - Send packet to layer3
  - Start timer for this seqnum
  - Mark timer active
  - Increment A.next_seqnum

---

### 📥 2. Input


---

### 📨 3. Timer Interrupt


---

## 📦 Structure
- `Output`
<img width="2206" height="3105" alt="SR_output" src="https://github.com/user-attachments/assets/6ba935e5-82ea-432c-9813-a3b1e740033c" />

- `Input`

- `Timer Interrupt`

## 📊 Results
<img width="3881" height="2261" alt="SR_result1" src="https://github.com/user-attachments/assets/51ff992e-17cc-420c-8dd4-b6596b87178a" />
<img width="4307" height="2197" alt="SR_result2" src="https://github.com/user-attachments/assets/6badd1e1-5d49-4b82-bc36-c110b4dc7b98" />
