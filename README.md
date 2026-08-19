# CipherVault

A local-first, offline personal data manager written in pure ANSI C — no dependencies, no runtime, no network calls. Just a compiler and a terminal.

CipherVault stores passwords, secure notes, and contacts in a custom binary format, encrypted with a lightweight in-house cipher before anything hits disk. It's a from-scratch systems project: custom binary I/O, in-place file mutation, a REPL-style CLI, and a small crypto layer — all built without pulling in a single external library.

---

## Why this project exists

Most "build a password manager" tutorials stop at a JSON file and `fopen`. CipherVault goes further on purpose:

- **Custom binary file format** — records are read and written as raw structs via `fread`/`fwrite`, with direct `fseek` offset access for in-place updates instead of full-file rewrites.
- **Lightweight custom cipher** — a Caesar-style shift combined with an XOR bitmask, with the shift key derived from a hash of the master password. This is a hand-rolled obfuscation layer, not a production cryptographic algorithm — it exists to demonstrate key derivation, encode/decode symmetry, and binary-safe transforms, not to withstand a real attacker. (If you're evaluating this as a security tool: don't. If you're evaluating it as a C systems project: this is the fun part.)
- **Zero dependencies, zero network** — compiles with GCC/Clang/MinGW and nothing else. No cloud sync, no telemetry, no external calls. Everything lives in one file on your machine.
- **Real CLI ergonomics** — masked password input with live backspace handling, aligned table rendering, in-memory search and sort, and a proper first-run setup flow.

---

## Quickstart

**Requirements:** any standard C compiler (GCC, Clang, or MinGW / Dev-C++).

```bash
gcc --version   # sanity check
```

```bash
git clone https://github.com/YourUsername/CipherVault.git
cd CipherVault
gcc main.c auth.c crypto.c display.c fileio.c vault_ops.c search.c -o ciphervault -Wall
./ciphervault
```

**Windows (Dev-C++ / Visual Studio):** create a new C Console project, add all `.c`/`.h` files, then Build & Run (`F11`).

---

## What it actually does

### First run
On first launch, CipherVault sets up a master password (masked input, no echo), derives an encryption key from it, and initializes the binary vault file.

```text
First run detected. Set your master password: **********
Vault initialized successfully.
Enter master password: **********
Access granted. Welcome back.
```

Three consecutive failed logins terminate the session — a basic brute-force gate.

### Records: passwords, notes, contacts
`add` walks you through a type-specific wizard (password fields vs. note fields vs. contact fields), then encrypts and commits the record to disk.

```text
vault> add
Select record type:
1. Password  2. Secure Note  3. Contact
Choice: 1
Title: GitHub Master Key
Username: octocat@github.com
Password: **************
URL (optional): https://github.com/login
Record saved. ID: 1
```

### View, search, sort
`view` decrypts active records into an aligned table. `search` does case-insensitive substring matching across titles, types, or all fields. `sort` reorders the in-memory view by ID, title, type, or date — without touching the underlying file layout.

```text
vault> view
ID   | Type       | Title                        | Preview
---- | ---------- | ---------------------------- | --------------------
1    | Password   | GitHub Master Key            | octocat@github.com
2    | Note       | Server Infrastructure Config | SSH Keys & Port 2222
3    | Contact    | Linus Torvalds               | Linus Torvalds
```

### In-place updates
`update <id>` seeks directly to the record's byte offset and overwrites only the changed fields — press Enter to leave a field untouched.

### Two-phase deletion
`delete <id>` soft-deletes a record instantly (flag flip, no rewrite). `purge` does the expensive work once: compacts the file, drops soft-deleted records, and re-indexes IDs.

```text
vault> delete 2
Record 2 marked as deleted.

vault> purge
WARNING: This will permanently remove all deleted records. Continue? (y/n): y
Purge complete. 1 record permanently deleted.
```

### Everything else
`stats` gives a live breakdown of record counts and vault metadata. `passwd` re-derives the key and re-encrypts the whole vault atomically when you change your master password. `export` dumps a decrypted plaintext backup on demand.

---

## Command reference

| Command | Syntax | Does |
|---|---|---|
| `help` | `help` | Show the command guide |
| `add` | `add` | Add a password, note, or contact |
| `view` | `view` | Decrypt and list active records |
| `search` | `search` | Search by title, type, or any field |
| `sort` | `sort` | Sort by ID, title, type, or date |
| `update` | `update <id>` | Edit a record's fields in place |
| `delete` | `delete <id>` | Soft-delete a record |
| `purge` | `purge` | Permanently remove deleted records, compact file |
| `stats` | `stats` | Show vault metrics |
| `passwd` | `passwd` | Change master password, re-encrypt vault |
| `export` | `export` | Write a decrypted backup to `vault_export.txt` |
| `about` | `about` | Explain the cipher and key derivation |
| `clear` | `clear` | Clear screen, reprint banner |
| `exit` | `exit` | Flush state and quit |

---

## Project structure

```
CipherVault/
├── vault.h            # Struct layouts, constants, prototypes
├── auth.h / auth.c    # Login gate, masked terminal input
├── crypto.h / crypto.c# Cipher, hashing, key derivation
├── fileio.h / fileio.c# Binary read/write/seek, integrity checks
├── vault_ops.h / .c   # Add / view / update / delete / purge / re-encrypt
├── search.h / search.c# Search + bubble sort (4 modes)
├── display.h / .c     # Banner, tables, stats dashboard
├── main.c             # Startup + REPL command dispatch
└── README.md
```

---

## Notes for reviewers

This project was built to practice C fundamentals that are easy to skip in higher-level languages: manual memory layout, binary file formats, byte-offset seeking, and writing a REPL loop from scratch. The cipher is intentionally simple and documented as such in `about` — the goal was understanding key derivation and encode/decode symmetry, not building something attacker-resistant. Happy to walk through any part of the implementation.
