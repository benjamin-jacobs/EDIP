/////////////////////////////////
//
// Program Bitmap_Parser
//
// CSCI 540
//
// Author: Mark A. Renslow markminn@mac.com
//
// Instructor: Dr. B. Julstrom
//
// I am sharing this code in the spirit of collaboration.  I
// have found that it serves my purposes, and I hope it
// improves your learning experience.  Please feel free to
// modify it for your own use.  I would appreciate any
// comments, questions or bug reports.

// I may be reached at: markminn@mac.com
//
// This program will read a binary file containing a bitmap
// and load it into a structure. Each pixel is assumed to be
// represented by 8 bits.  The binary data are maintained in
// the structure as bytes.  This makes reading and writing
// the files easy.  However, when integer values need to be
// extracted, the bytes need to be assembled into integers,
// using the Assemble_Integer() function.
//
// This code was written with the notion in mind that it
// could be scaled to deal with bitmaps other than the ones
// with 256 greys.  However, this version will only work
// with 256 greys. Additonally, extra work will need to be
// done to support bitmaps with bits-per-row not evenly
// divisible by 4.  (This is the "padding" issue)
//
// If you change the size of the image, there are three
// variables in the header which will need to be
// considered for updating.  They are:
//   bfSize
//   biWidth
//   biHeight
//
// Additionally, if the width of your new image is not
// evenly divisible by four bytes, you will have to add
// "padding" bytes to make it so.  This will change
// the size of the file (bfSize), so you should update
// that parameter to reflect padding. However, I do not
// think that biWidth is to be updated to reflect
// padding.  That is a question I have not had to
// answer yet.
//
////////////////////
#define M_PI 3.14159265358979323846 /* pi */

#include<opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <math.h>
#include <cmath>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <vector>
#include <string>
#include "coordinate.h"
using namespace std;
using namespace cv;

typedef unsigned char byte_t;

float THRESH_HOLD = 150;
float CONTRAST = 1.3;
int SMOOTH_LAYERS = 2;
int EDGE_LAYERS = 1;

// The following two structures were adapted from
// http://users.ece.gatech.edu/~slabaugh/personal/c/bmpwrite.html
struct bmpFILEHEADER
{
	byte_t bfType[2]; //Bitmap identifier,  Must be "BM"
	byte_t bfSize[4];
	byte_t bfReserved[4];
	byte_t bfOffbits[4]; //specifies the location
						 //(in bytes) in the file of the
						 // image data. Should be equal to
						 // sizeof(bmpFileHeader + sizeof(bmpInfoHeader)
						 //       + sizeof(Palette)
};

struct bmpINFOHEADER
{
	byte_t biSize[4];  // Size of the bmpInfoHeader,
					   // i.e. sizeof(bmpInfoheader)
	byte_t biWidth[4]; // Width of bitmap, in pixels
					   // change this if you change
					   // the size of the image. see ***** note below

	byte_t biHeight[4];		   // Height of bitmap, in pixels
							   //change this if you change the
							   // size of the image. see ***** note below
	byte_t biPlanes[2];		   // Should/must be 1.
	byte_t biBitCount[2];	  // The bit depth of the bitmap.
							   // For 8 bit bitmaps, this is 8
	byte_t biCompression[4];   // Should be 0 for
							   // uncompressed bitmaps
	byte_t biSizeImage[4];	 //The size of the padded
							   // image, in bytes
	byte_t biXPelsPerMeter[4]; //Horizontal resolution,
							   // in pixels per meter.  Not signif.
	byte_t biYPelsPermeter[4]; //Vertical resolution,
							   //as above.
	byte_t biClrUsed[4];	   //Indicates the number of
							   //colors in the palette.
	byte_t biClrImportant[4];  //Indicates number of
							   //colors to display the bitmap.
							   // Set to zero to indicate all colors should be used.
};

// *****Note (from above) you will have to write
//          a function to do this.  I have not yet.
struct bmpPALETTE
{
	byte_t palPalette[1024]; // this will need to be
							 //improved if the program is to scale.
							 // unless we change the palette,
							 // this will do.
};

struct bmpBITMAP_FILE // note:  this structure may not be
					  // written to file all at once.
					  // the two headers may be written
					  // normally, but the image requires
					  // a write for each line followed
					  //  by a possible 1 - 3 padding bytes.
{
	bmpFILEHEADER fileheader;
	bmpINFOHEADER infoheader;
	bmpPALETTE palette; //this implementation
						// will not generalize.  Fixed at 256 shades of grey.
	byte_t **image_ptr; //this points to the
						// image. Allows the allocation of a two dimensional
						// array dynamically
};

//================= Open_input_file =======================
//
// Gets the name of the input file from the user and opens
// it for input
//

void open_input_file(
	ifstream &in_file //Pre:  none
					  //Post: File name supplied by user
);

//================ Assemble_Integer ========================
//
// Accepts a pointer to an array of unsigned characters
// (there should be 4 bytes)
// Assembles them into a signed integer and returns the
// result

