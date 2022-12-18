# Borrowed from James McNally of Wiresmith Technology : https://www.wiresmithtech.com/
# https://github.com/JamesMc86/Python-and-LabVIEW-Presentation/blob/main/python-node-opencv/python/analysis.py

import cv2
import numpy as np
from collections import namedtuple


class BlobAnalysis:
    def __init__(self):
        params = cv2.SimpleBlobDetector_Params()
        params.minThreshold = 110
        params.maxThreshold = 255
        params.filterByColor = True
        params.blobColor = 0
        params.filterByArea = True
        params.maxArea = 150000
        self.detector = cv2.SimpleBlobDetector_create(params)

    def analyse(self, image):
        self.keypoints = self.detector.detect(image)

    def overlay_image(self, image):
        return cv2.drawKeypoints(image, self.keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

    def keypoint_tuples(self):
        tuples = []

        Keypoint = namedtuple("Keypoint", "x y diameter")
        for keypoint in self.keypoints:
            (x,y) = keypoint.pt
            diameter = keypoint.size
            tuple = Keypoint(x, y, diameter)
            tuples.append(tuple)

        return tuples