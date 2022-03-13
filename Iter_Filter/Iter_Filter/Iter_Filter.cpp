#include <iostream>
#include <string>
#include <Windows.h>
#include <iomanip>
#include <vector>
#include <fstream>
#include "lodepng.h"

typedef unsigned char uchar;
typedef unsigned int uint;

using namespace std;
using namespace lodepng;
const int n = 11;
int n_2 = n / 2;

int** ExtensionMatrix()
{
    int** matr = new int* [n];
    for (int i = 0; i < n; i++)
        matr[i] = new int[n];

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matr[i][j] = 0;

    for (int i = 0; i < n; i++)
    {
        matr[n / 2][i] = 1;
        matr[i][n / 2] = 1;
    }
    return matr;
}

//int ToFilter(int x, int y, vector<uchar> image, uint Rwidth, uint Rheight, int** exMatr, int color_int)
//{
//    int color = 0;
//    for (int i = 0; i < n; i++)
//        for (int j = 0; j < n; j++)
//            color += image[((y + n_2 - i) * Rheight + x + n_2 - j) * 3 + color_int] * exMatr[i][j];
//    int div = 2 * n - 1;
//    return color / div;
//}

vector<uchar> IncreaseImage(string inputfilename, uint &Rwidth, uint &Rheight)
{
    vector<uchar> image;
    uint width, height;
    decode(image, width, height, inputfilename, LCT_RGB);
    
    Rwidth = width + n_2 * 2, Rheight = height + n_2 * 2;
    vector<uchar> res(Rwidth * Rheight * 3, 0);

    for (int i = 0; i < width; i++)
        for (int j = 0; j < n_2; j++)
        {
            // Заполняем левый дополнительный участок
            int left_r = ((i + n_2) * Rheight + j) * 3;
            int left_i = (i * height + 0) * 3;
            for (int k = 0; k < 3; k++)
                res[left_r + k] = image[left_i + k];
            // Заполняем верхний дополнительный участок
            int top_r = (j * Rheight + i + n_2) * 3;
            int top_i = (0 * height + i) * 3;
            for (int k = 0; k < 3; k++)
                res[top_r + k] = image[top_i + k];
            // Заполняем нижний дополнительный участок
            int bottom_r = ((Rwidth - 1 - j) * Rheight + i + n_2) * 3;
            int bottom_i = ((width - 1) * height + i) * 3;
            for (int k = 0; k < 3; k++)
                res[bottom_r + k] = image[bottom_i + k];
            // Заполняем правый дополнительный участок
            int right_r = ((i + n_2) * Rheight + Rwidth - 1 - j) * 3;
            int right_i = (i * height + height - 1) * 3;
            for (int k = 0; k < 3; k++)
                res[right_r + k] = image[right_i + k];
        }

    // Заполняем центр
    for (int i = 0; i < width; i++)
        for (int j = 0; j < width; j++)
        {
            int r = ((i + n_2) * Rheight + j + n_2) * 3;
            int im = (i * height + j) * 3;
            for (int k = 0; k < 3; k++)
                res[r + k] = image[im + k];
        }

    for (int i = 0; i < n_2; i++)
        for (int j = 0; j < n_2; j++)
        {
            // Заполняем верхний левый угол
            int top_left_r = (i * Rheight + j) * 3;
            int top_left_i = (0 * height + 0) * 3;
            for (int k = 0; k < 3; k++)
                res[top_left_r + k] = image[top_left_i + k];
            // Заполняем нижний левый угол
            int bottom_left_r = ((Rwidth - 1 - i) * Rheight + j) * 3;
            int bottom_left_i = ((height - 1) * height + 0) * 3;
            for (int k = 0; k < 3; k++)
                res[bottom_left_r + k] = image[bottom_left_i + k];
            // Заполняем верхний правый угол
            int top_right_r = (i * Rheight + Rwidth - 1 - j) * 3;
            int top_right_i = (0 * height + height - 1) * 3;
            for (int k = 0; k < 3; k++)
                res[top_right_r + k] = image[top_right_i + k];
            // Заполняем нижний правый угол
            int bottom_right_r = ((Rwidth - 1 - i) * Rheight + Rwidth - 1 - j) * 3;
            int bottom_right_i = ((width - 1) * height + height - 1) * 3;
            for (int k = 0; k < 3; k++)
                res[bottom_right_r + k] = image[bottom_right_i + k];
        }
    return res;
}


vector<uchar> FilterExtensionMatrix(vector<uchar> image, uint Rwidth, uint Rheight, int** exMatr)
{
    vector<uchar> res ((Rwidth - n_2 * 2) * (Rheight - n_2 * 2) * 3, 0);
    for (int i = n_2; i < Rwidth - n_2; i++)
        for (int j = n_2; j < Rwidth - n_2; j++)
        {
            int ind = ((i - n_2) * (Rheight - n_2 * 2) + j - n_2) * 3;
            for (int k = 0; k < 3; k++)
            {
                int color = 0;
                for (int ii = 0; ii < n; ii++)
                    for (int jj = 0; jj < n; jj++)
                        color += image[((i + n_2 - ii) * Rheight + j + n_2 - jj) * 3 + k] * exMatr[ii][jj];
                int div = 2 * n - 1;
                res[ind + k] = color / div;
            }
                /*res[ind + k] = ToFilter(j, i, image, Rwidth, Rheight, exMatr, k);*/
        }
            
    return res;
}

int main()
{
    int** exMatr = ExtensionMatrix();
    vector<double> time_;

    for (int i = 4; i <= 512; i *= 2)
    {
        /*clock_t tStart = clock();*/
        string input = "C:\\tmp\\Images\\" + std::to_string(i) + 'x' + std::to_string(i) + ".png";
        string output = std::to_string(i) + 'x' + std::to_string(i) + "_new.png";

        uint Rwidth, Rheight;
        vector<uchar> increasedImage = IncreaseImage(input, Rwidth, Rheight);

        clock_t tStart = clock();

        vector<uchar> result = FilterExtensionMatrix(increasedImage, Rwidth, Rheight, exMatr);

        double time = (clock() - tStart) / double(CLOCKS_PER_SEC);
        cout << "Iter algorithm finished. Image: " << to_string(i) << "x" << to_string(i) << " Time: " << setprecision(6) << time << endl;
        time_.push_back(time);

        encode(output, result, Rwidth - 2 * n_2, Rheight - 2 * n_2, LCT_RGB);
    }

    ofstream fTime("time.txt");
    for (int i = 0; i < time_.size(); i++)
        fTime << "Image: " << to_string(pow(2, i + 2)) << "x" << to_string(pow(2, i + 2)) << " Time: " << setprecision(6) << time_[i] << endl;
    fTime.close();
}