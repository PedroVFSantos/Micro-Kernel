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
        static constexpr uint32_t ANY = 0xFFFFFFFF;
        static constexpr int MAILBOX_SIZE = 8;

        // Retorna false se o destino não existe ou a caixa dele está cheia
        static bool send(uint32_t dest, Message* msg);
        // Bloqueia a tarefa atual até chegar uma mensagem de `source` (ou de qualquer um, com ANY)
        static void receive(uint32_t source, Message* msg);
    };
}
