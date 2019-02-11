# CSCI440_Vision
## Project page
- https://web.stcloudstate.edu/bajulstrom/cs440/find-box/find-box-new.html

### Install Programs

##### Required
- OpenCV:
  - https://opencv.org/releases.html
- Install Visual Studio Community:
  - https://visualstudio.microsoft.com/downloads/


##### CLI arguments:
```
  -i: input file.  name of a bitmap file. required
  -o: output file. Name the output file to be. required
  -t: thresh hold. Threshold for the edges Function. Must be an integer. default = 150. Optional.
  -c: contrast multiplier. Can be a float default = 1.3 Must but > 1. Optional.
  -s: Smoothing window size. Changes the size of the neighboring pixels around an area. 1 is a 3x3 window.
        Must be an integer. If the size s the window is a 2*s*+1 by 2*s*+1. default = 2. Optional.
  -e: edge layer. Looks for an edge in the a certain window size. Must be an integer.
       If the size *s* the window is a 2*s*+1 by 2*s*+1. default = 1. Optional. RECOMMNED to leave it at the default value.
```

#### How to run the program:
  ```
  step 1: make
  step 2: vision.exe -i .\2.bmp -o image_2_thinned.bmp
  or
  vision.exe -i .\2.bmp -o image_2_thinned.bmp -c 1.2 -t 150 -s 3 -e 2
  ```
# Thank you
A big thank you to Mark A. Renslow for open sourcing part of his project for a template.
The original template can be found [here](https://web.stcloudstate.edu/bajulstrom/cs440/find-box/BP.txt)
