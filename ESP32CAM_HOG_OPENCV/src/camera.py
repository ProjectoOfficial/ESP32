import cv2
import argparse
import serial 
import serial.tools.list_ports
import requests
import time

def camera_settings(cap: cv2.VideoCapture) -> cv2.VideoCapture:
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    cap.set(cv2.CAP_PROP_FPS, 30)
    cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.25)

    return cap

def set_esp32camera(socket: str) -> None:
    settings = [
        "/control?var=framesize&val=7",
        "/control?var=quality&val=10",
        "/control?var=awb&val=1",
        "/control?var=awb_gain&val=1",
        "/control?var=brightness&val=-1",
        "/control?var=aec1&val=1",
        "/control?var=aec2&val=1",
        "/control?var=saturation&val=2",
        "/control?var=dcw&val=0",
    ]

    for setting in settings:
        response = requests.get(socket + setting)
        time.sleep(0.1)
        print(response.reason)

def open_camera(args: argparse.Namespace) -> cv2.VideoCapture:
    if args.camera_index is not None:
        cap = cv2.VideoCapture(args.camera_index)
    elif args.camera_socket is not None:
        cap = cv2.VideoCapture(args.camera_socket)
        cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    else: 
        raise Exception("You must specify either a camera index or a camera ip")
    assert cap.isOpened(), "Cannot open camera"

    ret, frame = cap.read()
    assert ret and frame.size > 0, "Cannot read camera"

    cap = camera_settings(cap)
    return cap

def close_camera(cap: cv2.VideoCapture) -> None:
    assert cap is not None, "Camera is not opened"
    cap.release()
    cv2.destroyAllWindows()

def get_camera_ip() -> None:
    ports = serial.tools.list_ports.comports()
    
    if not ports:
        print("No UART devices found.")
        return

    print("Available UART devices:")
    for port, desc, hwid in sorted(ports):
        print(f"{port}: {desc} ({hwid})")

    ip = None
    port = sorted([port for port in ports if "USB" in port[2]])[0][0]
    device = serial.Serial(port, 
                            baudrate=115200, 
                            bytesize=serial.EIGHTBITS,
                            parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE,
                            timeout=1,
                            xonxoff=False,
                            rtscts=False
                            )
    
    try:
        device.write(b"getIp\n")
        ip = device.readline().decode().strip()
        print(f"IP received from microcontroller: {ip}")
    finally:
        device.close()

    assert ip is not None, "could not get camera ip"
    return ip