int Assemble_Integer(
	unsigned char bytes[] // Pre: 4 little-endian bytes
						  // (least significant byte first)
);

//============= Display_FileHeader ==========================
//

void Display_FileHeader(bmpFILEHEADER &fileheader);

//============= Display_InfoHeader ==========================
//

void Display_InfoHeader(bmpINFOHEADER &infoheader);

//=============== Calc_Padding ==============================
//
// Returns the number of bytes of padding for an image
// (either 0,1,2,3).
// Each scan line must end on a 4 byte boundry.
// Threfore, if the pixel_width is not evenly divisible
// by 4, extra bytes are added (either 1, 2 or 3 extra
// bytes) when writing each line.  Likewise, when reading
// a bitmap file it may be helpful to remove the padding
// prior to any manipulations.
// This is not needed unless the number of bytes in a row
// are not evenly divisible by 4. See implementation
// section for details.
int Calc_Padding(int pixel_width); //pre: the width of the
								   //     image in pixels

//================= load_Bitmap_File =======================
//
void Load_Bitmap_File(
	bmpBITMAP_FILE &image, //Post: structure is filled with
						   //       data from a  .bmp file
	ifstream &fs_data);

//============== Display_Bitmap_File =======================
//
void Display_Bitmap_File(bmpBITMAP_FILE &image);

//================== Copy_Image ============================
//
// Pre: image_orig.byte_t points to a 2 dim array
//      image_copy does not have an array associated with
//      byte_t**
// Post: image_copy receives a copy of the structurs in
//       image_orig
void Copy_Image(bmpBITMAP_FILE &image_orig,
	bmpBITMAP_FILE &image_copy);

//================== Remove_Image ==========================
//
// Pre:  image.byte-t contains pointers to a 2-dim array
// Post:  memory that **byte_t points to is freed with the
//        delete operator image.bfType[] is set to "XX"
//
void Remove_Image(bmpBITMAP_FILE &image);

//================= Save_Bitmap_File =======================
//
void Save_Bitmap_File(bmpBITMAP_FILE &image, ofstream &fs_data);

//=================== Open_Output_File =====================
//
void Open_Output_File(ofstream &out_file);

//=================== Contrast =====================
//
void contrast(bmpBITMAP_FILE &image_c1, bmpBITMAP_FILE &image_c2);

//=================== Historgram =====================
//
void histogram(bmpBITMAP_FILE &image_c2, bmpBITMAP_FILE &image_c3);

//=================== Makin' it THICC =====================
//
void smooth(bmpBITMAP_FILE &image_c3, bmpBITMAP_FILE &image_c4);

//=================== Edges =====================
//
void edges(bmpBITMAP_FILE &image_c4, bmpBITMAP_FILE &image_c5);

//=================== Find the Box =====================
//
void white_out(bmpBITMAP_FILE &image_c5, bmpBITMAP_FILE &image_c6);

//=================== makin' it not THICC =====================
//
void thinning(bmpBITMAP_FILE &image_c5, bmpBITMAP_FILE &image_c6);

//=================== Creating a Hough space =====================
//
void hough(bmpBITMAP_FILE &image_c6, bmpBITMAP_FILE &image_c7, vector<coordinate> &line_pts);

// void draw_lines(bmpBITMAP_FILE &image_c7, bmpBITMAP_FILE &image_c8, vector<coordinate> &line_pts); dont think we need this
//========================== Draw Lines ==========================
//
//void Line(int x1, int y1, int x2, int y2, bmpBITMAP_FILE &image_c8);

//=================== Find Parallel Lines =====================
//
void find_parallel_lines(cv::Mat &image_c8, vector<Vec4i> &line_pts);

