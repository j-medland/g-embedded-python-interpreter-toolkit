import analysis
import cv2

def create_analyser():
    return analysis.BlobAnalysis()

def analyse_image(analyser: analysis.BlobAnalysis, image):
    analyser.analyse(image)
    return analyser.keypoint_tuples()

