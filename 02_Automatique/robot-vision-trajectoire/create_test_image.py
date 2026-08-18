"""
create_test_image.py
Generate a synthetic test image: a black curved line (the
trajectory) on a light background, and save it as a PNG file.
This image is the input of our line-detection module.
"""

import os               # create the output folder
import numpy as np      # work with images as grids of numbers
import cv2              # OpenCV: image processing and drawing

# 1. Image size, in pixels
WIDTH = 600
HEIGHT = 400

# 2. Create a light-gray background.
#    Shape (HEIGHT, WIDTH, 3): a grid of pixels, 3 values each
#    (Blue, Green, Red). dtype uint8: integers from 0 to 255.
image = np.full((HEIGHT, WIDTH, 3), 230, dtype=np.uint8)

# 3. Build the points of a curved trajectory (a sine wave)
points = []
for x in range(0, WIDTH):
    y = int(HEIGHT / 2 + np.sin(x / WIDTH * 2 * np.pi * 1.5) * 90)
    points.append([x, y])
points = np.array(points, dtype=np.int32)

# 4. Draw the black line on the image
#    color (0, 0, 0) = black, thickness = 6 pixels
cv2.polylines(image, [points], isClosed=False,
              color=(0, 0, 0), thickness=6)

# 5. Save the image to disk
os.makedirs("images", exist_ok=True)
output_path = "images/test_track.png"
cv2.imwrite(output_path, image)

print("Test image saved to:", output_path)