//======================  MAIN  ============================
//
int main(int argc, char **argv)
{
	bmpBITMAP_FILE orig_image;
	bmpBITMAP_FILE copy0;
	bmpBITMAP_FILE copy1;
	bmpBITMAP_FILE copy2;
	bmpBITMAP_FILE copy3;
	bmpBITMAP_FILE copy4;
	bmpBITMAP_FILE copy5;
	bmpBITMAP_FILE copy6;
	bmpBITMAP_FILE copy7;
	bmpBITMAP_FILE copy8;

	int c = 0;
	char *in_file_name = nullptr;
	char *output_file_name = nullptr;

	ifstream in_file;
	ofstream out_file;
	vector<coordinate> line_pts;

	// -i = the input bitmap file name
	// -o = the outfile bitmap file name
	// -t = threshhold for the egdes Function
	// -c = contrast multiplier
	for (int i = 1; i < argc; i++)
	{
		cout << "i: " << i << "   " << argv[i] << endl;
		if (!strcmp(argv[i], "-i"))
		{
			// cout << "i" << i << " " << argv[i] << " " << argv[i + 1] << endl;
			// strcpy(in_file_name, argv[i + 1]);

			in_file_name = argv[i + 1];
			cout << "in_file_name " << in_file_name << endl;
			in_file.open(in_file_name, ios::in | ios::binary);
			if (!in_file)
			{
				cerr << "Error opening file\a\a\n";
				exit(101);
			}
		}
		else if (!strcmp(argv[i], "-o"))
		{
			// strcpy(output_file_name, argv[i + 1]);

			output_file_name = argv[i + 1];

			out_file.open(output_file_name, ios::out | ios::binary);
			if (!out_file)
			{
				cout << "\nCannot open " << output_file_name << endl;
				exit(101);
			}
		}

		else if (!strcmp(argv[i], "-t"))
			THRESH_HOLD = atoi(argv[i + 1]);
		else if (!strcmp(argv[i], "-c"))
			CONTRAST = atoi(argv[i + 1]);
		else if (!strcmp(argv[i], "-s"))
			SMOOTH_LAYERS = atoi(argv[i + 1]);
		else if (!strcmp(argv[i], "-e"))
			EDGE_LAYERS = atoi(argv[i + 1]);
	}

	cout << "CONTRAST [" << CONTRAST << "]\n";
	cout << "THERES HOLD [" << THRESH_HOLD << "]\n";
	cout << "SMOOTH LAYERS [" << SMOOTH_LAYERS << "]\n";
	cout << "EDGE LAYERS [" << EDGE_LAYERS << "]\n";

	Load_Bitmap_File(orig_image, in_file);

	Display_FileHeader(orig_image.fileheader);
	Display_InfoHeader(orig_image.infoheader);
	//copies from orig_image to copy1

	int height = Assemble_Integer(orig_image.infoheader.biHeight);
	int width = Assemble_Integer(orig_image.infoheader.biWidth);
	printf("OG picture: HEIGHT [%d] x WIDTH [%d]", height, width);

	Copy_Image(orig_image, copy1);
	Copy_Image(orig_image, copy8);
	cout << endl
		<< "A copy of the file has been "
		<< "made in main memory.";

	/*
	 EDGE METHOD GOES HERE
	 */

	Remove_Image(orig_image); // frees dynamic memory too

	cout << endl
		<< "The original image has been "
		<< "removed from main memory.";

	cout << endl
		<< "To show that the copy starts as "
		<< "an exact copy of the original,";

	contrast(copy1, copy2);
	histogram(copy2, copy3);
	smooth(copy3, copy4);
	edges(copy4, copy5);
	white_out(copy5, copy0);
	thinning(copy0, copy6);

	cout << endl

		<< "Save the copy as a bitmap." << endl;
	Save_Bitmap_File(copy6, out_file);

	//------------------------------------------------
	// OpenCV functions
	cv::Mat OG_Image;
	cv::Mat Thinned_image;
	cv::Mat Hough_space;
	cv::Mat image_para_lines;
	cv::Mat final_image;
	std::string infile(in_file_name);
	std::string outfile(output_file_name);


	OG_Image = imread(infile, 0);
	Thinned_image = imread(outfile, 0);
	image_para_lines = OG_Image.clone();
	final_image = OG_Image.clone();

	for (int i = 0; i < Thinned_image.rows; i++)
	{
		for (int j = 0; j < Thinned_image.cols; j++)
		{
			Thinned_image.at<uchar>(i, j) = 255 - Thinned_image.at<uchar>(i, j);
		}
	}

	vector<Vec4i> lines;
	HoughLinesP(Thinned_image, lines, 1, CV_PI / 200, 80, 30, 20); // play with the last 3 vars to see if we get a better picture
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(image_para_lines, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 255), 2, 8);
	}

	find_parallel_lines(final_image, lines);

	namedWindow("Thinned", WINDOW_AUTOSIZE);
	imshow("Thinned", Thinned_image);

	namedWindow("Before Parallel", WINDOW_AUTOSIZE);
	imshow("Before Parallel", image_para_lines);

	namedWindow("Detected Lines", WINDOW_AUTOSIZE);
	imshow("Detected Lines", final_image);

	// ---------------------------------------------
	// this is commented out because the OpenCV will be doing the heavy lifting that
	// these functions cannot do at the moment
	// hough(copy6, copy7, line_pts);
	// draw_lines(copy7, copy8, line_pts);

	Remove_Image(copy0);
	Remove_Image(copy1);
	Remove_Image(copy2);
	Remove_Image(copy3);
	Remove_Image(copy4);
	Remove_Image(copy5);
	Remove_Image(copy6);
	//Remove_Image(copy7);
	//Remove_Image(copy8);

	waitKey(0);
	return 0;
}

//=============== END OF MAIN ==============================

//============== open_input_file ===========================
//

void open_input_file(
	ifstream &in_file)
{
	char in_file_name[80];

	cout << "Enter the name of the file" << endl
		<< "which contains the bitmap: ";
	cin >> in_file_name;

	//cout << "You entered: " << in_file_name << endl;

	in_file.open(in_file_name, ios::in | ios::binary);
	if (!in_file)
		cerr << "Error opening file\a\a\n", exit(101);

	return;
}

