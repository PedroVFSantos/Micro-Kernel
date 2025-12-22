#pragma once
#include <stdint.h>

namespace Manco {
    enum class MessageType : uint32_t {
        SYS_PRINT,
        SYS_READ_KBD,
        FS_OPEN,
        IPC_EMPTY
    };

    struct Message {
        uint32_t sender_id;
        MessageType type;
        uint32_t data[4];
    };
    
    class IPC {
    public:
        static void send(uint32_t dest, Message* msg);
        static void receive(uint32_t source, Message* msg);
    };
}