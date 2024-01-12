import cv2
import numpy as np
import imutils
import sklearn
from skimage.feature import hog
from skimage import color
from skimage.transform import pyramid_gaussian

from utils import non_max_suppression

def hog_detect(hog, frame:np.ndarray, win_stride: tuple=(4,4), padding: tuple=(8,8), scale: float=1.55, useMeanshiftGrouping: bool=False):
    boxes, weights = hog.detectMultiScale(frame, winStride=win_stride, padding=padding, scale=scale, useMeanshiftGrouping=False)
    boxes = np.array([[x, y, x + w, y + h] for (x, y, w, h) in boxes])
    boxes, weights = non_max_suppression(boxes, weights, overlapThresh=0.25)

    return zip(boxes, weights)

def sliding_window(image, window_size, step_size):
    for y in range(0, image.shape[0], step_size[1]):
        for x in range(0, image.shape[1], step_size[0]):
            yield (x, y, image[y: y + window_size[1], x: x + window_size[0]])

def svm_detect_hog(frame:np.ndarray, model, overlap_threshold: float=0.4, size: tuple=(64, 128), step_size=(32, 32), downscale: float=1.25):
    detections = []
    scale = 0

    for im_scaled in pyramid_gaussian(frame, downscale = downscale):
        #The list contains detections at the current scale
        if im_scaled.shape[0] < size[1] or im_scaled.shape[1] < size[0]:
            break
        for (x, y, window) in sliding_window(im_scaled, size, step_size):
            if window.shape[0] != size[1] or window.shape[1] != size[0]:
                continue

            if len(window.shape) > 2:
                window = color.rgb2gray(window)
            fd = hog(window, orientations=9, pixels_per_cell=(8,8), visualize=False, cells_per_block=(3,3))
            fd = fd.reshape(1, -1)
            pred = model.predict(fd)

            if pred == 1:
                
                if model.decision_function(fd) > 0.5:
                    detections.append((int(x * (downscale**scale)), int(y * (downscale**scale)), model.decision_function(fd), 
                    int(size[0] * (downscale**scale)),
                    int(size[1] * (downscale**scale))))
        scale += 1

    boxes = np.array([[x, y, x + w, y + h] for (x, y, _, w, h) in detections])
    scores = np.array([score[0] for (x, y, score, w, h) in detections])
    boxes, scores = non_max_suppression(boxes, scores, overlapThresh=overlap_threshold)

    return zip(boxes, scores)