//================ Assemble_Integer ========================
//
int Assemble_Integer(unsigned char bytes[])
{
	int an_integer;

	an_integer =
		int(bytes[0]) + int(bytes[1]) * 256 + int(bytes[2]) * 256 * 256 + int(bytes[3]) * 256 * 256 * 256;
	return an_integer;
}

//============= Display_FileHeader =======================
//

void Display_FileHeader(bmpFILEHEADER &fileheader)
{
	cout << "bfType:          " << fileheader.bfType[0]
		<< fileheader.bfType[1]
		<< "\n";

	cout << "bfSize:          "
		<< Assemble_Integer(fileheader.bfSize) << "\n";
	cout << "bfReserved:      "
		<< Assemble_Integer(fileheader.bfReserved) << "\n";
	cout << "bfOffbits:     "
		<< Assemble_Integer(fileheader.bfOffbits) << "\n";
}

//================ Display_InfoHeader ======================
//

void Display_InfoHeader(bmpINFOHEADER &infoheader)
{
	cout << "\nThe bmpInfoHeader contains the following:\n";
	cout << "biSize:             "
		<< Assemble_Integer(infoheader.biSize) << "\n";
	cout << "biWidth:    "
		<< Assemble_Integer(infoheader.biWidth) << "\n";
	cout << "biHeight:        "
		<< Assemble_Integer(infoheader.biHeight) << "\n";
	cout << "biPlanes:    "
		<< int(infoheader.biPlanes[0]) +
		int(infoheader.biPlanes[1]) * 256
		<< "\n";

	cout << "biBitCount:    "
		<< int(infoheader.biBitCount[0]) +
		int(infoheader.biBitCount[1]) * 256
		<< "\n";
	cout << "biCompression:    "
		<< Assemble_Integer(infoheader.biCompression) << "\n";
	cout << "biSizeImage:    "
		<< Assemble_Integer(infoheader.biSizeImage) << "\n";
	cout << "biClrUsed:    "
		<< Assemble_Integer(infoheader.biClrUsed) << "\n";
	cout << "biClrImportant:  "
		<< Assemble_Integer(infoheader.biClrImportant) << "\n";
}

//==================== Calc_Padding ========================
//

int Calc_Padding(int pixel_width)
{
	// Each scan line must end on a 4 byte boundry.
	// Threfore, if the pixel_width is not evenly divisible
	// by 4, extra bytes are added (either 1 - 3 extra bytes)

	int remainder;
	int padding = -1;

	remainder = pixel_width % 4;
	//cout << "\nPixel width: " << pixel_width << "\n";
	//cout << "Remainder:     " << remainder << "\n";

	switch (remainder)
	{
	case 0:
		padding = 0;
		break;
	case 1:
		padding = 3;
		break;
	case 2:
		padding = 2;
		break;
	case 3:
		padding = 1;
		break;
	default:
		cerr << "Error:  Padding was set to "
			<< padding << "\n";
		exit(101);
	}

	//cout << "Padding determined: " << padding << "\n";

	return padding;
}

//================== load_Bitmap_File ======================
//
void Load_Bitmap_File(bmpBITMAP_FILE &image, ifstream &fs_data)
{
	int bitmap_width;
	int bitmap_height;

	int padding;
	long int cursor1; // used to navigate through the
					  // bitfiles

	// open_input_file(fs_data);

	fs_data.read((char *)&image.fileheader,
		sizeof(bmpFILEHEADER));
	//fs_data.seekg(14L);
	fs_data.read((char *)&image.infoheader,
		sizeof(bmpINFOHEADER));

	fs_data.read((char *)&image.palette,
		sizeof(bmpPALETTE)); // this will need to
							 // be dynamic, once
							 // the size of the palette can vary

	bitmap_height = Assemble_Integer(image.infoheader.biHeight);
	bitmap_width = Assemble_Integer(image.infoheader.biWidth);
	padding = Calc_Padding(bitmap_width);

	// allocate a 2 dim array
	image.image_ptr = new byte_t *[bitmap_height];
	for (int i = 0; i < bitmap_height; i++)
		image.image_ptr[i] = new byte_t[bitmap_width];

	cursor1 = Assemble_Integer(image.fileheader.bfOffbits);
	fs_data.seekg(cursor1); //move the cursor to the
							// beginning of the image data

	//load the bytes into the new array one line at a time
	for (int i = 0; i < bitmap_height; i++)
	{
		fs_data.read((char *)image.image_ptr[i],
			bitmap_width);
		// insert code here to read the padding,
		// if there is any
	}

	fs_data.close(); //close the file
					 // (consider replacing with a function w/error checking)
}

