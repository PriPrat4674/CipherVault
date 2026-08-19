<div align="center">

```
  ____ _       _               __     __ault
 / ___(_)_ __ | |__   ___ _ __ \ \   / /_ _ _   _| | |_ 
| |   | | '_ \| '_ \ / _ \ '__| \ \ / / _` | | | | | __|
| |___| | |_) | | | |  __/ |     \ V / (_| | |_| | | |_ 
 \____|_| .__/|_| |_|\___|_|      \_/ \__,_|\__,_|_|\__|
        |_|                                             
```

# 🔐 CipherVault
### *The Zero-Dependency, Offline Encrypted Personal Data Manager in Pure C*

[![Language](https://img.shields.io/badge/Language-Pure%20C%20(C99%2FC11)-00599C?style=for-the-badge&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Compiler](https://img.shields.io/badge/Compiler-GCC%20%7C%20Clang%20%7C%20MSVC-brightgreen?style=for-the-badge&logo=gnu)](https://gcc.gnu.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-0078D4?style=for-the-badge&logo=windows)](https://github.com)
[![Dependencies](https://img.shields.io/badge/Dependencies-Zero%20(Standard%20Lib%20Only)-orange?style=for-the-badge)](https://github.com)
[![Security](https://img.shields.io/badge/Storage-Encrypted%20Binary%20Streams-red?style=for-the-badge&logo=shield)](https://github.com)
[![License](https://img.shields.io/badge/License-MIT-blueviolet?style=for-the-badge)](LICENSE)

<p align="center">
  <b>Stop saving passwords in unencrypted notes and spreadsheets.</b><br>
  CipherVault provides ultra-fast, local-first, cryptographically protected storage for credentials, sensitive notes, and contacts directly from your command line.
</p>

[✨ Highlights](#-highlights) • [⚡ Quickstart](#-quickstart--how-to-make-it-work) • [🎮 Interactive Tour](#-interactive-feature-tour) • [📖 Command Catalog](#-command-cheat-sheet) • [🔒 Cryptography](#-cryptographic-architecture) • [📁 Codebase Map](#-project-structure)

</div>

---

## 💡 Why CipherVault?

In an era of recurring cloud data breaches and bloated electron applications, **CipherVault goes back to first principles:**

- 🛡️ **Zero Plaintext Footprint**: Plaintext never touches your storage drive. Sensitive fields are encrypted *in memory* before being committed to binary disk streams.
- 📴 **100% Offline & Private**: No cloud sync, no tracking, no telemetry, no network calls. Your data lives exclusively on your local machine.
- 🚀 **Blazing Fast & Zero Dependencies**: Written in pure, idiomatic ANSI C. Compiles into a lightweight standalone executable with standard runtime libraries (`< 500 KB`).
- 🕶️ **Shoulder-Surfing Defense**: Real-time terminal masking with asterisks (`*`) for all password entries with interactive Backspace support.
- ⚡ **$O(1)$ In-Place Binary I/O**: Direct-seek byte offset updates and soft deletions that eliminate expensive full-file rewrites.

---

## ✨ Highlights

| Feature | Description |
|---|---|
| **🔑 Dynamic Key Derivation** | Encryption keys are derived mathematically from your master password hash (`(sum % 25) + 1`). |
| **🚨 3-Strike Lockout Gate** | Brute-force protection terminates the session immediately upon 3 consecutive failed attempts. |
| **📁 Heterogeneous Vault** | Native support for **Passwords** (username/password/URL), **Secure Notes** (body/tags), and **Contacts** (phone/email). |
| **🔍 In-Memory Search** | Custom case-insensitive substring search across titles, categories, or all content fields. |
| **📊 Multi-Criteria Sorting** | In-memory Bubble Sort by Record ID, Title (A–Z), Category Type, or Creation Date. |
| **🧹 Two-Phase Deletion** | Instant soft-delete (`is_deleted = 1`) paired with physical compaction (`purge`) that reclaims disk space. |
| **🔄 Atomic Key Migration** | Changing the master password decrypts and re-encrypts the entire vault in a single atomic transaction. |
| **📄 Decrypted Report Export** | Generates an auditable plaintext backup (`vault_export.txt`) on demand. |

---

## ⚡ Quickstart — How to Make It Work

### 📋 Prerequisites
All you need is a standard C compiler (**GCC**, **Clang**, or **MinGW / Dev-C++**).

```bash
# Verify your GCC installation
gcc --version
```

---

### 🔨 1. Clone & Build

```bash
# 1. Clone repository
git clone https://github.com/YourUsername/CipherVault.git
cd CipherVault

# 2. Compile the full project (Single Command)
gcc main.c auth.c crypto.c display.c fileio.c vault_ops.c search.c -o ciphervault -Wall

# 3. Launch CipherVault
./ciphervault
```

> **For Windows (Dev-C++ / Visual Studio):**  
> Create a new C Console project, add all `.c` and `.h` files into the project tree, and click **Build & Run** (`F11`).

---

### 🧪 2. Run the Isolated Unit Test Suite

Member B modules (`vault_ops.c` and `search.c`) can be tested independently with mock stubs without compiling `main.c`:

```bash
gcc test_b.c vault_ops.c search.c -o test_b -Wall
./test_b
```

---

## 🎮 Interactive Feature Tour

### 1. First-Run Setup & Masked Authentication
When launched for the first time, CipherVault automatically detects a fresh environment, prompts you to establish a Master Password with masked input, hashes it using `0xDEAD`, and initializes your binary database.

```text
+==================================================+
|                   CIPHERVAULT                    |
|        Secure Personal Data Manager in C         |
|     Type 'help' to see the menu after login      |
+==================================================+

First run detected. Set your master password: **********
Vault initialized successfully.
Enter master password: **********
Access granted. Welcome back.
Last login: Never

vault> 
```

---

### 2. Multi-Category Record Creation (`add`)
The `add` wizard dynamically tailors its input prompts based on the chosen category:

```text
vault> add
Select record type:
1. Password
2. Secure Note
3. Contact
Choice: 1
Title: GitHub Master Key
Username: octocat@github.com
Password: **************
URL (optional): https://github.com/login
Record saved. ID: 1
```

---

### 3. Decrypted Tabular View (`view`)
Loads records into memory, decrypts active slots, sanitizes non-printable characters via `isprint()`, and renders an aligned layout:

```text
vault> view
ID   | Type       | Title                           | Preview
---- | ---------- | --------------------------------| --------------------
1    | Password   | GitHub Master Key               | octocat@github.com
2    | Note       | Server Infrastructure Config    | SSH Keys & Port 2222
3    | Contact    | Linus Torvalds                  | Linus Torvalds
----+------------+--------------------------------+--------------------
```

---

### 4. Smart Substring Search (`search`)
Search across titles, categories, or all fields simultaneously with **case-insensitive matching**:

```text
vault> search
Search by:
1. Title keyword
2. Record type (Password / Note / Contact)
3. Keyword in any field
Choice: 1
Enter keyword: github
1 record(s) found:
ID   | Type       | Title                           | Preview
---- | ---------- | --------------------------------| --------------------
1    | Password   | GitHub Master Key               | octocat@github.com
----+------------+--------------------------------+--------------------
```

---

### 5. Multi-Criteria In-Memory Sorting (`sort`)
Sort your records on the fly without modifying their physical order in the binary file:

```text
vault> sort
Sort by:
1. ID (default order)
2. Title (A-Z)
3. Type
4. Date Added
Choice: 2
ID   | Type       | Title                           | Preview
---- | ---------- | --------------------------------| --------------------
1    | Password   | GitHub Master Key               | octocat@github.com
3    | Contact    | Linus Torvalds                  | Linus Torvalds
2    | Note       | Server Infrastructure Config    | SSH Keys & Port 2222
----+------------+--------------------------------+--------------------
Showing Title sorted records.
```

---

### 6. In-Place Record Mutation (`update <id>`)
Directly overwrites a single record slot via `fseek` byte offsets. **Pressing ENTER with no input preserves the existing value:**

```text
vault> update 1
Current values for ID 1:
Title  : GitHub Master Key
Field1 : octocat@github.com
Field2 : MyOldPassword123
Field3 : https://github.com/login
Enter new values (press ENTER to keep current):
Title: 
Field1: 
Field2: **************
Field3: 
Record updated.
```

---

### 7. Two-Phase Deletion & Storage Compaction (`delete` & `purge`)
- `delete 2` sets `is_deleted = 1` in-place. The record is instantly hidden from view.
- `purge` physically removes all soft-deleted records, compacts `vault.dat`, and re-indexes IDs:

```text
vault> delete 2
Record 2 marked as deleted.

vault> purge
WARNING: This will permanently remove all deleted records. Continue? (y/n): y
Purge complete. 1 records permanently deleted.
```

---

### 8. Live Analytics Dashboard (`stats`)
Inspect live vault health, active record counts, key metrics, and last login timestamps:

```text
vault> stats
======== VAULT STATISTICS ========
Total Records    : 2
Active Records   : 2
Deleted (hidden) : 0
--- By Type ---
Passwords        : 1
Notes            : 0
Contacts         : 1
Encryption Key   : 17
Last Login       : 19-Aug-2026
==================================
```

---

## 📖 Command Cheat Sheet

| Command | Syntax | Action |
|---|---|---|
| `help` | `help` | Display the interactive command guide |
| `add` | `add` | Launch wizard to add a Password, Note, or Contact |
| `view` | `view` | Decrypt and display active records in an aligned table |
| `search` | `search` | Substring search by Title, Category, or Global Fields |
| `sort` | `sort` | In-memory sort by ID, Title (A–Z), Type, or Date |
| `update` | `update <id>` | In-place field editor (press `ENTER` to keep values) |
| `delete` | `delete <id>` | Soft-delete record (instantly hidden from all views) |
| `purge` | `purge` | Permanently remove deleted records & compact storage |
| `stats` | `stats` | Display real-time storage metrics & breakdown |
| `passwd` | `passwd` | Change master password & re-encrypt all records |
| `export` | `export` | Export decrypted plaintext backup to `vault_export.txt` |
| `about` | `about` | Display educational cryptographic algorithm explainer |
| `clear` | `clear` | Clear terminal screen and reprint the ASCII banner |
| `exit` | `exit` | Safely flush state, lock the vault, and quit |

---

## 🔒 Cryptographic Architecture

CipherVault implements a **domain-isolated hybrid cipher pipeline** designed for 100% string reversibility with zero character collisions:

```
                  ┌───────────────────────────────────────────────┐
                  │          Plaintext Input Character            │
                  └──────────────────────┬────────────────────────┘
                                         │
                 ┌───────────────────────┴───────────────────────┐
                 │                                               │
     ['A' <= c <= 'Z']                                   [Non-Alphabetic]
                 │                               (Spaces, Digits, Symbols, @, .)
  c = ((c - 'A' + key) % 26) + 'A'                               │
                 │                                         c = c ^ 0x80
     ['a' <= c <= 'z']                                           │
                 │                                   (Sets Bit 7: c >= 128)
  c = ((c - 'a' + key) % 26) + 'a'                               │
                 │                                               │
                 └───────────────────────┬───────────────────────┘
                                         │
                                         ▼
                     [Encrypted Binary In-Memory Buffer]
                                         │
                                 fwrite() to Disk
                                         │
                                         ▼
                                   [vault.dat]
```

### 🧮 Mathematical Foundations
1. **Master Password Hashing**:
   $$\text{Hash} = \left(\sum_{i=1}^{n} \text{ASCII}(pwd_i)\right) \oplus \text{0xDEAD}$$
2. **Dynamic Key Derivation**:
   $$\text{Key} = \left(\left(\sum_{i=1}^{n} \text{ASCII}(pwd_i)\right) \pmod{25}\right) + 1 \quad \implies \quad \text{Key} \in [1, 25]$$
3. **Collision-Free High-Bit Partitioning**:
   - Alphabetic characters remain strictly within $[65, 90]$ and $[97, 122]$.
   - Non-alphabetic ASCII characters ($c \le 127$) XORed with `0x80` map into the high-byte domain ($c \ge 128$). They **never** collide with alphabet letters or null terminators (`\0`), guaranteeing that spaces, emails, and punctuation roundtrip with 100% fidelity.

---

## 📁 Project Structure

```
CipherVault/
├── 📄 vault.h            # Master Header: Struct layouts, constants, prototypes
├── 📄 auth.h / auth.c    # Authentication: 3-strike gate & masked terminal input handler
├── 📄 crypto.h / crypto.c# Crypto Engine: Caesar + 0x80 bitmask, hashing, key derivation
├── 📄 fileio.h / fileio.c# Binary I/O Engine: fread/fwrite/fseek, integrity validation
├── 📄 vault_ops.h / .c   # Business Logic: Add, View, Update, Soft-Delete, Purge, Re-encrypt
├── 📄 search.h / search.c# Query Engine: Case-insensitive search & 4-mode Bubble Sort
├── 📄 display.h / .c     # Presentation: Clean ASCII banner, formatted tables, stats dashboard
├── 📄 main.c             # Controller: Startup sequence, 14-command REPL dispatcher
├── 📄 test_b.c           # Isolated Unit Test Suite: Module tests with mock stubs
├── 📄 CiperVault_Team10.docx # 10-Section Academic Report & System Documentation
└── 📄 README.md          # Technical documentation
```

---

## 🎓 C Programming Concepts Mastered

| Concept | Application in CipherVault |
|---|---|
| **Bitwise Operations** | Modulo arithmetic, high-bit XOR bitmasking (`0x80`), hex encoding |
| **Control Flow & Gates** | 3-strike `do-while` authentication gate, 14-branch `if-else` REPL dispatch |
| **Arrays & Bounds Safety** | Fixed buffers (`struct Record buf[MAX_RECORDS]`), bounded string handling |
| **String Manipulation** | `strlen`, `strcpy`, `strcmp`, `strncpy`, `strcspn`, `sprintf`, `strftime`, `isprint`, `tolower` |
| **Pointers & Memory** | Pass-by-reference mutations (`Record *r`, `VaultMeta *meta`), pointer arithmetic |
| **Modular Architecture** | Header guards (`#ifndef`), decoupled authentication unit (`auth.c`), 31 discrete functions |
| **Binary File Streams** | Low-level binary streaming (`fopen`, `fread`, `fwrite`, `fseek` with `SEEK_SET`, `ftell`) |
| **Algorithms (Search & Sort)** | Custom manual substring matcher (`contains`), 4-criteria Bubble Sort (`swap_records`) |

---

## 🤝 Contributing & License

Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com).

This project is licensed under the **MIT License** — feel free to use, modify, and distribute as you see fit.

<div align="center">

**Built with pure C · Designed for security · Engineered for speed**  
*CipherVault — Department of Computer Science, CHRIST (Deemed to be University)*

</div>
