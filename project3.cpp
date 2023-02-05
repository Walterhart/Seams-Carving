/*
 * project3.cpp
 *
 *  This program perform seams carving on a pgm image
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
using namespace std;

// Rotate the image with 90 degree
void rotate90Degree(int &cols, int &rows, char *(&image), int *(&createdImage))
{
    int rotatedC = 0, rotated_r = 0;
    char *imageCopy = new char[cols * rows];
    int *dataCopy = new int[cols * rows];
    for (int i = 0; i < cols * rows; i++)
    {
        imageCopy[i] = image[i];
        dataCopy[i] = createdImage[i];
    }
    for (int c = 0; c < cols; c++)
    {
        for (int r = rows - 1; r >= 0; r--)
        {
            // Swap data of image
            image[rotatedC + rotated_r * rows] = imageCopy[c + r * cols];
            createdImage[rotatedC + rotated_r * rows] = dataCopy[c + r * cols];
            rotatedC++;
        }
        rotated_r++;
        rotatedC = 0;
    }
    int temp = cols;
    cols = rows;
    rows = temp;

    delete imageCopy;
    delete dataCopy;
}

// Rotate 270 degree the image. Rotate 3 times 90 degree

void rotate270Degree(int &cols, int &rows, char *(&image), int *(&energy))
{
    for (int i = 0; i < 3; i++)
        rotate90Degree(cols, rows, image, energy);
}

// Convert the char position to the integer number. Using for mapping
int convertCharIntoInt(char c)
{
    if (int(c) < 0)
        return 256 + int(c);
    else
        return int(c);
}

// Build the  image from the PGM image

int* buildImage(int cols, int rows, char image[])
{
    int *result = new int[cols * rows];
    int value = 0;
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            // Calculate the value of this pixel
            if (c - 1 >= 0)
                value += abs(convertCharIntoInt(image[c + r * cols]) - convertCharIntoInt(image[c - 1 + r * cols]));
            if (c + 1 < cols)
                value += abs(convertCharIntoInt(image[c + r * cols]) - convertCharIntoInt(image[c + 1 + r * cols]));
            if (r - 1 >= 0)
                value += abs(convertCharIntoInt(image[c + r * cols]) - convertCharIntoInt(image[c + (r - 1) * cols]));
            if (r + 1 < rows)
                value += abs(convertCharIntoInt(image[c + r * cols]) - convertCharIntoInt(image[c + (r + 1) * cols]));
            result[c + r * cols] = value;
            value = 0;
        }
    }
    return result;
}

// Try to cut the image with min and max value
void cutImage(int &cols, int &rows, char *(&image), int *(&energy))
{
    if (cols == 1 || cols == 0)
    {
        cout << "Error: columns too small. We can not do anything." << endl;
        return;
    }
    int *cumulative = new int[cols * rows];
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            // top
            if (r == 0)
                cumulative[c + r * cols] = energy[c + r * cols];
            else
            {
                // left
                if (c - 1 == -1)
                    cumulative[c + r * cols] = energy[c + r * cols]
                            + min(cumulative[c + (r - 1) * cols], cumulative[c + 1 + (r - 1) * cols]);
                // right
                else if (c + 1 == cols)
                    cumulative[c + r * cols] = energy[c + r * cols]
                            + min(cumulative[c - 1 + (r - 1) * cols], cumulative[c + (r - 1) * cols]);
                // middle
                else
                    cumulative[c + r * cols] = energy[c + r * cols]
                            + min(min(cumulative[c - 1 + (r - 1) * cols], cumulative[c + (r - 1) * cols]),
                                    cumulative[c + 1 + (r - 1) * cols]);
            }
        }
    }
    int inds[rows];
    // Find the min index
    int indMin = (rows - 1) * cols;
    for (int c = 0; c < cols; c++)
    {
        if (cumulative[c + (rows - 1) * cols] < cumulative[indMin])
            indMin = c + (rows - 1) * cols;
    }
    inds[0] = indMin;
    // Find the min value
    int MinVal, s = 1;
    for (int r = (rows - 2); r >= 0; r--)
    {
        if (indMin % cols == 0)
        {
            MinVal = min(cumulative[indMin - cols], cumulative[indMin - cols + 1]);
            if (cumulative[indMin - cols] == MinVal)
                indMin = indMin - cols;
            else
                indMin = indMin - cols + 1;
        }
        else if ((indMin + 1) % cols == 0)
        {
            MinVal = min(cumulative[indMin - cols], cumulative[indMin - cols - 1]);
            if (cumulative[indMin - cols - 1] == MinVal)
                indMin = indMin - cols - 1;
            else
                indMin = indMin - cols;
        }
        else
        {
            MinVal = min(min(cumulative[indMin - cols - 1], cumulative[indMin - cols]), cumulative[indMin - cols + 1]);
            if (cumulative[indMin - cols - 1] == MinVal)
                indMin = indMin - cols - 1;
            else if (cumulative[indMin - cols] == MinVal)
                indMin = indMin - cols;
            else
                indMin = indMin - cols + 1;
        }
        inds[s] = indMin;
        s++;
    }
    // Create the new image and copy data
    char *newImage = new char[(cols - 1) * rows];
    int c = 0;
    for (int i = 0; i < cols * rows; i++)
    {
        bool add_bool = true;
        for (int s = 0; s < (rows); s++)
        {
            if (inds[s] == i)
            {
                add_bool = false;
                break;
            }
        }
        if (add_bool)
        {
            newImage[c] = image[i];
            c++;
        }
    }
    cols += -1;
    delete image;
    image = newImage;
}

// Perform seam carving
void seamCarving(int &cols, int &rows, char *(&image), int *(&createdImage))
{

    rotate270Degree(cols, rows, image, createdImage);
    cutImage(cols, rows, image, createdImage);
    rotate90Degree(cols, rows, image, createdImage);
}
int main(int argc, char **argv)
{
    // Check the argument
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <image.pgm> <vertical> <horizontal>" << endl;
        return 1;
    }
    string image_filename = argv[1];
    string line;
    int vertical, horizontal;
    int cols = 0, rows = 0, gray_value_max = 0;
    vertical = atoi(argv[2]);
    horizontal = atoi(argv[3]);
    ifstream imageInput;
    ofstream imageOutput;

    imageInput.open(image_filename);
    // Open the input file.
    if (!imageInput.is_open())
    {
        cout << "Can not open the input file for process. Please check. Exiting..." << endl;
        return 1;
    }
    //Ignore the header line.
    getline(imageInput, line);
    getline(imageInput, line);
    // Read the column and row variable.
    imageInput >> cols;
    imageInput >> rows;
    // Read the gray value
    imageInput >> gray_value_max;
    char *imageData = new char[cols * rows];
    int value;
    // Read the image data
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            imageInput >> value;
            if (value > gray_value_max)
            {
                cout << "Invalid gray value. Look like PGM file error. Exiting...." << endl;
                return 0;
            }
            imageData[cols * r + c] = value;
        }
    }
    // Base on the vertical. Build the  image and create a new image
    for (int v = 0; v < vertical; v++)
    {
        int *createdImage = buildImage(cols, rows, imageData);
        cutImage(cols, rows, imageData, createdImage);
        delete createdImage;
    }

    // Base on the horizontal. Build the image and seam carving image
    for (int h = 0; h < horizontal; h++)
    {
        int *createdImage = buildImage(cols, rows, imageData);
        seamCarving(cols, rows, imageData, createdImage);
        delete createdImage;
    }
    // Write the output file.
    imageOutput.open(image_filename.substr(0, image_filename.size() - 4) + "_processed.pgm");
    imageOutput << "P2\n# Created by IrfanView\n" << cols << " " << rows << "\n" << gray_value_max << "\n";
    for (int r(0); r < rows; r++)
    {
        for (int c(0); c < cols; c++)
        {
            imageOutput << convertCharIntoInt(imageData[c + r * cols]);
            if (c != cols - 1)
                imageOutput << " ";
        }
        if (r != rows - 1)
            imageOutput << " \n";
    }
    imageOutput << " \n";
    imageOutput.close();
    delete imageData;
    return 0;

}
