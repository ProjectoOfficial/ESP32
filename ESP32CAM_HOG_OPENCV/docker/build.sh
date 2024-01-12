#!/bin/bash

IMAGE_NAME="ESP32-docker"
IMAGE_TAG="1.0.0"
echo "Started build for $IMAGE_NAME:$IMAGE_TAG"

# Percorso assoluto del Dockerfile
MAIN_DIR="$(cd "$(dirname "$0")" && pwd)/.."
echo "setting main directory as $PWD"

# Esegui la build dell'immagine Docker
echo "building: $MAIN_DIR/Dockerfile"
docker build -t "$IMAGE_NAME:$IMAGE_TAG" -f "$MAIN_DIR/docker/Dockerfile" "$MAIN_DIR"


# Controlla se la build è stata completata con successo
if [ $? -eq 0 ]; then
    echo "Build dell'immagine completata con successo."
else
    echo "Si è verificato un errore durante la build dell'immagine."
fi