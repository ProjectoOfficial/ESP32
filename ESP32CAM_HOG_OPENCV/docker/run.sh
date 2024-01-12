#!/bin/bash

# exit on error
set -e

IMAGE_NAME="ESP32-docker"
IMAGE_TAG="1.0.0"

echo $IMAGE_NAME:$IMAGE_TAG started!

CONTAINER_NAME="ESP32"

# x11 forwarding
echo "Setting x11 forwarding"
XSOCK=/tmp/.x11-unix
XAUTH=/tmp/.docker.xauth
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -
chmod 755 $XAUTH

launch_command="docker run "
base_options="--shm-size 2GB -ti --rm "                                 # set container shared memory to 2GB
                                                                        # start container with interactive mode
                                                                        # and enable auto-remove of the container

if command nvcc -v > /dev/null 2>&1 && command nvidia-smi > /dev/null 2>&1; then
        base_options+="--gpus all "                                     # eat all gpus
        options+="--device=/dev/nvidia-modeset "                        # nvidia modeset map to support graphic card acceleration
fi


echo "preparing docker run options"
options="-v /media:/media "                                             # mount media directory
options+="${MOUNT_SRC_PATH} "                                           # mount project directory
options+="-e http_proxy -e https_proxy "                                # set environment variables for http and https
options+="-e "DISPLAY" --env "QT_X11_NO_MITSHM=1" "                     # X11 display and disable memory share in MIT-SHM for Qt applications
options+="-v $XSOCK:$XSOCK -v $XAUTH:$XAUTH "                           # X11 forward settings
options+="-e XAUTHORITY=$XAUTH "                                        # set XAUTHORITY with host authorization path
options+="--name $CONTAINER_NAME "                                      # update container name
options+="--user $(id -u):$(id -g) "                                    # sync user ID and group ID
options+="--net=host "                                                  # add internet connetion
options+="--group-add video "                                           # add container to video group
options+="--device=/dev/dri:/dev/dri "                                  # map host DRI (Direct Rendering Infrastructure) to container
options+="-v $(dirname $PWD)/src:/home/user/src "                       # mount src path

CAMERA_MOUNTED=0
UART_MOUNTED=0
while [ "$#" -gt 0 ]; do
    case "$1" in
        -w)
            video_devices=($(ls /dev/video* 2>/dev/null))
            if [ ${#video_devices[@]} -gt 0 ] && [ "$CAMERA_MOUNTED" -eq 0 ]; then
                for video_device in "${video_devices[@]}"; do
                    echo "Setting device: $video_device"
                    options+="--device ${video_device}:${video_device} "
                done
                CAMERA_MOUNTED=1
            else
                echo "Could not find any video input device or camera already mounted"
            fi
            ;;
        -u)
            uart_device=$(ls /dev/ttyUSB* 2>/dev/null | head -n 1)
            if [ -n "$uart_device" ] && [ "$UART_MOUNTED" -eq 0 ]; then
                echo "setting UART device: $uart_device"
                options+="--device ${uart_device}:${uart_device} "
                UART_MOUNTED=1
            else
                echo "Could not find any UART device or UART already mounted"
            fi
            ;;
        -d)
            shift
            if [ -n "$1" ] && [ -d "$1" ]; then
                last_dir=$(basename "$1")
                echo "mounting data directory: $last_dir"
                options+="-v ${1}:/home/user/$last_dir "
            else
                echo "Invalid or missing directory path after -d option"
                exit 1
            fi
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
    shift
done

options+="$IMAGE_NAME:$IMAGE_TAG "                                      # set image name and image tag


$launch_command $base_options $options
