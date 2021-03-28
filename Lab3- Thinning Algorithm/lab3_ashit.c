#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

int *thinning(int val[],int i,int j)
{
  //Function to return the values of a,b and c for the pixel values
  int dir[4][2]={{0,1},{1,0},{0,-1},{-1,0}};             //showing the traversal of pixels in clockwise direction
  int k,l;
  int y_pos=0,x_pos=0;
  int val1[187];                                        //variable to store and image with a padding on the boundaries to help deal with edge case scenarios
  int i_1,j_1;
  int i_init,j_init;
  int a=0,b=0,c=1;
  int *answer;
  int res=0;
  int count=0;
  int dir_r=0;
  answer=(int *)calloc(4,sizeof(int));

  //Creating the image with a padded size of 1 band of pixels on all sides, to deal with edge case scenarios
  for(k=0;k<17;k++)
  {
    for(l=0;l<11;l++)
    {
      if(l==0 || k==0|| k==16 || l==10)
      {
        val1[k*11+l]=255;
      }
      else
      {
        val1[k*11+l]=val[(k-1)*9+(l-1)];
      }
    }
  }
  i_init=i+1;
  j_init=j+1;

  i_1=i_init-1;
  j_1=j_init-1;
  x_pos=(i_1)*11+(j_1);
  y_pos=(i_1+dir[dir_r][0])*11+(j_1+dir[dir_r][1]);
  count++;
  if((val1[x_pos]==0) && (val1[y_pos]==255))
  {
    a++;
  }
  if(val1[x_pos]==0)
  {
    b++;
  }

  while(y_pos!=(i*11+j))
  {
    x_pos=y_pos;
    i_1=y_pos/11;
    j_1=y_pos%11;
    y_pos=(i_1+dir[dir_r][0])*11+(j_1+dir[dir_r][1]);
    count=count+1;

    if((val1[x_pos]==0) && (val1[y_pos]==255))
    {
      a=a+1;
    }
    if(val1[x_pos]==0)
    {
      b=b+1;
    }
    if(count==2)
    {
      dir_r++;
      count=count*0;
    }
  }

  if((val[(i-1)*9+j]!=0) || (val[(i)*9+(j+1)]!=0) || ((val[(i+1)*9+j]!=0)&&(val[(i)*9+(j-1)]!=0)))
  {
    c=0;
  }
  if((a==1) && (2<=b) && (b<=6) && (c==0))
  {
    res=1;        //denoting pixel is ready to be removed
  }
  answer[0]=a;
  answer[1]=b;
  answer[2]=c;
  answer[3]=res;
  return answer;
}

