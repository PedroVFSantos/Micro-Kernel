#!/bin/bash

IMAGE_NAME="microkernel-builder"

if [[ "$(docker images -q $IMAGE_NAME 2> /dev/null)" == "" ]]; then
    echo "[*] Imagem Docker não encontrada. Construindo '$IMAGE_NAME'..."
    docker build -t $IMAGE_NAME .
fi

echo "[*] Compilando o kernel "

docker run --rm \
    -v "$(pwd)":/root/env \
    $IMAGE_NAME make iso

if [ -f microkernel.iso ]; then
    echo "[SUCCESS] microkernel.iso gerado"
else
    echo "[ERROR] Falha na compilação."
    exit 1
fi
