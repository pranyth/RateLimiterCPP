# The Rate Limiter — C++ Middleware

A high-performance, Linux-based rate limiting middleware built in C++ to protect backend services from request abuse and DDoS-style traffic spikes.

This project was developed incrementally to mirror real-world backend system design and infrastructure workflows.

---

## 🚀 Project Overview

**Problem**
Modern APIs must protect themselves from excessive or malicious traffic. Without a rate limiter, a small number of abusive clients can degrade performance or take a service offline.

**Solution**
This project implements a lightweight C++ middleware that:

* Listens for incoming HTTP requests
* Identifies clients using source IP
* Tracks request frequency per IP
* Enforces rate limits
* Returns HTTP 429 responses for abusive clients

The middleware is designed to run as a standalone service in front of any backend.

---

## 🏗️ Architecture

```
Client → Rate Limiter (C++) → Backend Service
```

The rate limiter operates at the **network layer**, intercepting connections before application logic executes.

---

## ⚙️ Tech Stack

| Layer      | Technology           |
| ---------- | -------------------- |
| Language   | C++                  |
| OS         | Linux (WSL2 for dev) |
| Networking | POSIX sockets        |
| Algorithm  | Fixed Window Counter |
| Container  | Docker (planned)     |
| Cloud      | AWS EC2 (planned)    |

---

## ✨ Features

* TCP server implemented using Linux sockets
* HTTP request parsing
* IP-based client identification
* Fixed-window rate limiting (5 requests per 60 seconds)
* HTTP 429 enforcement
* Graceful socket handling
* Memory-safe IP tracking with TTL eviction
* Request logging

---

## 🧠 Design Decisions

### Why C++?

* Near-zero runtime overhead
* High throughput
* Fine-grained memory control
* Common in real infrastructure systems

### Why IP-Based Limiting?

* Available immediately after connection acceptance
* No dependency on HTTP parsing
* Works at the network layer

### Why Fixed Window Algorithm?

* Simple
* Predictable
* Easy to reason about
* Suitable for MVP

---

## 🧪 How It Works

1. Server opens a TCP socket on port 8080
2. Client connects → OS assigns a client socket
3. Source IP is extracted
4. Request count for that IP is updated
5. If limit exceeded → HTTP 429 returned
6. Otherwise → request forwarded (simulated)
7. Connection closed gracefully

---

## 🛡️ Memory Safety

To prevent unbounded memory growth:

* Each IP entry tracks `last_seen` timestamp
* Inactive IPs are evicted after 5 minutes
* Cleanup runs every 30 requests

This mimics TTL-based eviction used in production systems.

---

## 🧱 Project Evolution (Narratable in Interviews)

### Phase 0 — Environment Setup

* Configured Linux dev environment using WSL2
* Installed Ubuntu 22.04
* Set up C++ toolchain

**Outcome:** Production-like dev setup

---

### Phase 1 — Basic TCP Server

* Implemented socket creation, binding, and listening
* Handled incoming TCP connections
* Sent valid HTTP responses

**Outcome:** Functional HTTP server in C++

---

### Phase 2 — Core Rate Limiter

* Extracted client IP using `inet_ntop()`
* Added per-IP request counters
* Enforced fixed-window limits
* Returned HTTP 429 for blocked clients

**Outcome:** Real working rate limiter

---

### Phase 3 — Stability Improvements

* Added graceful socket close delay
* Eliminated TCP reset issues

**Outcome:** Cleaner client behavior

---

### Phase 4 — Memory Safety

* Added TTL-based eviction
* Periodic cleanup of stale IPs

**Outcome:** Long-running stability

---

## 🧪 How to Run

```bash
# Compile
g++ server.cpp -o server

# Run
./server
```

Test:

```bash
for i in {1..7}; do curl http://localhost:8080; done
```

---

## 📈 Current Capabilities

* Linux-based C++ TCP server
* IP-based rate limiting
* Fixed-window enforcement
* HTTP 429 blocking
* TTL-based memory safety
* Graceful socket handling

---

## 🔮 Roadmap

* Docker containerization
* Config file support
* Thread safety
* Token bucket algorithm
* AWS deployment

---

## 🎤 Interview Pitch (60-Second Version)

> "I built a C++ rate limiting middleware that sits in front of backend services to protect them from abusive traffic. It listens for incoming TCP connections, extracts the source IP, and enforces a fixed-window rate limit. If a client exceeds the limit, it returns an HTTP 429 response and terminates the connection early to save resources. I also implemented TTL-based eviction to prevent memory growth from inactive IPs. I developed it on Linux using POSIX sockets and designed it to be Docker- and cloud-ready."

---

*End of README*
