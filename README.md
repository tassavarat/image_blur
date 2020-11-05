# image_blur: multithreaded image blur

`image_blur` allows one to blur an image in hopefully a reasonable amount of time. The project was written as a way to learn and apply multithreading to speed up a task. This program will only blur PBM images.

## Requirements

* Linux distro
* git

## Installation

To use `image_blur`, first clone the repository to a desired location
```
$ git clone https://github.com/tassavarat/image_blur.git
```
If desired, one can compile `image_blur` locally using make
```
$ make
```

If one wishes to delete the object files
```
$ make clean
```

## Usage

To use the program, run the executable `img_blur`. The program takes two arguments, the PBM image to blur, and the convolution matrix used for blurring. The blurred image will be written in the same directory as the executable and be named `output.pbm`. The images folder contains a sample image to blur and what it will look like once Gaussian Blur is applied. The kernel folder contains the Gaussian Blur kernel used.
```
$ ./img_blur images/sample.pbm kernels/gaussian.knl
```

### Before
![Sample image](https://i.imgur.com/zj7KCDk.jpg)

### After
![Blurred image](https://i.imgur.com/SD1Jfgf.jpg)
