
	/*
	** This program reads bridge.ppm, a 512 x 512 PPM image.
	** It smooths it using a standard 7x7 mean filter, 7x1 and 1x7 separable filters as well as using sliding windows.
	**
	** To compile, must link using -lrt  (man clock_gettime() function).
	** This program was written on Windows. To run on Linux change the read and write modes from rb to r and wb to w.
  ** Author : Ashit Mohanty
  */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main()
{
FILE		*fpt;
unsigned char	*image;
float *smoothed_intermediate;
unsigned char	*smoothed;
unsigned char	*smoothed_sep;
unsigned char	*smoothed_sep_sw;
char		header[320];
int		ROWS,COLS,BYTES;
int		r,c,r2,c2,sum,flag,visit;
float sum_inter;
struct timespec	tp1,tp2;

	/* read image */
if ((fpt=fopen("bridge.ppm","rb")) == NULL)
  {
  printf("Unable to open bridge.ppm for reading\n");
  exit(0);
  }
fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
  printf("Not a greyscale 8-bit PPM image\n");
  exit(0);
  }
image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
header[0]=fgetc(fpt);	/* read white-space character that separates header */
fread(image,1,COLS*ROWS,fpt);
fclose(fpt);

	/* allocate memory for smoothed version of image */
smoothed=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

  /* allocate memory for smoothed version of image using separable filters */
smoothed_sep=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

/* allocate memory for smoothed version of image using separable filters and sliding window */
smoothed_sep_sw=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

/* allocate memory for storing intermediate values during separable filters */
smoothed_intermediate=(float *)calloc(ROWS*COLS,sizeof(float));

/*PART 1 : Basic 2D convolution using 7x7 filter*/
/*DESCRIPTION: To achieve this functionality in the code, I used the if condition with a set of relational statements that would ensure that the index(row,column)
of the pixel would always be greater than 0 and lesser than the maximum number of rows and columns.
If it were to fail this condition the sum would be equated to zero, followed by changing the value of the flag variable to 1.
The if statement, as mentioned earlier, would have this flag variable to check whether any convolution kernel was lying outside the image boundaries.
If yes, then it wouldn’t enter the if block.*/

  /* query timer */
clock_gettime(CLOCK_REALTIME,&tp1);
printf("\nStart time for 2D 7x7 convolution: %lds %ldns\n",(long int)tp1.tv_sec,tp1.tv_nsec);

	/* smooth image, skipping the border points */
for (r=0; r<ROWS; r++)
{
  for (c=0; c<COLS; c++)
    {
    sum=0;
    flag=0;
    for (r2=-3; r2<=3; r2++)
    {
      for (c2=-3; c2<=3; c2++)
      {
        if (((r+r2)>=0) && ((c+c2)>=0) && ((r+r2)<ROWS) && ((c+c2)<COLS) && (flag==0))
        {
          sum+=image[(r+r2)*COLS+(c+c2)];
        }
        else
        {
          sum=0;
          flag=1;
        }
      }
    }
    smoothed[r*COLS+c]=round((double)sum/49);
    }
}
	/* query timer */
clock_gettime(CLOCK_REALTIME,&tp2);
printf("End time for 2D 7x7 convolution: %lds %ldns\n",(long int)tp2.tv_sec,tp2.tv_nsec);

	/* report how long it took to smooth */
printf("The time to perform convolution using simple 2D 7x7 matrix is %ld nsecs\n",tp2.tv_nsec-tp1.tv_nsec);

	/* write out smoothed image to see result */
fpt=fopen("smoothed.ppm","wb");
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(smoothed,COLS*ROWS,1,fpt);
fclose(fpt);


/*PART 2: Convolution using separable filters*/
/*DESCRIPTION:To achieve this functionality of the separable filters in the columns, in the code, I used the if condition with a set of relational statements that would
ensure that the index of the pixel(column index) would always be greater than 0 and lesser than the maximum number of rows and columns.
If it were to fail this condition the sum would be equated to zero, followed by changing the value of the flag variable to 1.
The output from this 1x7 filter was then stored in an intermediate float array, which was used as the input for the next filter, 7x1.

To achieve this functionality of separable filter in the rows, in the code, I used the if condition with a set of relational statements that would ensure that the
index of the pixel(row index) would always be greater than 0 and lesser than the maximum number of rows.
If it were to fail this condition the sum would be equated to zero, followed by changing the value of the flag variable to 1.
The if statement, as mentioned earlier, would have this flag variable to check whether any convolution kernel was lying outside the image boundaries.
If yes, then it wouldn’t enter the if block.*/

/* query timer */
clock_gettime(CLOCK_REALTIME,&tp1);
printf("\nStart time for convolution using separable filters: %lds %ldns\n",(long int)tp1.tv_sec,tp1.tv_nsec);

/* smooth image, skipping the border points */

for (r=0; r<ROWS; r++)
{
  for (c=0; c<COLS; c++)
  {
    flag=0;
    sum=0;
    for (c2=-3; c2<=3; c2++)
    {
      if (((c+c2)>=0) && ((c+c2)<COLS) && (flag==0))
      {
        sum+=image[(r*COLS)+(c+c2)];
      }
      else
      {
        sum=0;
        flag=1;
      }
    }
    smoothed_intermediate[r*COLS+c]=(double)sum/7;
  }
}

