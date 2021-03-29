//Submitted by: Ashit Mohanty- C13582787
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  FILE		*fpt;
  int		ROWS,COLS,BYTES;
  char		header[80];
  unsigned char *image,*filtered,*marked,*final;
  int *filtered_inter;
  float *energy;
  int *internal1,*internal2,*external;
  int points[42][2];
  int points_f[42][2];
  int max_i1,min_i1,max_i2,min_i2,max_e,min_e;
  int move_c,move_r;
  float move_min;
  int i,j,r,c,r2,c2;
  int dist,t_dist=0;
  float avg_dist;
  int sum_x,sum_y;
  int max=0,min=99999;
  int flag,count,counter=0;
  int sfilter_c[3][3]={{-1,0,1},{-2,0,2},{-1,0,1}};
  int sfilter_r[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};

  if(argc!=3)
  {
    printf("Usage: [executable][contour points filename][image filename]\n");
    exit(0);
  }
  fpt=fopen(argv[2],"rb");
  if (fpt== NULL)
  {
    printf("Unable to open %s for reading\n",argv[2]);
    exit(0);
  }
  i=fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

  fpt=fopen(argv[2],"rb");
  if (fpt== NULL)
  {
    printf("Unable to open %s for reading\n",argv[2]);
    exit(0);
  }
  i=fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
  }
  final=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  fread(final,1,COLS*ROWS,fpt);
  fclose(fpt);

  filtered=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  filtered_inter=(int *)calloc(ROWS*COLS,sizeof(int));
  //This section below, indicates the algorithm for sobel gradient generation
  for (r=0; r<ROWS; r++)
  {
    for (c=0; c<COLS; c++)
    {
      sum_x=0;
      sum_y=0;
      flag=0;
      for (r2=-1; r2<=1; r2++)
      {
        for (c2=-1; c2<=1; c2++)
        {
          if (((r+r2)>=0) && ((c+c2)>=0) && ((r+r2)<ROWS) && ((c+c2)<COLS))
          {
            sum_x+=image[(r+r2)*COLS+(c+c2)]*sfilter_c[r2+1][c2+1];
            sum_y+=image[(r+r2)*COLS+(c+c2)]*sfilter_r[r2+1][c2+1];
          }
          else
          {
            sum_x+=0;
            sum_y+=0;
          }
        }
      }
      filtered_inter[r*COLS+c]=sqrt(pow(sum_y,2)+pow(sum_x,2));
      if (filtered_inter[r*COLS+c]>max)
      {
        max=filtered_inter[r*COLS+c];
      }
      if (filtered_inter[r*COLS+c]<min)
      {
        min=filtered_inter[r*COLS+c];
      }
    }
  }
  //Normalizing the Sobel edge gradient image
  for(r=0;r<ROWS;r++)
  {
    for(c=0;c<COLS;c++)
    {
      filtered[r*COLS+c]=round((double)((filtered_inter[r*COLS+c])-min)*((double)(255-0)/(max-min)))+0;
    }
  }
  fpt=fopen("hawk_sobel.ppm","wb");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(filtered,COLS*ROWS,1,fpt);
  fclose(fpt);

  //printing original image with initial contour points
  marked=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  marked=image;
  fpt=fopen(argv[1],"rb");
  if (fpt== NULL)
  {
    printf("Unable to open %s for reading\n",argv[1]);
    exit(0);
  }
  i=fscanf(fpt,"%d %d",&points_f[count][0],&points_f[count][1]);
  while(i==2)
  {
    for(r2=-3;r2<=3;r2++)
    {
      for(c2=-3;c2<=3;c2++)
      {
        if((r2==0)||(c2==0))
        {
          marked[(points_f[count][1]+r2)*COLS+(points_f[count][0]+c2)]=0;
        }
      }
    }
    count++;
    i=fscanf(fpt,"%d %d",&points_f[count][0],&points_f[count][1]);
  }
  fclose(fpt);
  fpt=fopen("hawk_marked_initial.ppm","wb");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(marked,COLS*ROWS,1,fpt);
  fclose(fpt);

  internal1=(int *)calloc(49,sizeof(int));
  internal2=(int *)calloc(49,sizeof(int));
  external=(int *)calloc(49,sizeof(int));
  energy=(float *)calloc(49,sizeof(float));

  //computing average distance between initial consecutive contour points
  for(i=0;i<count;i++)
  {
    if(i==(count-1))
    {
      t_dist+=sqrt(pow(points_f[i][0]-points_f[0][0],2)+pow(points_f[i][1]-points_f[1][1],2));
    }
    else
    {
      t_dist+=sqrt(pow(points_f[i][0]-points_f[i+1][0],2)+pow(points_f[i][1]-points_f[i+1][1],2));
    }
  }
  avg_dist=(double)t_dist/count;

  //active contour algorithm being run for 50 iterations
  while(counter<50)
  {
    for(i=0;i<count;i++)
    {
      points[i][0]=points_f[i][0];
      points[i][1]=points_f[i][1];
    }
    for(i=0;i<count;i++)
    {
      max_i1=-9999;
      min_i1=9999;
      max_i2=-9999;
      min_i2=9999;
      max_e=-9999;
      min_e=9999;
      move_min=9999;
      for(r=-3;r<=3;r++)
      {
        for(c=-3;c<=3;c++)
        {
          external[(3+r)*7+(3+c)]=-1*pow(filtered[(points[i][1]+r)*COLS+(points[i][0]+c)],2);
          if(i==(count-1))
          {
            internal1[(3+r)*7+(3+c)]=pow((points[i][0]+c)-points[0][0],2)+pow((points[i][1]+r)-points[0][1],2);
            internal2[(3+r)*7+(3+c)]=pow(avg_dist-sqrt(pow((points[i][0]+c)-points[0][0],2)+pow((points[i][1]+r)-points[0][1],2)),2);
          }
          else
          {
            internal1[(3+r)*7+(3+c)]=pow((points[i][0]+c)-points[i+1][0],2)+pow((points[i][1]+r)-points[i+1][1],2);
            internal2[(3+r)*7+(3+c)]=pow(avg_dist-sqrt(pow((points[i][0]+c)-points[i+1][0],2)+pow((points[i][1]+r)-points[i+1][1],2)),2);
          }
        }
      }
      for(j=0;j<49;j++)
      {
        if(internal1[j]>max_i1)
        {
          max_i1=internal1[j];
        }
        if(internal1[j]<min_i1)
        {
          min_i1=internal1[j];
        }
        if(internal2[j]>max_i2)
        {
          max_i2=internal2[j];
        }
        if(internal2[j]<min_i2)
        {
          min_i2=internal2[j];
        }
        if(external[j]>max_e)
        {
          max_e=external[j];
        }
        if(external[j]<min_e)
        {
          min_e=external[j];
        }
      }
      for(r=-3;r<=3;r++)
      {
        for(c=-3;c<=3;c++)
        {
          energy[(3+r)*7+(3+c)]=(double)(internal1[(3+r)*7+(3+c)]-min_i1)*((double)(1-0)/(max_i1-min_i1))+1.6*(double)(internal2[(3+r)*7+(3+c)]-min_i2)*((double)(1-0)/(max_i2-min_i2))+0.8*(double)(external[(3+r)*7+(3+c)]-min_e)*((double)(1-0)/(max_e-min_e));
          if(energy[(3+r)*7+(3+c)]<move_min)
          {
            move_min=energy[(3+r)*7+(3+c)];
            move_c=c;
            move_r=r;
          }
        }
      }
      points_f[i][0]+=move_c;
      points_f[i][1]+=move_r;
    }
    counter++;
  }
  //printing original image with final position of contour points
  fpt=fopen("hawk_points_final.txt","wb");
  for(i=0;i<count;i++)
  {
    fprintf(fpt,"%d %d\n",points_f[i][0],points_f[i][1]);
    for(r2=-3;r2<=3;r2++)
    {
      for(c2=-3;c2<=3;c2++)
      {
        if((r2==0)||(c2==0))
        {
          final[(points_f[i][1]+r2)*COLS+(points_f[i][0]+c2)]=0;
        }
      }
    }
  }
  fclose(fpt);
  fpt=fopen("hawk_final_image.ppm","wb");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(final,COLS*ROWS,1,fpt);
  fclose(fpt);
}
