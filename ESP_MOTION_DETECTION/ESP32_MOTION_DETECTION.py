# (C) Dott. Daniel Rossi 09/10/2022 - ProjectoOfficial
# how to flash the firmware: https://www.hackster.io/onedeadmatch/esp32-cam-python-stream-opencv-example-1cc205


import cv2
import numpy as np
from datetime import datetime

import requests
import time
import os

# ESP32 URL
URL = "" #update with your URL

cap = cv2.VideoCapture(URL + ":81/stream")
requests.get(URL + "/control?var=framesize&val=10") # 10: UXGA(1600x1200)

SAVE_MOTION_FRAME = True
current = os.path.dirname(os.path.realpath(__file__))  

if __name__ == '__main__':
    
    prev_frame = None

    if cap.isOpened():
        ret, prev_frame = cap.read()
        prev_frame = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)
        prev_frame = cv2.medianBlur(prev_frame, 5)

    kernel = np.ones((5, 5))
    start_time = time.perf_counter()
    while True:
        if cap.isOpened():
            ret, frame = cap.read()

            original = frame.copy()            
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            gray = cv2.medianBlur(gray, 5)

            if ret and prev_frame is not None:
                difference = cv2.absdiff(prev_frame, gray)
                thresh = cv2.threshold(difference, thresh=80, maxval=255, type=cv2.THRESH_BINARY)[1]
                thresh = cv2.dilate(thresh, kernel, 2)
                #cv2.imshow("frame", thresh)
                
                contours, _ = cv2.findContours(thresh, mode=cv2.RETR_EXTERNAL, method=cv2.CHAIN_APPROX_SIMPLE)

                motion = False
                for contour in contours:
                    if cv2.contourArea(contour) < 100:
                        continue
                    (x, y, w, h) = cv2.boundingRect(contour)
                    cv2.rectangle(frame, (x, y), (x + w, y + h), color=(0, 255, 255), thickness=2)
                    motion = True
            
            cv2.imshow("frame", frame)

            if SAVE_MOTION_FRAME and (time.perf_counter() - start_time) >= 2:
                if motion:
                    filename = os.path.join(current, "{}.jpg".format(datetime.now().strftime("%d_%m_%Y__%H_%M_%S")))
                    cv2.imwrite(filename, original)
                    start_time = time.perf_counter()

            prev_frame = gray.copy()

        key = cv2.waitKey(1)
        if key == 27: # ESC
            break

    cv2.destroyAllWindows()
    cap.release()


    