int main(int argc, char *arg[])
{
  FILE *fpt;
  FILE *fpt1;
  FILE *fpt2;
  int x,i,j,k,thresh_l,thresh_u,sum=0,avg,max=0,min=99999,sumc;
  int r,c,flag;
  unsigned char *inp_l;
  int l=3,inp_col,inp_row;
  int a1,b1,c1,res;
  int mark=1;
  int flag1=1;
  int *ANS;                                     //to store the ouput from the thinning function
  int counter=0;
  int EP,BP;
  int COLS,ROWS,BYTES;
  int COLS_T,ROWS_T,BYTES_T;
  char header[2];
  char header_t[2];
  unsigned char *image;                         //to store parenthood.ppm image
  unsigned char *image_t;                       //to store the template image
  unsigned char *image_msf;                     //to store the computed normalized msf image
  unsigned char *bin_image;                     //to store the value of binary image
  int oper_image[135];                          //to store intermediate thinned image
  unsigned char *thinned_image;                 //to store the thinned image
  unsigned char *gt_pixel;                     //to store the copy of 9x15 centered around g-truth locatin
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

  //##############################################################################################
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
  //##############################################################################################
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
  fpt=fopen("msf_e.ppm","wb");
  fprintf(fpt,"%s %d %d %d\n",header_t,COLS,ROWS,BYTES);
  fwrite(image_msf,1,COLS*ROWS,fpt);
  fclose(fpt);

  //##############################################################################################
  //The below part is part of Lab 3
  //Looping through range of threshold to find the values of TP,TN,FP,FN
  bin_image=(unsigned char *)calloc(9*15,sizeof(unsigned char));
  thinned_image=(unsigned char *)calloc(9*15,sizeof(unsigned char));
  gt_pixel=(unsigned char *)calloc(9*15,sizeof(unsigned char));
  inp_l=(unsigned char *)calloc(1,sizeof(unsigned char));
  ANS=(int *)calloc(9*15,sizeof(int));
  fpt1=fopen("output_lab3.txt","wb");

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

    //Reading the first entry of the text file and iterating it till end of file reached
    l=fscanf(fpt,"%s %d %d",inp_l,&inp_col,&inp_row);
    while(l==3)
    {
      detect=0;
      counter=0;
      mark=1;
      for (i=inp_row-7; i<=inp_row+7; i++)                      //detecting the letter from msf image
      {
        for(j=inp_col-4;j<=inp_col+4;j++)
        {
          if (image_msf[i*COLS+j]>k)
          {
            detect=1;
          }
        }
      }
      if(detect == 1)
      {
        EP=0,BP=0;
        flag1=1;
        for (i=inp_row-7; i<=inp_row+7; i++)                        //if detected, extracting 9x15 image from main image file
        {
          for(j=inp_col-4;j<=inp_col+4;j++)
          {
            gt_pixel[counter]=image[i*COLS+j];
            counter=counter+1;
          }
        }
        for (i=0;i<15;i++)                                          //thresholding the extracted image to create a binary file
        {
          for(j=0;j<9;j++)
          {
            if(gt_pixel[i*9+j]>128)
            {
              bin_image[i*9+j]=255;
              oper_image[i*9+j]=255;
              thinned_image[i*9+j]=255;
            }
            else
            {
              bin_image[i*9+j]=0;
              oper_image[i*9+j]=0;
              thinned_image[i*9+j]=0;
            }
          }
        }
        while(mark!=0)                                            //thinning image till no more pixels can be removed
        {
          flag1=1;
          mark=0;
          for(i=0;i<15;i++)
          {
            for(j=0;j<9;j++)
            {
              if(oper_image[i*9+j]==0)
              {
                ANS=thinning(oper_image,i,j);                     //calling thinning function
                if(ANS[3]==1)
                {
                  mark=mark+1;;
                  thinned_image[i*9+j]=255;
                  flag1=0;
                }
                if(ANS[3]!=1)
                {
                  thinned_image[i*9+j]=0;
                }
              }
              else
              {
                thinned_image[i*9+j]=oper_image[i*9+j];
              }
            }
          }
          if(flag1!=1)                                    //the image with removed pixels is again fed in variable to compute thinning, if pixels were peeled in previous iteration
          {
            for(i=0;i<15;i++)
            {
              for(j=0;j<9;j++)
              {
                if (thinned_image[i*9+j]==0)
                {
                  oper_image[i*9+j]=0;
                }
                if (thinned_image[i*9+j]==255)
                {
                  oper_image[i*9+j]=255;
                }
              }
            }
          }
        }

        for(i=0;i<15;i++)                           //adding the thinned image to an int pointer so that can be computed using thinning function to detect branch and end points
        {
          for(j=0;j<9;j++)
          {
            if (thinned_image[i*9+j]==0)
            {
              oper_image[i*9+j]=0;
            }
            if (thinned_image[i*9+j]==255)
            {
              oper_image[i*9+j]=255;
            }
          }
        }
        for(i=0;i<15;i++)                            //calculating branch-points and end-points
        {
          for(j=0;j<9;j++)
          {
            if(thinned_image[i*9+j]==0)
            {
              ANS=thinning(oper_image,i,j);
              if(ANS[0]==1)
              {
                EP++;
              }
              else if(ANS[0]>2)
              {
                BP++;
              }
            }
          }
        }
        if((EP==1) && (BP==1))                            //detecting system response
        {
          detect=1;
        }
        else if((EP!=1) || (BP!=1))
        {
          detect=0;
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
  fpt2=fopen("binary.ppm","wb");
  fprintf(fpt2,"%s %d %d %d\n",header_t,9,15,BYTES_T);
  fwrite(bin_image,1,9*15,fpt2);
  fclose(fpt2);

  fpt2=fopen("binary_thin.ppm","wb");
  fprintf(fpt2,"%s %d %d %d\n",header_t,9,15,BYTES_T);
  fwrite(thinned_image,1,9*15,fpt2);
  fclose(fpt2);

  fpt2=fopen("original_letter.ppm","wb");
  fprintf(fpt2,"%s %d %d %d\n",header_t,9,15,BYTES_T);
  fwrite(gt_pixel,1,9*15,fpt2);
  fclose(fpt2);
  fclose(fpt1);
}
