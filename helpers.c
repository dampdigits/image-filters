#include "helpers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Function declaration
void makecopy(int height, int width, RGBTRIPLE copy[height][width], RGBTRIPLE image[height][width]);
int compute(int gx, int gy);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // Traverse through each pixel and modify
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Find average pixel value
            BYTE average = round((image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0);
            // Set red, green and blue to average
            image[i][j].rgbtBlue = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtRed = average;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // Traverse through one half of image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            // Swap pixels on the right with the pixels on the left
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - j - 1];
            image[i][width - j - 1] = temp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Make copy of image
    RGBTRIPLE(*copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (copy == NULL)
    {
        printf("Not enough memory to create blurred image.\n");
        return;
    }
    makecopy(height, width, copy, image);

    // Change image pixel to average RGB value of surrounding pixels in copy
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float red = 0, green = 0, blue = 0;
            float pixel_count = 0;
            // Sum of Red, Green and Blue of grid in copy
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    // Check for borders
                    if (k + i - 1 < 0 || k + i - 1 > height - 1 || l + j - 1 < 0 || l + j - 1 > width - 1)
                    {
                        continue;
                    }

                    red += copy[k + i - 1][l + j - 1].rgbtRed;
                    green += copy[k + i - 1][l + j - 1].rgbtGreen;
                    blue += copy[k + i - 1][l + j - 1].rgbtBlue;
                    // Count pixels in grid
                    pixel_count += 1;
                }
            }
            // Alter image pixel with average RGB value
            image[i][j].rgbtRed = round(red / pixel_count);
            image[i][j].rgbtGreen = round(green / pixel_count);
            image[i][j].rgbtBlue = round(blue / pixel_count);
        }
    }
    free(copy);
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Make copy of image
    RGBTRIPLE(*copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (copy == NULL)
    {
        printf("Not enough memory to create blurred image.\n");
        return;
    }
    makecopy(height, width, copy, image);

    // Create convolutional matrices
    int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    // Traverse through pixels of copy
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int gxred = 0, gyred = 0, gxgreen = 0, gygreen = 0, gxblue = 0, gyblue = 0;
            // Traverse through Gx & Gy matrix and surrounding pixels
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    // Check for borders
                    if (k + i - 1 < 0 || k + i - 1 > height - 1 || l + j - 1 < 0 || l + j - 1 > width - 1)
                    {
                        continue;
                    }
                    // Compute Gx and Gy for each channel of red, green and blue
                    gxred += (copy[k + i - 1][l + j - 1].rgbtRed * gx[k][l]);
                    gyred += (copy[k + i - 1][l + j - 1].rgbtRed * gy[k][l]);

                    gxgreen += (copy[k + i - 1][l + j - 1].rgbtGreen * gx[k][l]);
                    gygreen += (copy[k + i - 1][l + j - 1].rgbtGreen * gy[k][l]);

                    gxblue += (copy[k + i - 1][l + j - 1].rgbtBlue * gx[k][l]);
                    gyblue += (copy[k + i - 1][l + j - 1].rgbtBlue * gy[k][l]);
                }
            }
            // Alter pixel of image with RGB value of Gx and Gy matrices
            image[i][j].rgbtRed = compute(gxred, gyred);
            image[i][j].rgbtGreen = compute(gxgreen, gygreen);
            image[i][j].rgbtBlue = compute(gxblue, gyblue);
        }
    }
    free(copy);
    return;
}

// Make copy of image
void makecopy(int height, int width, RGBTRIPLE copy[height][width], RGBTRIPLE image[height][width])
{
    // Copy pixels
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            copy[i][j] = image[i][j];
        }
    }
}
// Compute new channel value
int compute(int gx, int gy)
{
    int value = round(sqrt(gx * gx + gy * gy));
    // Cap value to 255 if exceeds
    if (value > 255)
    {
        value = 255;
    }
    return value;
}
