# MancoOS - Educational Microkernel

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Platform](https://img.shields.io/badge/platform-i386-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

**MancoOS** is a microkernel-based operating system designed for educational purposes. Built from scratch using **C++17** and **x86 Assembly**, it emphasizes modularity, memory isolation, and a robust inter-process communication (IPC) system.

---

## System Evidence

| **IDT & Boot Sequence** | **SJF Scheduler Initialization** |
|:------------------------:|:---------------------------------:|
| ![IDT Load](image_ec6601.png) | ![Scheduler Start](image_ecd206.png) |
| *Kernel successfully loading the Interrupt Descriptor Table.* | *The SJF algorithm starting to manage system tasks.* |

---

## Architecture

Following the **Minix philosophy**, MancoOS keeps the Ring 0 (Kernel Space) as minimal as possible. Services such as video drivers and input handlers run in Ring 3 (User Space), communicating via a message-passing bus.

```mermaid
graph TD
    classDef userLayer fill:#E3F2FD,stroke:#1565C0,stroke-width:2px,color:#0D47A1
    classDef ipcLayer fill:#FFF3E0,stroke:#EF6C00,stroke-width:2px,stroke-dasharray: 5 5,color:#E65100
    classDef kernelLayer fill:#FFEBEE,stroke:#C62828,stroke-width:2px,color:#B71C1c

    subgraph Ring3 [Ring 3 - User Space]
        direction LR
        VD[VGA Service]:::userLayer
        KBD[Input Service]:::userLayer
        APP[Applications]:::userLayer
    end

    subgraph Bus [Inter-Process Communication]
        IPC{{Message Passing System}}:::ipcLayer
    end

    subgraph Ring0 [Ring 0 - Microkernel]
        direction LR
        SCHED[SJF Scheduler]:::kernelLayer
        MM[Memory Manager]:::kernelLayer
        ISR[Interrupt Handler]:::kernelLayer
    end

    VD <--> IPC
    KBD <--> IPC
    APP <--> IPC

    IPC <--> SCHED
    IPC <--> MM
    IPC <--> ISR
```

---

## Key Technical Features

- **SJF Scheduler**: Implements the Shortest Job First algorithm to minimize average waiting time, featuring assembly-level context switching.
- **Physical Memory Manager (PMM)**: A high-performance memory allocator using a Bitmap structure to manage 4KB pages.
- **Custom Visual Identity**: A unique VGA color palette based on Rich Mahogany and Coffee Bean codes.
- **Robust Exception Handling**: Custom ISRs and IDT configuration to ensure system stability and prevent triple faults.
- **Containerized Build System**: A fully automated Docker pipeline for cross-compiling i386-elf binaries.

---

## How to Run

### 1. Build the ISO (Docker)

Ensure you have Docker installed and run the following command to generate the bootable image:

```powershell
docker run --rm -v ${PWD}:/root/env microkernel-builder make iso
```

### 2. Execute via QEMU

You can run the OS locally or through a browser-based VNC container:

```powershell
docker run -it --rm -p 8006:8006 -v ${PWD}:/storage -e BOOT="file:///storage/microkernel.iso" -e KVM=N qemux/qemu
```

Access the system at `http://localhost:8006`.

---

## Author

Developed by **PedroVFSantos** - Computer Science Student at UFSCar.