//============== Display_Bitmap_File =======================
//
void Display_Bitmap_File(bmpBITMAP_FILE &image)
{
	int bitmap_width;
	int bitmap_height;

	Display_FileHeader(image.fileheader);
	Display_InfoHeader(image.infoheader);

	//display the palatte here too, perhaps.

	bitmap_height = Assemble_Integer(image.infoheader.biHeight);
	bitmap_width = Assemble_Integer(image.infoheader.biWidth);

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < bitmap_width; j++)
			cout << setw(4) << int(image.image_ptr[i][j]);
		cout << "\n\nNew Line\n\n";
	}
}

//=============== Copy_Image ===============================
//
void Copy_Image(bmpBITMAP_FILE &image_orig,
	bmpBITMAP_FILE &image_copy)
{
	int height, width;

	image_copy.fileheader = image_orig.fileheader;
	image_copy.infoheader = image_orig.infoheader;
	image_copy.palette = image_orig.palette;

	height = Assemble_Integer(image_copy.infoheader.biHeight);
	width = Assemble_Integer(image_copy.infoheader.biWidth);

	image_copy.image_ptr = new byte_t *[height];

	for (int i = 0; i < height; i++)
		image_copy.image_ptr[i] = new byte_t[width];

	//load the bytes into the new array one byte at a time
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			image_copy.image_ptr[i][j] =
			image_orig.image_ptr[i][j];
	}
}

//================== Remove_Image ==========================
//
void Remove_Image(bmpBITMAP_FILE &image)
{
	int height, width;

	height = Assemble_Integer(image.infoheader.biHeight);
	width = Assemble_Integer(image.infoheader.biWidth);

	//once the palette is dynamic, must delete the memory
	// allocated for the palatte here too

	// delete the dynamic memory
	for (int i = 0; i < height; i++)
		delete[] image.image_ptr[i];
	delete[] image.image_ptr;

	image.fileheader.bfType[0] = 'X'; // just to mark it as
	image.fileheader.bfType[1] = 'X'; // unused.
									  // Also, we may wish to initialize all the header
									  // info to zero.
}

//================== Save_Bitmap_File ======================
//
void Save_Bitmap_File(bmpBITMAP_FILE &image, ofstream &fs_data)
{
	int width;
	int height;
	int padding;
	long int cursor1; // used to navigate through the
					  // bitfiles

	height = Assemble_Integer(image.infoheader.biHeight);
	width = Assemble_Integer(image.infoheader.biWidth);

	// Open_Output_File(fs_data);

	fs_data.write((char *)&image.fileheader,
		sizeof(bmpFILEHEADER));
	if (!fs_data.good())
	{
		cout << "\aError 101 writing bitmapfileheader";
		cout << " to file.\n";
		exit(101);
	}

	fs_data.write((char *)&image.infoheader,
		sizeof(bmpINFOHEADER));
	if (!fs_data.good())
	{
		cout << "\aError 102 writing bitmap";
		cout << " infoheader to file.\n";
		exit(102);
	}

	fs_data.write((char *)&image.palette,
		sizeof(bmpPALETTE));
	if (!fs_data.good())
	{
		cout << "\aError 103 writing bitmap palette to";
		cout << "file.\n";
		exit(103);
	}
	//this loop writes the image data
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			fs_data.write((char *)&image.image_ptr[i][j],
				sizeof(byte_t));
			if (!fs_data.good())
			{
				cout << "\aError 104 writing bitmap data";
				cout << "to file.\n";
				exit(104);
			}
		}
	}

	fs_data.close();
}

//================== Open_Output_File ===================
//
void Open_Output_File(ofstream &out_file)
{
	char out_file_name[80];

	cout << "Save file as: ";
	cin >> out_file_name;

	out_file.open(out_file_name, ios::out | ios::binary);
	if (!out_file)
	{
		cout << "\nCannot open " << out_file_name << endl;
		exit(101);
	}

	return;
}

void contrast(bmpBITMAP_FILE &image_c1, bmpBITMAP_FILE &image_c2)
{
	int height, width;

	image_c2.fileheader = image_c1.fileheader;
	image_c2.infoheader = image_c1.infoheader;
	image_c2.palette = image_c1.palette;

	height = Assemble_Integer(image_c1.infoheader.biHeight);
	width = Assemble_Integer(image_c1.infoheader.biWidth);

	image_c2.image_ptr = new byte_t *[height];

	for (int i = 0; i < height; i++)
		image_c2.image_ptr[i] = new byte_t[width];

	//load the bytes into the new array one byte at a time
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{

			image_c2.image_ptr[i][j] = 127 + CONTRAST * (image_c1.image_ptr[i][j] - 127);

			if (image_c2.image_ptr[i][j] > 255)
				image_c2.image_ptr[i][j] = 255;
			if (image_c2.image_ptr[i][j] < 0)
				image_c2.image_ptr[i][j] = 0;
		}
	}
}

