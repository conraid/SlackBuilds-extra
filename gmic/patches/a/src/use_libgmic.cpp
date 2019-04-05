/*
 #
 #  File        : use_libgmic.cpp
 #                ( C++ source file )
 #
 #  Description : Show how to call the C++ version of the G'MIC library from a C++ source code.
 #                (for a C API, see 'use_libcgmic.c' instead)
 #
 #  Copyright   : David Tschumperle
 #                ( http://tschumperle.users.greyc.fr/ )
 #
 #  Licenses    : This file is 'dual-licensed', you have to choose one
 #                of the two licenses below to apply.
 #
 #                CeCILL-C
 #                The CeCILL-C license is close to the GNU LGPL.
 #                ( http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html )
 #
 #            or  CeCILL v2.1
 #                The CeCILL license is compatible with the GNU GPL.
 #                ( http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html )
 #
 #  This software is governed either by the CeCILL or the CeCILL-C license
 #  under French law and abiding by the rules of distribution of free software.
 #  You can  use, modify and or redistribute the software under the terms of
 #  the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
 #  at the following URL: "http://www.cecill.info".
 #
 #  As a counterpart to the access to the source code and  rights to copy,
 #  modify and redistribute granted by the license, users are provided only
 #  with a limited warranty  and the software's author,  the holder of the
 #  economic rights,  and the successive licensors  have only  limited
 #  liability.
 #
 #  In this respect, the user's attention is drawn to the risks associated
 #  with loading,  using,  modifying and/or developing or reproducing the
 #  software by the user in light of its specific status of free software,
 #  that may mean  that it is complicated to manipulate,  and  that  also
 #  therefore means  that it is reserved for developers  and  experienced
 #  professionals having in-depth computer knowledge. Users are therefore
 #  encouraged to load and test the software's suitability as regards their
 #  requirements in conditions enabling the security of their systems and/or
 #  data to be ensured and,  more generally, to use and operate it in the
 #  same conditions as regards security.
 #
 #  The fact that you are presently reading this means that you have had
 #  knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
 #
*/
/*
    Note : To compile this example, using g++, use :

    g++ -o use_libgmic use_libgmic.cpp -lgmic -lfftw3
*/

/*
  Uncomment the two lines below if you want to use the CImg library along with the G'MIC library.
 */
//#include "CImg.h"
//using namespace cimg_library;

#include "gmic.h"
#include <cstdio>
#include <cmath>

int main() {

  // First step : Create a list of input images.
  //--------------------------------------------
  std::fprintf(stderr,"\n- 1st step : Create input list of images.\n");

  gmic_list<float> images;                            // List of images, will contain all images pixel data
  gmic_list<char> images_names;                       // List of images names. Can be left empty if no names
  images.assign(5);                                   // Assign list to contain 5 images
  for (unsigned int i = 0; i<images._width; ++i) {
    gmic_image<float>& img = images[i];
    img.assign(256,256,1,3);                          // Assign i-th image with size 256x256x1x3 (2d color image)

    std::fprintf(stderr,"    Input image %u =  %ux%ux%ux%u, buffer : %p\n",i,
                 img._width,
                 img._height,
                 img._depth,
                 img._spectrum,
                 img._data);

    // Fill each image buffer with sinus values (with different frequencies).
    float *ptr = img;
    for (unsigned int c = 0; c<img._spectrum; ++c)
      for (unsigned int y = 0; y<img._height; ++y)
        for (unsigned int x = 0; x<img._width; ++x)
          *(ptr++) = std::cos(x/(1. + i))*std::sin(y/(1. + i + c));
  }

  // Second step : Call G'MIC API to process input images.
  //------------------------------------------------------
  std::fprintf(stderr,"\n- 2st step : Call G'MIC interpreter.\n");

  try {

    // Here you can call any G'MIC command you want !
    // (here, create a deformed average of the input images, and save it as a BMP file).
    gmic("add normalize 0,255 flower 8 sharpen 100 output foo1.bmp",images,images_names);

  } catch (gmic_exception &e) { // Catch exception, if an error occurred in the interpreter
    std::fprintf(stderr,"\n- Error encountered when calling G'MIC : '%s'\n",e.what());
    return 0;
  }

  // Third step (alternative) : Call G'MIC API to process input images.
  //---------------------------------------------------------------------
  std::fprintf(stderr,"\n- 3rd step (alternative) : Call G'MIC interpreter from empty instance.\n");

  gmic gmic_instance; // Construct first an empty 'gmic' instance

  try {

    // Here, we use the already constructed 'gmic' instance. The same instance can be used
    // several times.
    gmic_instance.run("blur 5 sharpen 1000 normalize 0,255 output foo2.bmp",images,images_names);
    std::fputc('\n',stderr);
    gmic_instance.run("+resize 50%,50% to_rgba[-1] rotate[-1] 30 drop_shadow[-1] 0,13 "
                      "blur_radial[0] 10% blend alpha output foo3.bmp",images,images_names);

  } catch (gmic_exception &e) { // Catch exception, if an error occurred in the interpreter
    std::fprintf(stderr,"\n- Error encountered when calling G'MIC : '%s'\n",e.what());
    return 0;
  }

  // Fourth step : get back modified image data.
  //---------------------------------------------
  std::fprintf(stderr,"\n- 4th step : Returned %u output images.\n",images._width);
  for (unsigned int i = 0; i<images._width; ++i) {
    std::fprintf(stderr,"   Output image %u = %ux%ux%ux%u, buffer : %p\n",i,
                 images[i]._width,
                 images[i]._height,
                 images[i]._depth,
                 images[i]._spectrum,
                 images[i]._data);
  }

  // Fourth step : Free image resources.
  //-------------------------------------
  images.assign(0U);

  // That's it !
  //-------------
  std::fprintf(stderr,"\n- That's it !\n");
  return 0;
}
