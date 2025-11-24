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
    %% Definição de Classes de Estilo (Cores profissionais)
    classDef userLayer fill:#E3F2FD,stroke:#1565C0,stroke-width:2px,color:#0D47A1
    classDef ipcLayer fill:#FFF3E0,stroke:#EF6C00,stroke-width:2px,stroke-dasharray: 5 5,color:#E65100
    classDef kernelLayer fill:#FFEBEE,stroke:#C62828,stroke-width:2px,color:#B71C1c

    %% Camada de Usuário
    subgraph Ring3 [Ring 3 - User Space]
        direction LR
        VD[Video Driver]:::userLayer
        KBD[Input Driver]:::userLayer
        FS[File System]:::userLayer
        APP[User Applications]:::userLayer
    end

    %% Camada de IPC (O Barramento)
    subgraph Bus [Inter-Process Communication]
        IPC{{Message Passing System}}:::ipcLayer
    end

    %% Camada de Kernel
    subgraph Ring0 [Ring 0 - Microkernel]
        direction LR
        SCHED[Scheduler]:::kernelLayer
        MM[Memory Manager]:::kernelLayer
        ISR[Interrupt Handler]:::kernelLayer
    end

    %% Conexões
    VD <--> IPC
    KBD <--> IPC
    FS <--> IPC
    APP <--> IPC

    IPC <--> SCHED
    IPC <--> MM
    IPC <--> ISR

    %% Conexões
    VD <--> MSG
    KBD <--> MSG
    FS <--> MSG
    SHELL <--> MSG
    MSG <--> SCHED
    MSG <--> MEM
    MSG <--> INT