void histogram(bmpBITMAP_FILE &image_c2, bmpBITMAP_FILE &image_c3)
{
	int height, width;
	int H[256];

	for (int i = 0; i < 256; ++i)
	{
		H[i] = 0;
	}

	image_c3.fileheader = image_c2.fileheader;
	image_c3.infoheader = image_c2.infoheader;
	image_c3.palette = image_c2.palette;

	height = Assemble_Integer(image_c2.infoheader.biHeight);
	width = Assemble_Integer(image_c2.infoheader.biWidth);

	image_c3.image_ptr = new byte_t *[height];

	for (int i = 0; i < height; i++)
		image_c3.image_ptr[i] = new byte_t[width];

	//Build Hist
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			H[image_c2.image_ptr[i][j]] += 1;
	}

	//Build cumulative hist
	for (int i = 1; i < 256; ++i)
	{
		H[i] = H[i - 1] + H[i];
	}

	//Normalize cumulative hist
	for (int i = 0; i < 256; ++i)
	{
		H[i] = 256 * H[i] / (height * width);
	}

	//make new image
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			image_c3.image_ptr[i][j] = H[image_c2.image_ptr[i][j]];
	}
}

void smooth(bmpBITMAP_FILE &image_c3, bmpBITMAP_FILE &image_c4)
{
	int height, width, average;
	int number_of_pixels = pow((2 * SMOOTH_LAYERS + 1), 2) - 1;
	average = 0;

	image_c4.fileheader = image_c3.fileheader;
	image_c4.infoheader = image_c3.infoheader;
	image_c4.palette = image_c3.palette;

	height = Assemble_Integer(image_c3.infoheader.biHeight);
	width = Assemble_Integer(image_c3.infoheader.biWidth);

	image_c4.image_ptr = new byte_t *[height];

	for (int i = 0; i < height; i++)
		image_c4.image_ptr[i] = new byte_t[width];

	for (int i = 0; i < height; i++)
		image_c4.image_ptr[i][0] = image_c3.image_ptr[i][0];
	for (int i = 0; i < height; i++)
		image_c4.image_ptr[i][width - 1] = image_c3.image_ptr[i][width - 1];
	for (int i = 0; i < width; i++)
		image_c4.image_ptr[0][i] = image_c3.image_ptr[0][i];
	for (int i = 0; i < width; i++)
		image_c4.image_ptr[height - 1][i] = image_c3.image_ptr[height - 1][i];

	// Loop through the whole histogram
	for (int i = SMOOTH_LAYERS; i < height - SMOOTH_LAYERS; ++i)
	{
		for (int j = SMOOTH_LAYERS; j < width - SMOOTH_LAYERS; ++j)
		{
			int total = 0;
			// Loop through the pixels surrounding the current pixel being looked at
			for (int a = -SMOOTH_LAYERS; a <= SMOOTH_LAYERS; a++)
			{
				for (int b = -SMOOTH_LAYERS; b <= SMOOTH_LAYERS; b++)
				{
					if (a == 0 && b == 0)
					{
						continue;
					}
					else
					{
						total += image_c3.image_ptr[i + a][j + b];
					}
				}
			}
			average = total / number_of_pixels;

			image_c4.image_ptr[i][j] = average;
		}
	}
}

void edges(bmpBITMAP_FILE &image_c4, bmpBITMAP_FILE &image_c5)
{
	int height, width, above, below, left, right;
	int number_of_pixels = pow((2 * EDGE_LAYERS + 1), 2) - 1;

	image_c5.fileheader = image_c4.fileheader;
	image_c5.infoheader = image_c4.infoheader;
	image_c5.palette = image_c4.palette;

	height = Assemble_Integer(image_c4.infoheader.biHeight);
	width = Assemble_Integer(image_c4.infoheader.biWidth);

	image_c5.image_ptr = new byte_t *[height];

	for (int i = 0; i < height; i++)
		image_c5.image_ptr[i] = new byte_t[width];

	for (int i = 0; i < height; i++)
		image_c5.image_ptr[i][0] = 255;
	for (int i = 0; i < height; i++)
		image_c5.image_ptr[i][width - 1] = 255;
	for (int i = 0; i < width; i++)
		image_c5.image_ptr[0][i] = 255;
	for (int i = 0; i < width; i++)
		image_c5.image_ptr[height - 1][i] = 255;

	// Loop through the whole histogram
	for (int i = EDGE_LAYERS; i < height - EDGE_LAYERS; ++i)
	{
		for (int j = EDGE_LAYERS; j < width - EDGE_LAYERS; ++j)
		{
			right = 0;
			left = 0;
			above = 0;
			below = 0;
			for (int a = -EDGE_LAYERS; a <= EDGE_LAYERS; a++)
			{
				for (int b = -EDGE_LAYERS; b <= EDGE_LAYERS; b++)
				{
					if (b < 0)
					{
						left += image_c4.image_ptr[i + a][j + b];
						above += image_c4.image_ptr[i + b][j + a];
					}
					else if (b > 0)
					{
						right += image_c4.image_ptr[i + a][j + b];
						below += image_c4.image_ptr[i + b][j + a];
					}
				}
			}

			if (abs(left - right) < THRESH_HOLD && abs(above - below) < THRESH_HOLD)
			{
				image_c5.image_ptr[i][j] = 255;
			}
			else
			{
				image_c5.image_ptr[i][j] = 0;
			}
		}
	}
}

