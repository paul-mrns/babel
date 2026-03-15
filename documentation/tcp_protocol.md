# **TCP PROTOCOL** : Networking flow

## 1. Packets structure

### Header

The packet header is composed of :  
- an **op_code** (uint8_t = 1 byte) : 0x01 -> 0x0e
- the **body size** (uint16_t = 2 bytes), which correpsond to the variable length of the following body

### Body

- Type: std::vector<uint8_t>
- Content: The body is a raw byte stream. While the protocol is binary, the payload contains either integers for multiple-state response or binary infos such as ip and port sharing.

## 2. Networking flow

### 2.1. Server connexion ("CONNECT" cmd)

```mermaid
sequenceDiagram
    participant Client
    participant Server

    Client->>Server: {op_code : 0x01, {}}
    Server->>Client: {op_code : 0x02, {0 OR 1 (0 = success, 1 = failure)}}
```

### 2.2. User Registration ("REGISTER" cmd)

```mermaid
sequenceDiagram
    participant Client
    participant Server

    Client->>Server: {op_code : 0x03, {username + password}}
    Server->>Client: {op_code : 0x04, {0 OR 1 OR 2(0 = success, 1 = failure, 2 = user already registered)}}
```

### 2.3. User Login ("LOGIN" cmd)

```mermaid
sequenceDiagram
    participant Client
    participant Server

    Client->>Server: {op_code : 0x05, {username + password}}
    Server->>Client: {op_code : 0x06, {0 OR 1 OR 2(0 = success, 1 = failure, 2 = user already login)}}
```

### 2.4. User listing ("USERS" cmd)

```mermaid
sequenceDiagram
    participant Client
    participant Server

    Client->>Server: {op_code : 0x07, {}}
    Server->>Client: {op_code : 0x08, {user1, user2, user3, ...}}
```

### 2.5. Calling user ("CALL" cmd)

```mermaid
sequenceDiagram
    participant ClientA
    participant Server
    participant ClientB

    ClientA->>Server: {op_code : 0x09, {clientB username}}
    Server->>ClientB: {op_code : 0x0a, {}}
    alt ClientB types "ACCEPT"
        ClientB->>Server: {op_code: 0x0b, {0}}
    else ClientB types "DECLINE"
        ClientB->>Server: {op_code: 0x0b, {1}}
    end
```

### 2.6. Call process

```mermaid
sequenceDiagram
    participant ClientA
    participant Server
    participant ClientB

    Server->>ClientA: {op_code : 0x0c, {0}}
    Server->>ClientB: {op_code : 0x0c, {0}}
    ClientA->>Server: {op_code : 0x0d, {clientA UDP ip + port}}
    ClientB->>Server: {op_code : 0x0d, {clientB UDP ip + port}}
    Server->>ClientA: {op_code : 0x0d, {clientB UDP ip + port}}
    Server->>ClientB: {op_code : 0x0d, {clientA UDP ip + port}}
```

### 2.7. END_CALL

```mermaid
sequenceDiagram
    participant Client
    participant Server

    Client->>Server: {op_code : 0x0e, {}}

```

