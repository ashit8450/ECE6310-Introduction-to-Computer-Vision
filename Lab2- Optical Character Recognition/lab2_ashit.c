#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

int main(int argc, char *arg[])
{
  FILE *fpt;
  FILE *fpt1;
  int x,i,j,k,thresh_l,thresh_u,sum=0,avg,max=0,min=99999,sumc;
  int r,c,flag;
  char inp_l[1];
  int l=3,inp_col,inp_row;
  int COLS,ROWS,BYTES;
  int COLS_T,ROWS_T,BYTES_T;
  char header[2];
  char header_t[2];
  unsigned char *image;                         //to store parenthood.ppm image
  unsigned char *image_t;                       //to store the template image
  unsigned char *image_msf;                     //to store the computed normalized msf image
  unsigned char *bin_image;                     //to store the value of binary image
  int *zero_mean;                               //to store the zero mean template
  int *msf_val;                                 //to store the msf image(not 8 bit)

  int detect=0;                                 //to store if image is detected or not
  int TP,FP,FN,TN;
  double TPR,FPR;

  if (argc!=3)
  {
    printf("You need to enter three inputs: [executable] [threshold_lower] [threshold upper]\n");
    exit(0);
  }
  thresh_l=atoi(arg[1]);                                                        //lower threshold limit
  thresh_u=atoi(arg[2]);                                                        //upper threshold limit
  if(thresh_l>thresh_u)
  {
    printf("Enter lower threshold first followed by upper threshold eg. [lower threshold] [upper threshold]");
    exit(0);
  }
  //Opening parenthood.ppm image and storing in variable 'image'
  fpt=fopen("parenthood.ppm","rb");
  if(fpt == NULL)
  {
    printf("Error in opening file parenthood.ppm\n");
    exit(0);
  }
  x=fscanf(fpt,"%s %d %d %d ",header,&COLS,&ROWS,&BYTES);
  if (x!=4 || strcmp(header,"P5")!=0 || BYTES!=255)
  {
    printf("Error in reading the file parenthood.ppm\n");
    exit(0);
  }
  image=(unsigned char *)calloc(COLS*ROWS,sizeof(unsigned char));
  if(image==NULL)
  {
    printf("The memory allocation of size %d x %d failed\n",COLS,ROWS);
    exit(0);
  }
  fread(image,1,COLS*ROWS,fpt);
  fclose(fpt);

  /*##############################################################################################*/
  //Opening parenthood_e_template.ppm and storing in variable 'image_t'
  fpt=fopen("parenthood_e_template.ppm","rb");
  if(fpt == NULL)
  {
    printf("Error in opening file parenthood_e_template.ppm\n");
    exit(0);
  }
  x=fscanf(fpt,"%s",header_t);
  x+=fscanf(fpt,"%d %d",&COLS_T,&ROWS_T);
  x+=fscanf(fpt,"%d ",&BYTES_T);
  if (x!=4 || strcmp(header_t,"P5")!=0 || BYTES_T!=255)
  {
    printf("Error in reading the file parenthood_e_template.ppm\n");
    exit(0);
  }
  image_t=(unsigned char *)calloc(COLS_T*ROWS_T,sizeof(unsigned char));
  if(image==NULL)
  {
    printf("The memory allocation of size %d x %d failed\n",COLS_T,ROWS_T);
    exit(0);
  }
  fread(image_t,1,COLS_T*ROWS_T,fpt);
  fclose(fpt);
  /*##############################################################################################*/
  //Computing the average of the template image to find the zero mean template
  for(i=0;i<ROWS_T;i++)
  {
    for(j=0;j<COLS_T;j++)
    {
      sum=sum+image_t[i*COLS_T+j];
    }
  }
  avg=sum/(ROWS_T*COLS_T);
  zero_mean=(int *)calloc(COLS_T*ROWS_T,sizeof(int));
  for(i=0;i<ROWS_T;i++)
  {
    for(j=0;j<COLS_T;j++)
    {
      zero_mean[i*COLS_T+j]=image_t[i*COLS_T+j] - avg;
    }
  }

  //Calculating the msf image(not 8bit) using the zero mean template
  image_msf=(unsigned char *)calloc(COLS*ROWS,sizeof(unsigned char));
  msf_val=(int *)calloc(COLS*ROWS,sizeof(int));
  for (i=0; i<ROWS; i++)
  {
    for (j=0; j<COLS; j++)
    {
      sumc=0;
      flag=0;
      for (r=(-ROWS_T/2); r<=(ROWS_T/2); r++)
      {
        for (c=(-COLS_T/2); c<=(COLS_T/2); c++)
        {
          if (((i+r)>=0) && ((j+c)>=0) && ((i+r)<ROWS) && ((j+c)<COLS) && (flag==0))
          {
            sumc+=image[(i+r)*COLS+(j+c)]*zero_mean[(r+ROWS_T/2)*COLS_T+(c+COLS_T/2)];
          }
          else
          {
            sumc=0;
            flag=1;
          }
        }
      }
      msf_val[i*COLS+j]=sumc;
      if (msf_val[i*COLS+j]>max && flag==0)
      {
        max=msf_val[i*COLS+j];
      }
      if (msf_val[i*COLS+j]<min && flag==0)
      {
        min=msf_val[i*COLS+j];
      }
    }
  }

  //To compute the normalized MSF image
  for(i=ROWS_T/2;i<ROWS-(ROWS_T/2);i++)
  {
    for(j=COLS_T/2;j<COLS-(COLS_T/2);j++)
    {
      image_msf[i*COLS+j]=round((double)((msf_val[i*COLS+j])-min)*((double)(255-0)/(max-min)))+0;
    }
  }
  fpt=fopen("parenthood_msf.ppm","wb");
  fprintf(fpt,"%s %d %d %d\n",header_t,COLS,ROWS,BYTES);
  fwrite(image_msf,1,COLS*ROWS,fpt);
  fclose(fpt);

  /*##############################################################################################*/
  //Looping through range of threshold to find the values of TP,TN,FP,FN

  bin_image=(unsigned char *)calloc(COLS*ROWS,sizeof(unsigned char));
  fpt1=fopen("output_lab2.txt","wb");
  for(k=thresh_l;k<=thresh_u;k++)
  {
    TP=0,FP=0,FN=0,TN=0;
    TPR=0.0,FPR=0.0;
    l=3;
    fpt=fopen("parenthood_gt.txt","rb");
    if(fpt==NULL)
    {
      printf("Failed to open file parenthood_gt.txt\n");
      exit(0);
    }

    //Creating a binary image using the threshold value
    for(i=0;i<ROWS;i++)
    {
      for(j=0;j<COLS;j++)
      {
        if(image_msf[i*COLS+j]>=k)
        {
          bin_image[i*COLS+j]=255;
        }
        else
        {
          bin_image[i*COLS+j]=0;
        }
      }
    }

    //Reading the first entry of the text file and iterating it till end of file reached
    l=fscanf(fpt,"%s %d %d",inp_l,&inp_col,&inp_row);
    while(l==3)
    {
      detect=0;
      for (i=inp_row-7; i<=inp_row+7; i++)
      {
        for(j=inp_col-4;j<=inp_col+4;j++)
        {
          if (bin_image[i*COLS+j]==255)
          {
            detect=1;
          }
        }
      }
      if((strcmp(inp_l,"e")==0) && detect==1)
      {
        TP++;
      }
      else if((strcmp(inp_l,"e")!=0) && detect==1)
      {
        FP++;
      }
      else if((strcmp(inp_l,"e")==0) && detect==0)
      {
        FN++;
      }
      else if((strcmp(inp_l,"e")!=0) && detect==0)
      {
        TN++;
      }
      l=fscanf(fpt,"%s %d %d",inp_l,&inp_col,&inp_row);
    }
    TPR=(double)TP/(TP+FN);
    FPR=(double)FP/(FP+TN);
    fclose(fpt);
    printf("Threshold:%d TP:%d FP:%d\n",k,TP,FP);
    fprintf(fpt1,"%d %d %d %d %d %lf %lf\n",k,TP,FP,FN,TN,TPR,FPR);
  }
  fclose(fpt1);
}