// https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm
void thinning(bmpBITMAP_FILE &image_c5, bmpBITMAP_FILE &image_c6)
{
	Copy_Image(image_c5, image_c6);

	int height = Assemble_Integer(image_c6.infoheader.biHeight);
	int width = Assemble_Integer(image_c6.infoheader.biWidth);
	printf("\nTHINNING: HEIGHT [%d] x WIDTH [%d]", height, width);

	int changed_pixels = 0;

	do
	{
		changed_pixels = 0;
		// step 1
		for (int y = 1; y < width - 1; y++)
		{
			for (int x = 1; x < height - 1; x++)
			{

				/* The varaibles below are representing a windows of pixels like:
					p9 p2 p3
					p8 p1 p4
					p7 p6 p5
				*/
				int p2 = image_c6.image_ptr[x - 1][y];
				int p3 = image_c6.image_ptr[x - 1][y + 1];
				int p4 = image_c6.image_ptr[x][y + 1];
				int p5 = image_c6.image_ptr[x + 1][y + 1];
				int p6 = image_c6.image_ptr[x + 1][y];
				int p7 = image_c6.image_ptr[x + 1][y - 1];
				int p8 = image_c6.image_ptr[x][y - 1];
				int p9 = image_c6.image_ptr[x - 1][y - 1];
				int num_of_black_pixel = 0;
				int transitions = 0;

				if (p2 == 0)
					num_of_black_pixel++;
				if (p3 == 0)
					num_of_black_pixel++;
				if (p4 == 0)
					num_of_black_pixel++;
				if (p5 == 0)
					num_of_black_pixel++;
				if (p6 == 0)
					num_of_black_pixel++;
				if (p7 == 0)
					num_of_black_pixel++;
				if (p8 == 0)
					num_of_black_pixel++;
				if (p9 == 0)
					num_of_black_pixel++;

				if (p9 == 255 && p2 == 0)
					transitions++;
				if (p2 == 255 && p3 == 0)
					transitions++;
				if (p3 == 255 && p4 == 0)
					transitions++;
				if (p4 == 255 && p5 == 0)
					transitions++;
				if (p5 == 255 && p6 == 0)
					transitions++;
				if (p6 == 255 && p7 == 0)
					transitions++;
				if (p7 == 255 && p8 == 0)
					transitions++;
				if (p8 == 255 && p9 == 0)
					transitions++;

				if (image_c6.image_ptr[x][y] == 0)
				{
					// cout << "here 1" << endl;
					if (num_of_black_pixel >= 2 && num_of_black_pixel <= 6)
					{
						// cout << "here 2" << endl;

						if (transitions == 1)
						{
							// cout << "here 3" << endl;

							if ((p2 == 255 || p4 == 255 || p6 == 255) &&
								(p4 == 255 || p6 == 255 || p8 == 255))
							{
								// cout << "------------------------------------" << endl;
								image_c6.image_ptr[x][y] = 255;
								++changed_pixels;
							}
						}
					}
				}
			}
		}

		// Step 2
		for (int y = 1; y < width - 1; y++)
		{
			for (int x = 1; x < height - 1; x++)
			{

				/* The varaibles below are representing a windows of pixels like:
					p9 p2 p3
					p8 p1 p4
					p7 p6 p5
				*/
				int p2 = image_c6.image_ptr[x - 1][y];
				int p3 = image_c6.image_ptr[x - 1][y + 1];
				int p4 = image_c6.image_ptr[x][y + 1];
				int p5 = image_c6.image_ptr[x + 1][y + 1];
				int p6 = image_c6.image_ptr[x + 1][y];
				int p7 = image_c6.image_ptr[x + 1][y - 1];
				int p8 = image_c6.image_ptr[x][y - 1];
				int p9 = image_c6.image_ptr[x - 1][y - 1];
				int num_of_black_pixel = 0;
				int transitions = 0;

				if (p2 == 0)
					num_of_black_pixel++;
				if (p3 == 0)
					num_of_black_pixel++;
				if (p4 == 0)
					num_of_black_pixel++;
				if (p5 == 0)
					num_of_black_pixel++;
				if (p6 == 0)
					num_of_black_pixel++;
				if (p7 == 0)
					num_of_black_pixel++;
				if (p8 == 0)
					num_of_black_pixel++;
				if (p9 == 0)
					num_of_black_pixel++;

				if (p9 == 255 && p2 == 0)
					transitions++;
				if (p2 == 255 && p3 == 0)
					transitions++;
				if (p3 == 255 && p4 == 0)
					transitions++;
				if (p4 == 255 && p5 == 0)
					transitions++;
				if (p5 == 255 && p6 == 0)
					transitions++;
				if (p6 == 255 && p7 == 0)
					transitions++;
				if (p7 == 255 && p8 == 0)
					transitions++;
				if (p8 == 255 && p9 == 0)
					transitions++;

				if (image_c6.image_ptr[x][y] == 0 &&
					(num_of_black_pixel >= 2 && num_of_black_pixel <= 6) &&
					transitions == 1 &&
					(p2 == 255 || p4 == 255 || p8 == 255) &&
					(p2 == 255 || p6 == 255 || p8 == 255))
				{
					image_c6.image_ptr[x][y] = 255;
					++changed_pixels;
				}
			}
		}
	} while (changed_pixels > 0);
}