for (c=0; c<COLS; c++)
{
  for (r=0; r<ROWS; r++)
  {
    sum_inter=0;
    flag=0;
    for (r2=-3; r2<=3; r2++)
    {
      if (((r+r2)>=0) && ((r+r2)<ROWS) && (flag==0))
      {
        sum_inter+=smoothed_intermediate[((r+r2)*COLS)+c];
      }
      else
      {
        sum_inter=0;
        flag=1;
      }
    }
    smoothed_sep[r*COLS+c]=round(sum_inter/7);
  }
}

  /* query timer */
clock_gettime(CLOCK_REALTIME,&tp2);
printf("End time for convolution using separable filters : %lds %ldns\n",(long int)tp2.tv_sec,tp2.tv_nsec);

/* report how long it took to smooth using separable filters */
printf("The time to perform convolution using separable filters is %ld nsecs\n",tp2.tv_nsec-tp1.tv_nsec);

/* write out smoothed image to see result */
fpt=fopen("smoothed_sep.ppm","wb");
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(smoothed_sep,COLS*ROWS,1,fpt);
fclose(fpt);


/*PART 3: Convolution using separable filters and sliding window*/
/*DESCRIPTION: To achieve this functionality of separable filters and sliding window in the code, I used the if condition with a set of relational statements that would
ensure that the index of the pixel(row index) would always be greater than 0 and lesser than the maximum number of rows.
If it were to fail this condition the sum would be equated to zero, followed by changing the value of the flag variable to 1.
Also when it moving along the same row,a visit variable is used to check whether any pixel has been visited in that row.
If yes, the code performs operations on the sum to attain the total sum of that pixel for the 1x7 filter using the sliding window logic.
The output from this 1x7 filter was then stored in an intermediate float array, which was used as the input for the next filter, 7x1.
To achieve this functionality in the code, I used the if condition with a set of relational statements that would ensure that the index
of the pixel(column index) would always be greater than 0 and lesser than the maximum number of columns. If it were to fail this condition
the sum would be equated to zero, followed by changing the value of the flag variable to 1. Also, when it is moving along the same column,
a visit variable is used to check whether any pixel has been visited in that column. If yes, the code performs operations on the sum,
 to attain the total sum of that pixel for the 7x1 filter using the sliding window logic.
*/

/* query timer */
clock_gettime(CLOCK_REALTIME,&tp1);
printf("\nStart time for convolution with separable filters + sliding window: %lds %ldns\n",(long int)tp1.tv_sec,tp1.tv_nsec);

/* smooth image, skipping the border points */
for (c=0; c<COLS; c++)
{
  sum=0;
  visit=0;
  for (r=0; r<ROWS; r++)
  {
    flag=0;
    if (visit==1 && ((r+3)<ROWS) && ((r-3)>=0))                                   //checking if in same row and in image boundary
    {
      sum=sum-image[(r-4)*COLS+c]+image[(r+3)*COLS+c];                            //calculating sum using sliding window
    }
    else
    {
      sum=0;
    }
    for (r2=-3; r2<=3 && (visit == 0); r2++)
    {
      if (((r+r2)>=0) && ((r+r2)<ROWS) && (flag==0))
      {
        sum+=image[(r+r2)*COLS+c];
      }
      else
      {
        sum=0;
        flag=1;
      }
    }
    if ((flag==0) && (visit==0))
    {
      visit=1;
    }
    smoothed_intermediate[r*COLS+c]=(double)sum/7;
  }
}

for (r=0; r<ROWS; r++)
{
  sum_inter=0;
  visit=0;
  for (c=0; c<COLS; c++)
  {
    flag=0;
    if (visit==1 && ((c+3)<COLS) && ((c-3)>=0))                                                        //checking if in same column and in image boundary
    {
      sum_inter=sum_inter-smoothed_intermediate[r*COLS+(c-4)]+smoothed_intermediate[r*COLS+(c+3)];       //calculating sum using sliding window
    }
    else
    {
      sum_inter=0;
    }
    for (c2=-3; c2<=3 && (visit == 0); c2++)
    {
      if (((c+c2)>=0) && ((c+c2)<COLS) && (flag==0))
      {
        sum_inter+=smoothed_intermediate[r*COLS+(c+c2)];
      }
      else
      {
        sum_inter=0;
        flag=1;
      }
    }
    if ((flag==0) && (visit==0))
    {
      visit=1;
    }
    smoothed_sep_sw[r*COLS+c]=round(sum_inter/7);
  }
}
  /* query timer */
clock_gettime(CLOCK_REALTIME,&tp2);
printf("End time for convolution with separable filters + sliding window: %lds %ldns\n",(long int)tp2.tv_sec,tp2.tv_nsec);

/* report how long it took to smooth using separable filters and sliding window*/
printf("The time to perform convolution with separable filters + sliding window is %ld nsecs\n",tp2.tv_nsec-tp1.tv_nsec);

/* write out smoothed image to see result */
fpt=fopen("smoothed_sep_sw.ppm","wb");
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
//fwrite(smoothed_sep_sw,COLS*ROWS,1,fpt);
fwrite(smoothed_sep_sw,COLS*ROWS,1,fpt);
fclose(fpt);
}
