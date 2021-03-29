//Submitted by: Ashit Mohanty- C13582787
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  void RegionGrow();
  FILE		*fpt;
  int		ROWS,COLS,BYTES;
  double	xangle,yangle,dist;
  double		P[3][128*128];
  char		header[80];
  double	cp[7];
  double	ScanDirectionFlag,SlantCorrection;
  unsigned char *image,*threshold_image,*labeled,*RGB_image;
  double *normal_x,*normal_y,*normal_z;
  int RegionSize,*RegionPixels,TotalRegions,*indices;
  int i,r,c,r2,c2,thres_dist,flag;
  double ans_x,ans_y,ans_z;
  double	x0, y0, z0, x1, y1, z1, x2, y2, z2;
  double	ax, ay, az, bx, by, bz, cx, cy, cz;

  if(argc!=2)
  {
    printf("Usage: [executable][image filename]\n");
    exit(0);
  }
  fpt=fopen(argv[1],"rb");
  if (fpt== NULL)
  {
    printf("Unable to open %s for reading\n",argv[1]);
    exit(0);
  }
  i=fscanf(fpt,"%s %d %d %d ",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

  threshold_image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  for (r=0; r<ROWS; r++)
  {
    for (c=0; c<COLS; c++)
    {
      if(image[r*COLS+c]>140)
      {
        threshold_image[r*COLS+c]=255;
      }
      else
      {
        threshold_image[r*COLS+c]=0;
      }
    }
  }

  fpt=fopen("chair_threshold.ppm","wb");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(threshold_image,COLS*ROWS,1,fpt);
  fclose(fpt);

  ScanDirectionFlag=1;

  cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
  cp[1]=32.0;		/* scan time per single pixel in microseconds */
  cp[2]=(COLS/2)-0.5;		/* middle value of columns */
  cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
  cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
  cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
  cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

  cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
  cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
  cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
  cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
  cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
  cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

  fpt=fopen("chair_coordinates.txt","wb");
  for (r=0; r<ROWS; r++)
    {
    for (c=0; c<COLS; c++)
      {
      SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
      xangle=cp[0]*cp[1]*((double)c-cp[2]);
      yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+	/* Standard Transform Part */
	SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
      dist=(double)image[r*COLS+c]+cp[6];
      P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
	+(tan(yangle)*tan(yangle))));
      P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
      P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];

      fprintf(fpt,"%lf %lf %lf\n",P[0][r*COLS+c],P[1][r*COLS+c],P[2][r*COLS+c]);
      }
    }
  fclose(fpt);

  thres_dist=4;

  normal_x=(double *)calloc(ROWS*COLS,sizeof(double));
  normal_y=(double *)calloc(ROWS*COLS,sizeof(double));
  normal_z=(double *)calloc(ROWS*COLS,sizeof(double));

  fpt=fopen("chair_normal_coordinates.txt","wb");
  for (r=0;r<ROWS;r++)
  {
    for (c=0;c<COLS;c++)
    {
      if(((r+thres_dist)>=ROWS)||((c+thres_dist)>=COLS))
      {
        normal_x[r*COLS+c]=0;
        normal_y[r*COLS+c]=0;
        normal_z[r*COLS+c]=0;
      }
      else
      {
        if(threshold_image[r*COLS+c]==255)
        {
          normal_x[r*COLS+c]=0;
          normal_y[r*COLS+c]=0;
          normal_z[r*COLS+c]=0;
        }
        else
        {
          x0 = P[0][r*COLS+c];
          y0 = P[1][r*COLS+c];
          z0 = P[2][r*COLS+c];

          x1 = P[0][(r-thres_dist)*COLS+c];
          y1 = P[1][(r-thres_dist)*COLS+c];
          z1 = P[2][(r-thres_dist)*COLS+c];

          x2 = P[0][r*COLS+(c-thres_dist)];
          y2 = P[1][r*COLS+(c-thres_dist)];
          z2 = P[2][r*COLS+(c-thres_dist)];

          ax = x1-x0;
          ay = y1-y0;
          az = z1-z0;
          bx = x2-x0;
          by = y2-y0;
          bz = z2-z0;

          normal_x[r*COLS+c] = (ay*bz) - (az*by);
          normal_y[r*COLS+c] = (az*bx) - (ax*bz);
          normal_z[r*COLS+c] = (ax*by) - (ay*bx);
        }
      }
      fprintf(fpt,"%lf %lf %lf\n",normal_x[r*COLS+c],normal_y[r*COLS+c],normal_z[r*COLS+c]);
    }
  }
  fclose(fpt);

  labeled=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  indices = (int *)calloc(ROWS*COLS*3, sizeof(int));
  TotalRegions=0;
  for (r=2; r<ROWS-2; r++)
  {
    for (c=2; c<COLS-2; c++)
    {
      flag=0;
      for (r2=-2; r2<=2; r2++)
      {
        for (c2=-2; c2<=2; c2++)
        {
          if (labeled[(r+r2)*COLS+(c+c2)]!=0)
          {
            flag=1;
          }
        }
      }
      if(flag==0 && threshold_image[r*COLS+c]==0)
      {
        TotalRegions++;
        RegionGrow(threshold_image,labeled,ROWS,COLS,r,c,0,TotalRegions,indices,&RegionSize,normal_x,normal_y,normal_z,&ans_x,&ans_y,&ans_z);
        if (RegionSize < 35)
        {
          for (i=0; i<RegionSize; i++)
          {
            labeled[indices[i]]=0;
          }
          TotalRegions--;
        }
        else
        {
          printf("Region labeled %d is %d pixels in size with average normals X:%lf, Y:%lf, Z:%lf\n",TotalRegions,RegionSize,ans_x,ans_y,ans_z);
        }
      }
    }
  }

  printf("%d total regions were found\n",TotalRegions);

  RGB_image=(unsigned char *)calloc(ROWS*COLS*3,sizeof(unsigned char));
  for(i=0; i<ROWS*COLS;i++)
  {
    RGB_image[i*3+0] = (labeled[i]*53)%255;
    RGB_image[i*3+1] = (labeled[i]*97)%255;
    RGB_image[i*3+2] = (labeled[i]*223)%255;
  }
  if ((fpt=fopen("chair-color.ppm","wb")) == NULL)
  {
  printf("Unable to open file for writing\n");
  exit(0);
  }
  fprintf(fpt,"P6 %d %d 255\n",COLS,ROWS);
  fwrite(RGB_image,1,ROWS*COLS*3,fpt);
  fclose(fpt);
}

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow(unsigned char *image,	/* image data */
		unsigned char *labels,	/* segmentation labels */
		int ROWS,int COLS,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count,
    double *x_n,
    double *y_n,
    double *z_n,
    double *a_x,
    double *a_y,
    double *a_z)		/* output:  count of pixels painted */
{
int	r2,c2;
int	queue[MAX_QUEUE],qh,qt;
double angle,dot,mag_a,mag_b;
*count=0;

if (indices != NULL)
  indices[0]=r*COLS+c;
queue[0]=r*COLS+c;
qh=1;	/* queue head */
qt=0;	/* queue tail */
(*count)=1;
(*a_x) = x_n[(queue[qt]/COLS)*COLS+queue[qt]%COLS];
(*a_y) = y_n[(queue[qt]/COLS)*COLS+queue[qt]%COLS];
(*a_z) = z_n[(queue[qt]/COLS)*COLS+queue[qt]%COLS];
while (qt != qh)
{
  for (r2=-1; r2<=1; r2++)
  {
    for (c2=-1; c2<=1; c2++)
    {
      if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label)
        continue;
      if((x_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=0) && (y_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=0) && (z_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=0))
      {
        (*a_x) = ((*a_x) + x_n[(r+r2)*COLS+(c+c2)]) / 2.0;
        (*a_y) = ((*a_y) + y_n[(r+r2)*COLS+(c+c2)]) / 2.0;
        (*a_z) = ((*a_z) + z_n[(r+r2)*COLS+(c+c2)]) / 2.0;
        dot = ((*a_x)*x_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]) + ((*a_y)*y_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]) + ((*a_z)*z_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]);
        mag_a=sqrt(pow((*a_x), 2) + pow((*a_y), 2) + pow((*a_z), 2));
        mag_b=sqrt(pow(x_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2) + pow(y_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2) + pow(z_n[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2));
        angle = acos(dot/(mag_a*mag_b));
        if (fabs(angle) > (0.174*6))
          continue;
        labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
        if (indices != NULL)
          indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
        (*count)++;
        queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
        qh=(qh+1)%MAX_QUEUE;
        if (qh == qt)
        {
          printf("Max queue size exceeded\n");
          exit(0);
        }
      }
    }
  }
  qt=(qt+1)%MAX_QUEUE;
  }
}
