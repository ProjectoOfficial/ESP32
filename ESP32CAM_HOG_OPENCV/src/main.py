import argparse
import cv2
import time
import joblib
import numpy as np

from camera import open_camera, close_camera, set_esp32camera, get_camera_ip
from detectors import hog_detect, svm_detect_hog
from utils import create_gaborfilter, apply_filter


def grad_frame(frame: np.ndarray) -> np.ndarray:
    gradx = cv2.Sobel(frame, cv2.CV_32F ,1, 0, ksize=3)
    grady = cv2.Sobel(frame, cv2.CV_32F ,0, 1, ksize=3)
    # compute the magnitude and angle of the gradients
    norm, angle = cv2.cartToPolar(gradx, grady, angleInDegrees=True)
    return norm, angle

def process_camera(args: argparse.Namespace) -> None:
    cv2.startWindowThread()

    cap = open_camera(args)

    hog = cv2.HOGDescriptor(_winSize=(64,128), _blockSize=(16,16), _blockStride=(8,8), _cellSize=(8,8), _nbins=9)
    hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
    clahe = cv2.createCLAHE(clipLimit=8.0, tileGridSize=(8,8))

    model = joblib.load('models/models.dat')
    gabor = create_gaborfilter()
    while True:
        fps_start_time = time.time()
        ret, frame = cap.read()
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        alpha = 0.2
        blur = cv2.bilateralFilter(frame,9,75,75)
        frame = cv2.addWeighted(frame, alpha, blur, (1.0 - alpha), 0.0)
        frame = apply_filter(frame, gabor)


        if ret and frame.size > 0:
            detections = hog_detect(hog, frame)
            # detections = svm_detect_hog(frame, model)

            for (x0, y0, x1, y1), score in detections:
                if score > 0.55:
                    cv2.rectangle(frame, (x0, y0), (x1, y1), (0, 255, 0), 2)
            
            frame = cv2.putText(frame, f"FPS: {1/(time.time() - fps_start_time):.2f}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
            
            norm, angle = grad_frame(frame.copy())
            frame = np.concatenate((frame, norm.astype(np.uint8), angle.astype(np.uint8)), axis=1)
            cv2.imshow("Original", frame)
            
        else:
            raise Exception("Unexpected error occured while reading the camera")

        if cv2.waitKey(1) & 0xFF == ord("q"):
            break

    close_camera(cap)


def main(args: argparse.Namespace):
    if args.camera_socket is None and args.camera_index is None:
        ip = get_camera_ip()
        args.camera_socket = "http://" + ip 
        set_esp32camera(socket)
        args.camera_socket += ":81/stream"
        print(f"Camera socket: {args.camera_socket}")

    elif args.camera_socket:
        set_esp32camera(args.camera_socket)
        args.camera_socket += ":81/stream"
        print(f"Camera socket: {args.camera_socket}")

    process_camera(args)
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--camera-index", type=int, default=None, help="Index of the camera to use")
    parser.add_argument("--camera-socket", type=str, default=None, help="socket of the camera to use")
    parser.add_argument("--get-camera-ip", action="store_true", help="True if you want to get the camera ip")

    args = parser.parse_args()

    main(args)