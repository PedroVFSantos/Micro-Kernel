# Microkernel C++ Project

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Platform](https://img.shields.io/badge/platform-x86__64-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

> **Um sistema operacional de arquitetura Microkernel educativo, escrito em Modern C++ e Assembly, focado em modularidade, troca de mensagens e estabilidade.**

---

## Arquitetura (Architecture)

Diferente de kernels monolíticos, este projeto segue a filosofia **Minix**: o Kernel faz o mínimo possível. Driversde vídeo, sistema de arquivos e rede rodam como processos de usuário isolados.

```mermaid
graph TD
    subgraph User Space [Ring 3 - User Space]
        style User Space fill:#f9f9f9,stroke:#333,stroke-width:2px
        VD[📺 Video Driver]
        KBD[⌨️ Keyboard Driver]
        FS[📂 File System]
        SHELL[>_ Shell/User Apps]
    end

    subgraph IPC Layer [IPC Bus]
        style IPC Layer fill:#e1f5fe,stroke:#0277bd,stroke-width:2px,stroke-dasharray: 5 5
        MSG(Message Passing System)
    end

    subgraph Kernel Space [Ring 0 - Microkernel]
        style Kernel Space fill:#ffebee,stroke:#c62828,stroke-width:2px
        SCHED[⏱️ Scheduler]
        MEM[💾 Memory Manager]
        INT[⚡ Interrupt Handling]
    end

    %% Conexões
    VD <--> MSG
    KBD <--> MSG
    FS <--> MSG
    SHELL <--> MSG
    MSG <--> SCHED
    MSG <--> MEM
    MSG <--> INT