void hough(bmpBITMAP_FILE &image_c6, bmpBITMAP_FILE &image_c7, vector<coordinate> &line_pts)
{
	Copy_Image(image_c6, image_c7);

	int height = Assemble_Integer(image_c7.infoheader.biHeight);
	int width = Assemble_Integer(image_c7.infoheader.biWidth);
	printf("\nHough: HEIGHT [%d] x WIDTH [%d]", height, width);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			image_c7.image_ptr[i][j] = 255;
	}
	int height_half = height / 2;

	float radius_max = hypot(width, height);
	float delta_radius = radius_max / height_half;
	float delta_theta = M_PI / width;
	for (int x = 0; x < height; x++)
	{
		for (int y = 0; y < width; y++)
		{
			int pixel = image_c6.image_ptr[x][y];
			if (pixel == 255)
			{
				continue;
			}
			for (int x_1 = 0; x_1 < (width - 1); x_1++)
			{
				float theta = delta_theta * x_1;

				// Once we are able to get lines drawn of the image. if they dont match up with the
				// box, switch the sin and cos around below this comment
				float radius = (x * sin(theta)) + (y * cos(theta));
				int y_1 = (height / 2) + int((radius / delta_radius) + .5);
				// cout << "x_1: " << x_1 << " y_1: " << y_1 << endl;
				if (image_c7.image_ptr[y_1][x_1] > 0)
				{
					if (image_c7.image_ptr[y_1][x_1] == 50)
					{
						coordinate temp_coordinate(theta, radius);
						line_pts.push_back(temp_coordinate);
					}
					image_c7.image_ptr[y_1][x_1] -= 1;
				}
			}
		}
	}
}

void white_out(bmpBITMAP_FILE &org, bmpBITMAP_FILE &white)
{
	int p_in = 100;

	Copy_Image(org, white);

	int height = Assemble_Integer(org.infoheader.biHeight);
	int width = Assemble_Integer(org.infoheader.biWidth);

	// left of image
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < p_in; j++)
		{
			white.image_ptr[i][j] = 255;
		}
	}
	// right of image
	for (int i = 0; i < height; i++)
	{
		for (int j = 1; j < p_in; j++)
		{
			white.image_ptr[i][width - j] = 255;
		}
	}
	// top of image
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < p_in; j++)
		{
			white.image_ptr[j][i] = 255;
		}
	}
	// bottom of image
	for (int i = 0; i < width; i++)
	{
		for (int j = 1; j < p_in; j++)
		{
			white.image_ptr[height - j][i] = 255;
		}
	}
}


void find_parallel_lines(cv::Mat &image_c8, vector<Vec4i> &line_pts)
{
	int slope;
	float slope_threshold = 1.5;
	size_t line_num = line_pts.size();
	float * line_slopes = new float[line_num];
	float * same_slopes = new float[line_num];

	// find slopes
	for (int i = 0; i < line_num; i++)
	{
		if (line_pts[i][2] - line_pts[i][0] != 0)
		{
			line_slopes[i] = (line_pts[i][3] - line_pts[i][1]) / (line_pts[i][2] - line_pts[i][0]);
		}
		else
		{
			line_slopes[i] = 100000; // really it does not exist
		}
	}
	// find parallel lines
	for (int i = 0; i < line_num - 1; i++)
	{
		for (int j = 0; j < line_num - 1; j++)
		{
			if (i == j)
			{
				continue;
			}
			else if (abs(line_slopes[i] - line_slopes[j]) < slope_threshold) // thresh hold for the parallel lines
			{
				same_slopes[i] = line_slopes[j];
			}
			else
			{
				same_slopes[i] = -999999999;
			}
		}
	}
	for (int i = 0; i < line_num - 1; i++)
	{
		for (int j = 0; j < line_num - 1; j++)
		{
			if (i == j)
			{
				continue;
			}
			else if ((abs(same_slopes[i] - same_slopes[j]) < slope_threshold) && same_slopes[j] != -999999999) // thresh hold for the parallel lines
			{
				//draw parallel lines
				cv::line(image_c8, Point(line_pts[j][0], line_pts[j][1]), Point(line_pts[j][2], line_pts[j][3]), Scalar(255, 255, 255), 2, 8);
			}
		}
	}
	delete[] same_slopes;
	delete[] line_slopes;
}
