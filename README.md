# Microkernel C++ Project

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Platform](https://img.shields.io/badge/platform-x86__64-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

> **A Microkernel architecture operating system for educational purposes, written in Modern C++ and Assembly, focused on modularity, message passing, and stability.**

---

## Architecture

Unlike monolithic kernels, this project follows the **Minix** philosophy:  
the Kernel does the bare minimum. Video drivers, file systems, and network stacks run as isolated user processes.

```mermaid
graph TD
    %% Style Definitions
    classDef userLayer fill:#E3F2FD,stroke:#1565C0,stroke-width:2px,color:#0D47A1
    classDef ipcLayer fill:#FFF3E0,stroke:#EF6C00,stroke-width:2px,stroke-dasharray: 5 5,color:#E65100
    classDef kernelLayer fill:#FFEBEE,stroke:#C62828,stroke-width:2px,color:#B71C1c

    %% Ring 3 - User Space
    subgraph Ring3 [Ring 3 - User Space]
        direction LR
        VD[Video Driver]:::userLayer
        KBD[Input Driver]:::userLayer
        FS[File System]:::userLayer
        APP[User Applications]:::userLayer
    end

    %% IPC Layer
    subgraph Bus [Inter-Process Communication]
        IPC{{Message Passing System}}:::ipcLayer
    end

    %% Ring 0 - Kernel Space
    subgraph Ring0 [Ring 0 - Microkernel]
        direction LR
        SCHED[Scheduler]:::kernelLayer
        MM[Memory Manager]:::kernelLayer
        ISR[Interrupt Handler]:::kernelLayer
    end

    %% Connections
    VD <--> IPC
    KBD <--> IPC
    FS <--> IPC
    APP <--> IPC

    IPC <--> SCHED
    IPC <--> MM
    IPC <--> ISR
