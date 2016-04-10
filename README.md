# Photo mosaic

This openFrameworks application generates [photo mosaics](https://en.wikipedia.org/wiki/Photographic_mosaic). Specify a target image ("outer image") and a directory of candidate images and the program will assign all the candidate images to a tile in the mosaic. All candidates are assigned exactly once with none missing and no duplicates. Thus the following [samples](https://www.flickr.com/photos/genekogan/albums/72157666460255840) are all made of exactly the same tiles, which is the animals categories of the [Caltech-256](http://www.vision.caltech.edu/Image_Datasets/Caltech256/images/) dataset. Samples:


![Saraswati by Raja Ravi Varma](https://c2.staticflickr.com/2/1583/25748935214_4399510909_b.jpg "Saraswati by Raja Ravi Varma")
Raja Ravi Varma

![Tarsila do Amaral](https://c2.staticflickr.com/2/1471/25751013693_82fda34ec9_c.jpg "Tarsila do Amaral")
Tarsila

![@genekogan](https://c2.staticflickr.com/2/1644/26327965226_ab93e463c7_c.jpg "@genekogan")
[@genekogan](http://www.twitter.com/genekogan)

### How it works

1) the target image is divided into a matrix of tiles, and a color histogram is extracted from each of the target image's tiles, as well as all the candidate images from the specified directory.

2) a distance measurement is extracted between every possible pair of tile and candidate image. the metric used is the [Earth Mover's Distance](https://en.wikipedia.org/wiki/Earth_mover%27s_distance) (EMD) between the histograms of each tile and candidate. 

3) the candidates are assigned to the grid so as to minimize the total EMD.

### Requirements
 - [ofxHistogram](https://github.com/genekogan/ofxHistogram)
 - [ofxEMD](https://github.com/genekogan/ofxEMD)
 - [ofxAssignment](https://github.com/kylemcdonald/ofxAssignment)


