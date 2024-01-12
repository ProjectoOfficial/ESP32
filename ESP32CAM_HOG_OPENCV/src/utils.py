import numpy as np
import cv2

def non_max_suppression(boxes: np.ndarray, weights:np.ndarray, overlapThresh: float = 0.4) -> np.ndarray:
    if len(boxes) == 0:
        return [], []

    # Initialize the list of picked indices
    pick = []

    # Grab the coordinates of the bounding boxes
    x0 = boxes[:, 0]
    y0 = boxes[:, 1]
    x1 = boxes[:, 2]
    y1 = boxes[:, 3]

    # Compute the area of the bounding boxes
    area = (x1 - x0 + 1) * (y1 - y0 + 1)

    # Sort the bounding boxes by their bottom-right y-coordinate
    idxs = np.argsort(y1)

    # Keep looping while some indices still remain in the indexes list
    while len(idxs) > 0:
        # Grab the last index in the indexes list and add the index value to the list of picked indices
        last = len(idxs) - 1
        i = idxs[last]
        pick.append(i)

        # Find the largest (x, y) coordinates for the start of the bounding box and the smallest (x, y) coordinates for the end of the bounding box
        xx0 = np.maximum(x0[i], x0[idxs[:last]])
        yy0 = np.maximum(y0[i], y0[idxs[:last]])
        xx1 = np.minimum(x1[i], x1[idxs[:last]])
        yy1 = np.minimum(y1[i], y1[idxs[:last]])

        # Compute the width and height of the bounding box
        w = np.maximum(0, xx1 - xx0 + 1)
        h = np.maximum(0, yy1 - yy0 + 1)

        # Compute the ratio of overlap
        overlap = (w * h) / area[idxs[:last]]

        # Delete all indexes from the index list that have overlap greater than the specified threshold
        idxs = np.delete(idxs, np.concatenate(([last], np.where(overlap > overlapThresh)[0])))

    # Return only the bounding boxes that were picked
    return boxes[pick], weights[pick]

def create_gaborfilter():
    # This function is designed to produce a set of GaborFilters 
    # an even distribution of theta values equally distributed amongst pi rad / 180 degree
     
    filters = []
    num_filters = 16  # Number of filters to create
    ksize = 3  # The local area to evaluate
    sigma = 5.0  # Larger Values produce more edges
    lambd = 10.0 # Wavelength - Higher values produce more edges
    gamma = 0.8 # Values closer to 1 produce stronger components
    psi = 0.2  # Offset value - lower generates cleaner results
    for theta in np.arange(0, np.pi, np.pi / num_filters):  # Theta is the orientation for edge detection
        kern = cv2.getGaborKernel((ksize, ksize), sigma, theta, lambd, gamma, psi, ktype=cv2.CV_64F)
        kern /= 1.0 * kern.sum()  # Brightness normalization
        filters.append(kern)
    return filters

def apply_filter(img, filters):
# This general function is designed to apply filters to our image
     
    # First create a numpy array the same size as our input image
    newimage = np.zeros_like(img)
     
    # Starting with a blank image, we loop through the images and apply our Gabor Filter
    # On each iteration, we take the highest value (super impose), until we have the max value across all filters
    # The final image is returned
    depth = -1 # remain depth same as original image
     
    for kern in filters:  # Loop through the kernels in our GaborFilter
        image_filter = cv2.filter2D(img, depth, kern)  #Apply filter to image
         
        # Using Numpy.maximum to compare our filter and cumulative image, taking the higher value (max)
        np.maximum(newimage, image_filter, newimage)
    return newimage