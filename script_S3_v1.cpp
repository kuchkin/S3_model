//g++ script_S3_v1.cpp -std=c++17 -pthread -ffast-math -O3 -o a 

#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <cmath>
// #include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <algorithm>
#include <atomic>
#include <unordered_set>
#include <random>
#include "bitmap_image.hpp"

using namespace std;

const double pi = 3.1415926535897932384626433832795;
const double tpi = 6.283185307179586476925286766559;



#define DIM 12
#define PHYS 10
#define MaxNumOfShells 4


char configfilename[64] = "S3_config.cfg";
char InitialBmpFile[64];
char InitialBinFile[64];
char IntermediateBmpFile[64];
char IntermediateBinFile[64];
char FinalBmpFile[64];
char FinalBinFile[64];
char InputBin[64];
char InputOvf[64];
char ConvergenceInfoFile[64];
char FinalCsvFile[64];
char FinalOvfFile[64];
char State1[64];
char State2[64];

#define R2D     57.295779513
int ColorShift=0;
char BuferString[400];


int ReadHeaderLine(FILE * fp, char * line)
{
    char c;//single character 
    int pos=0;
    do{ c = (char)fgetc(fp);//get current char and move pointer to the next position
        if (c != EOF && c != '\n') { line[pos++] = c;}//if it's not the end of the file
    }while(c != EOF && c != '\n');//if it's not the end of the file or end of the line
    line[pos] = 0;//complite the readed line
    if ((pos==0 || line[0]!='#') && c != EOF){
        return ReadHeaderLine(fp, line);// recursive call for ReadHeaderLine if the current line is empty
    } 
    return pos-1;// the last symbol is the line end symbol
}


void readConfigFile(int** Dimension, double** Parameters, double* tempJD)
{
   printf("Reading config file %s ... ", configfilename);
   char  line[256];//whole line of header should be not longer then 256 characters
   int   lineLength=0;
    char  keyW1 [256];//key word 1
    char  keyW2 [256];//key word 2
    char  keyW3 [256];//key word 3

    FILE * FilePointer = fopen(configfilename, "rb");
   if(FilePointer != NULL) {  
      lineLength=ReadHeaderLine(FilePointer, line);//read and check the first nonempty line which starts with '#'
      if (lineLength == -1) {// if there are no one line which starts with '#'
         printf("%s has a wrong format! \n", configfilename);
      }else{
          sscanf(line, "# %s %s %s", keyW1, keyW2, keyW3 );
          // printf("%s %s %s\n", keyW1, keyW2, keyW3);
          if(strncmp(keyW1, "begin",5)!=0 || strncmp(keyW2, "config",6)!=0 || strncmp(keyW3, "file",  4)!=0){
              //if the first line isn't "# magnoom config file"
            printf("%s has wrong header of wrong file format! \n", configfilename);
            lineLength = -1;
          }
      }
      //READING HEADER
      if (lineLength != -1){
         do{
            lineLength = ReadHeaderLine(FilePointer, line);
            sscanf(line, "# %s %s %s", keyW1, keyW2, keyW3 );
            // printf("%s %s %s\n", keyW1, keyW2, keyW3);
            if (strncmp(keyW1, "na:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[2][0] );               
            }else if (strncmp(keyW1, "nb:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[2][1] );
            }else if (strncmp(keyW1, "nc:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[2][2] );            
            }else if (strncmp(keyW1, "Nx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[0][0] );               
            }else if (strncmp(keyW1, "Ny:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[0][1] );
            }else if (strncmp(keyW1, "Nz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[0][2] );            
            }else if (strncmp(keyW1, "NumOfShelsX:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[1][0] );            
            }else if (strncmp(keyW1, "NumOfShelsY:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[1][1] );
            }else if (strncmp(keyW1, "NumOfShelsZ:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[1][2] );            
            }else if (strncmp(keyW1, "NumOfIterations:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[5][1] );            
            }else if (strncmp(keyW1, "WriteIterations:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[5][2] );            
            }else if (strncmp(keyW1, "Type:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[6][0] );            
            }else if (strncmp(keyW1, "SaveConvergenceInfo:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[6][1] );            
            }else if (strncmp(keyW1, "SaveIntermediateBmpFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[6][2] );            
            }else if (strncmp(keyW1, "SaveIntermediateBinFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[7][0] );            
            }else if (strncmp(keyW1, "Graphics:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[7][1] );            
            }else if (strncmp(keyW1, "DrawingMode:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[7][2] );            
            }else if (strncmp(keyW1, "PBCx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[8][0] );            
            }else if (strncmp(keyW1, "PBCy:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[8][1] );            
            }else if (strncmp(keyW1, "PBCz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[8][2] );            
            }else if (strncmp(keyW1, "Subspace:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[9][0] );            
            }else if (strncmp(keyW1, "FBCx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[10][0] );            
            }else if (strncmp(keyW1, "FBCy:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[10][1] );            
            }else if (strncmp(keyW1, "FBCz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[10][2] );            
            }else if (strncmp(keyW1, "DMI_top:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[11][2] );            
            }else if (strncmp(keyW1, "MicroBC:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[11][1] );            
            }else if (strncmp(keyW1, "DMI_bot:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[11][0] );            
            }else if (strncmp(keyW1, "Jx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[0][0] );            
            }else if (strncmp(keyW1, "Jy:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[0][1] );
            }else if (strncmp(keyW1, "Jz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[0][2] );            
            }else if (strncmp(keyW1, "Dx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[1][0] );            
            }else if (strncmp(keyW1, "Dy:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[1][1] );
            }else if (strncmp(keyW1, "Dz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[1][2] );            
            }else if (strncmp(keyW1, "kx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[2][0] );            
            }else if (strncmp(keyW1, "ky:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[2][1] );
            }else if (strncmp(keyW1, "kz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[2][2] );            
            }else if (strncmp(keyW1, "hx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[3][0] );            
            }else if (strncmp(keyW1, "hy:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[3][1] );
            }else if (strncmp(keyW1, "hz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[3][2] );            
            }else if (strncmp(keyW1, "Lx:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[4][0] );            
            }else if (strncmp(keyW1, "Ly:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[4][1] );
            }else if (strncmp(keyW1, "Angle:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[5][0] );
            }else if (strncmp(keyW1, "dt:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[5][1] );
            }else if (strncmp(keyW1, "MaxAverageTorque:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[5][2] );
            }else if (strncmp(keyW1, "mass:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[6][0] );
            }else if (strncmp(keyW1, "SpringConstant:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[6][1] );
            }else if (strncmp(keyW1, "Lz:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[4][2] );            
            }else if (strncmp(keyW1, "Damping:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[6][2] );            
            }else if (strncmp(keyW1, "Temperature:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[7][0] );            
            }else if (strncmp(keyW1, "kappa:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[7][1] );            
            }else if (strncmp(keyW1, "ku:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[7][2] );            
            }else if (strncmp(keyW1, "Current:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[8][0] );            
            }else if (strncmp(keyW1, "ChargeConstraint:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[8][1] );            
            }else if (strncmp(keyW1, "Charge:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[8][2] );            
            }else if (strncmp(keyW1, "BN:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[9][0] );            
            }else if (strncmp(keyW1, "km:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[9][1] );            
            }else if (strncmp(keyW1, "ks:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &Parameters[9][2] );            
            }else if (strncmp(keyW1, "JijX1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[0] );            
            }else if (strncmp(keyW1, "JijX2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[1] );            
            }else if (strncmp(keyW1, "JijX3:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[2] );            
            }else if (strncmp(keyW1, "JijX4:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[3] );            
            }else if (strncmp(keyW1, "JijY1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[4] );            
            }else if (strncmp(keyW1, "JijY2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[5] );            
            }else if (strncmp(keyW1, "JijY3:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[6] );            
            }else if (strncmp(keyW1, "JijY4:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[7] );            
            }else if (strncmp(keyW1, "JijZ1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[8] );            
            }else if (strncmp(keyW1, "JijZ2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[9] );            
            }else if (strncmp(keyW1, "JijZ3:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[10] );            
            }else if (strncmp(keyW1, "JijZ4:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[11] );            
            }else if (strncmp(keyW1, "DijX1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[0+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijX2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[1+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijX3:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[2+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijX4:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[3+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijY1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[4+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijY2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[5+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijY3:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[6+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijY4:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[7+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijZ1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[8+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijZ2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[9+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijZ3:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[10+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "DijZ4:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%lf", &tempJD[11+3*MaxNumOfShells] );            
            }else if (strncmp(keyW1, "ReadInputFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%d", &Dimension[5][0]);            
            }else if (strncmp(keyW1, "InitialBmpFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", InitialBmpFile);       
            }else if (strncmp(keyW1, "InitialBinFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", InitialBinFile);       
            }else if (strncmp(keyW1, "FinalBinFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", FinalBinFile);       
            }else if (strncmp(keyW1, "FinalBmpFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", FinalBmpFile);       
            }else if (strncmp(keyW1, "IntermediateBinFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", IntermediateBinFile);       
            }else if (strncmp(keyW1, "IntermediateBmpFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", IntermediateBmpFile);       
            }else if (strncmp(keyW1, "InputBin:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", InputBin);       
            }else if (strncmp(keyW1, "ConvergenceInfoFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", ConvergenceInfoFile);       
            }else if (strncmp(keyW1, "FinalCsvFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", FinalCsvFile);       
            }else if (strncmp(keyW1, "FinalOvfFile:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", FinalOvfFile);       
            }else if (strncmp(keyW1, "InputOvf:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", InputOvf);       
            }else if (strncmp(keyW1, "State1:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", State1);       
            }else if (strncmp(keyW1, "State2:",sizeof(keyW1))==0) {
               sscanf(keyW2, "%s", State2);       
            }


         }while(strncmp(keyW1, "end",3) !=0 || strncmp(keyW2, "config",6) !=0 || strncmp(keyW3, "file",  4) !=0);
      }     
      // when everything is done
      printf("Done!\n");
      fclose(FilePointer);
   }else{
      printf("Cannot open file: %s \n", configfilename);
      exit(3);
   }
}

void checkDimension(int** Dimension, double** Parameters, double*tempJD, double* JD){
   bool error = false;
   for(int i = 0; i<DIM; i++){
      if(Dimension[0][0]<1 || Dimension[0][0]<Dimension[1][0]){
         printf("ERROR: Nx has to be from the range [1, NumOfShelsX-1]. I have Nx = %d, and NumOfShelsX = %d\n",Dimension[0][0],Dimension[1][0]);
         error = true;
      }
      if(Dimension[0][1]<1 || Dimension[0][1]<Dimension[1][1]){
         printf("ERROR: Ny has to be from the range [1, NumOfShelsY-1]. I have Ny = %d, and NumOfShelsY = %d\n",Dimension[0][1],Dimension[1][1]);
         error = true;
      }
       if(Dimension[0][2]<1 || Dimension[0][2]<Dimension[1][2]){
         printf("ERROR: Nz has to be from the range [1, NumOfShelsZ-1]. I have Nz = %d, and NumOfShelsZ = %d\n",Dimension[0][2],Dimension[1][2]);
         error = true;
      }
      if(Dimension[1][0]<0){
         printf("ERROR: NumOfShelsX has to be at least 0. I have %d\n",Dimension[1][0]);
         error = true;
      }
      if(Dimension[1][1]<0){
         printf("ERROR: NumOfShelsY has to be at least 0. I have %d\n",Dimension[1][1]);
         error = true;
      }
      if(Dimension[1][2]<0){
         printf("ERROR: NumOfShelsZ has to be at least 0. I have %d\n",Dimension[1][2]);
         error = true;
      }
      if(Dimension[1][0]>MaxNumOfShells){
         printf("ERROR: NumOfShelsX = %d exceeds maximal number of shells %d\n",Dimension[1][0], MaxNumOfShells);
         error = true;
      }
      if(Dimension[1][1]>MaxNumOfShells){
         printf("ERROR: NumOfShelsY = %d exceeds maximal number of shells %d\n",Dimension[1][1], MaxNumOfShells);
         error = true;
      }
      if(Dimension[1][2]>MaxNumOfShells){
         printf("ERROR: NumOfShelsZ = %d exceeds maximal number of shells %d\n",Dimension[1][2], MaxNumOfShells);
         error = true;
      }
      if(Dimension[0][0]%Dimension[2][0] != 0){
         printf("ERROR: na = %d is not a divisor of Nx = %d\n",Dimension[2][0],Dimension[0][0]);
         error = true;
      }
      if(Dimension[0][1]%Dimension[2][1] != 0){
         printf("ERROR: nb = %d is not a divisor of Ny = %d\n",Dimension[2][1],Dimension[0][1]);
         error = true;
      }
      if(Dimension[0][2]%Dimension[2][2] != 0){
         printf("ERROR: nc = %d is not a divisor of Nz = %d\n",Dimension[2][2],Dimension[0][2]);
         error = true;
      }

      if(error) exit(-2);
      Dimension[3][0] = 2*Dimension[1][0]+Dimension[0][0]/Dimension[2][0];//N1
      Dimension[3][1] = 2*Dimension[1][1]+Dimension[0][1]/Dimension[2][1];//N2
      Dimension[3][2] = 2*Dimension[1][2]+Dimension[0][2]/Dimension[2][2];//N3

      Dimension[4][0] = Dimension[0][0]*Dimension[0][1]*Dimension[0][2];//N
      Dimension[4][1] = Dimension[2][0]*Dimension[2][1]*Dimension[2][2];//NumOfThreads
      Dimension[4][2] = Dimension[3][0]*Dimension[3][1]*Dimension[3][2];//Ns

      
   }
   printf("System of %dx%dx%d = %d spins\n",Dimension[0][0],Dimension[0][1],Dimension[0][2],Dimension[0][0]*Dimension[0][1]*Dimension[0][2]);
   printf("with (%d,%d,%d) shells\n",Dimension[1][0],Dimension[1][1],Dimension[1][2]);
   if(Dimension[4][1]>1) printf("is split into %d independent parts\n",Dimension[4][1]);
   printf("Boundary conditions are: [%d, %d, %d]\n",Dimension[8][0],Dimension[8][1],Dimension[8][2]);
   printf("size (Lx,Ly,Lz) = (%f,%f,%f)\n",Parameters[4][0],Parameters[4][1],Parameters[4][2]);
   printf("##########################\n");
   printf("Physical parameters are:\n");
   printf("Exchange in (x,y,z): (%0.1f,%0.1f,%0.1f)\n",Parameters[0][0],Parameters[0][1],Parameters[0][2]);
   printf("DMI in (x,y,z): (%0.1f,%0.1f,%0.1f)\n",Parameters[1][0],Parameters[1][1],Parameters[1][2]);
   printf("DMI axes angle: %f\n",Parameters[9][0]);
   if(Dimension[11][0] == 0) {
      printf("DMI is in all layers\n");
   }else{
      printf("DMI is in %d surface layers\n",Dimension[11][0]);
   }
   printf("Easy axis/plane anisotropy in (x,y,z): (%f,%f,%f)\n",Parameters[2][0],Parameters[2][1],Parameters[2][2]);
   printf("Magnetic field in (x,y,z): (%f,%f,%f)\n",Parameters[3][0],Parameters[3][1],Parameters[3][2]);


   for(int i=0;i<6*MaxNumOfShells;i++){
      // printf("%f\n",tempJD[i]);
   }
   for(int i = 0; i<Dimension[1][0]; i++){
      JD[i] = tempJD[i];
      JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]] = tempJD[i+3*MaxNumOfShells];
   }
   for(int i = Dimension[1][0]; i<(Dimension[1][0]+Dimension[1][1]); i++){
      JD[i] = tempJD[i-Dimension[1][0]+MaxNumOfShells];
      JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]] = tempJD[i-Dimension[1][0]+4*MaxNumOfShells];
      // printf("%f\n",JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]] );
   }
   for(int i = (Dimension[1][0]+Dimension[1][1]); i<(Dimension[1][0]+Dimension[1][1]+Dimension[1][2]); i++){
      JD[i] = tempJD[i-Dimension[1][0]-Dimension[1][1]+2*MaxNumOfShells];
      JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]] = tempJD[i-Dimension[1][0]-Dimension[1][1]+5*MaxNumOfShells];
      // printf("%f\n",JD[i] );
   }



   if(Dimension[1][0] == 0){
      printf("Exchange along x: 0.0\n");
      printf("DMI along x: 0.0\n");
   }else{
      printf("Exchange along x: ");
      for(int i = 0; i<Dimension[1][0]; i++){
         printf("%f ", JD[i]); 
      }printf("\n"); 
      printf("DMI along x: ");
      for(int i = 0; i<Dimension[1][0]; i++){
         printf("%f ", JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]]); 
      }printf("\n");
   }
   if(Dimension[1][1] == 0){
      printf("Exchange along y: 0.0\n");
      printf("DMI along y: 0.0\n");
   }else{
      printf("Exchange along y: ");
      for(int i = Dimension[1][0]; i<(Dimension[1][1]+Dimension[1][0]); i++){
         printf("%f ", JD[i]); 
      }printf("\n"); 
      printf("DMI along y: ");
      for(int i = Dimension[1][0]; i<(Dimension[1][1]+Dimension[1][0]); i++){
         printf("%f ", JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]]); 
      }printf("\n");
   }
   if(Dimension[1][2] == 0){
      printf("Exchange along z: 0.0\n");
      printf("DMI along z: 0.0\n");
   }else{
      printf("Exchange along z: ");
      for(int i = (Dimension[1][0]+Dimension[1][1]); i<(Dimension[1][0]+Dimension[1][1]+Dimension[1][2]); i++){
         printf("%f ", JD[i]); 
      }printf("\n"); 
      printf("DMI along z: ");
      for(int i = (Dimension[1][0]+Dimension[1][1]); i<(Dimension[1][0]+Dimension[1][1]+Dimension[1][2]); i++){
         printf("%f ", JD[i+Dimension[1][0]+Dimension[1][1]+Dimension[1][2]]); 
      }printf("\n");
   }
   


}

















void ReadDataLine(FILE * fp, char * line)
{
    char c;//single character 
    int pos=0;
    do{ c = (char)fgetc(fp);
        if (c != EOF && c != '\n') { line[pos++] = c;}
    }while(c != EOF && c != '\n');
    line[pos] = 0;
}

void Cone(double** g, bool** b, int** Dimension){
   int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;

    double Th = 0.5*M_PI;


    for(int k=0; k<Nz; k++){
        for(int j=0; j<Ny; j++){
            for(int i=0; i<Nx; i++){
                int n = i + j*Nx + k*Nx*Ny;
                int it = (int) i*na/Nx;
                int jt = (int) j*nb/Ny;
                int kt = (int) k*nc/Nz;

                int nt = it + jt*na + kt*na*nb;
                int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;

                double g1 = g[nt][pos], g2 = g[nt][pos+Ns];
                double ss = (b[nt][pos])? 1.0 : -1.0;
                double gg = 1./(1.0+g1*g1+g2*g2); 
                double sx = 2.0*g1*gg;
                double sy = 2.0*g2*gg;
                double sz = ss*(1.0-g1*g1-g2*g2)*gg;
                sx = 0.0; sy = 0.0; sz = 1.0;
                double phi = tpi*k/Nz;

                // if(k==0 || k==Nz){
                //     sx = 0.0; sy = 0.0; sz = 1.0;
                // }

                // if(i<30 || j<30 || i>90 || j>90){
                //     sx = 0.0; sy = 0.0; sz = 1.0;
                // }

                double mx = sx*cos(phi)+sy*sin(phi);
                double my = sy*cos(phi)-sx*sin(phi);

                double mz = sz*cos(Th) + mx*sin(Th);
                mx = mx*cos(Th) - sz*sin(Th);

                sx = mx*cos(phi)-my*sin(phi);
                sy = my*cos(phi)+mx*sin(phi);
                sz = mz;

                // sx = cos(phi);
                // sy = sin(phi);
                // sz = 0.0;


                ss = (sz>0)? 1.0 : -1.0;
                b[nt][pos] = (sz>0)? true : false;
                g[nt][pos]    = sx/(1.0+ss*sz);
                g[nt][pos+Ns] = sy/(1.0+ss*sz); 
            }
        }
    }

}

void Helicoid(double** g, bool** b, int** Dimension){
   int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;

    double Th = 0.45*M_PI;


    for(int k=0; k<Nz; k++){
        for(int j=0; j<Ny; j++){
            for(int i=0; i<Nx; i++){
                int n = i + j*Nx + k*Nx*Ny;
                int it = (int) i*na/Nx;
                int jt = (int) j*nb/Ny;
                int kt = (int) k*nc/Nz;

                int nt = it + jt*na + kt*na*nb;
                int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;

                double g1 = g[nt][pos], g2 = g[nt][pos+Ns];
                double ss = (b[nt][pos])? 1.0 : -1.0;
                double gg = 1./(1.0+g1*g1+g2*g2); 
                double sx = 2.0*g1*gg;
                double sy = 2.0*g2*gg;
                double sz = ss*(1.0-g1*g1-g2*g2)*gg;
                sx = 0.0; sy = 0.0; sz = 1.0;
                double phi = 6*tpi*i/Nx;
                // double phi = tpi*j/Ny;

                // if(k==0 || k==Nz){
                //     sx = 0.0; sy = 0.0; sz = 1.0;
                // }

                // if(i<30 || j<30 || i>90 || j>90){
                //     sx = 0.0; sy = 0.0; sz = 1.0;
                // }

                double mx = sx*cos(phi)+sy*sin(phi);
                double my = sy*cos(phi)-sx*sin(phi);

                double mz = sz*cos(Th) + mx*sin(Th);
                mx = mx*cos(Th) - sz*sin(Th);

                sx = 0.0;
                sy = cos(phi);
                sz = sin(phi);

                sz = cos(0.2);
                sx = sin(0.2);
                sy = 0.0;

                // sy = 0.0;
                // sx = sin(phi);
                // sz = cos(phi);


                ss = (sz>0)? 1.0 : -1.0;
                b[nt][pos] = (sz>0)? true : false;
                g[nt][pos]    = sx/(1.0+ss*sz);
                g[nt][pos+Ns] = sy/(1.0+ss*sz); 
            }
        }
    }

}




void ReadOVF2(double** g, bool** b, int** Dimension, char inputfilename[100])
{
   
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;

    

    char     line[256];//whole line of header should be not longer then 256 characters
    int      lineLength = 0;
    int      valuedim = 3;

    int NN;
    
    double  dx, dy, dz, time;
    char    keyW1 [256];//key word 1
    char    keyW2 [256];//key word 2
    char    keyW3 [256];//key word 3
    char    keyW4 [256];//key word 3
    char    keyW5 [256];//key word 3
    char    keyW6 [256];//key word 3
    int     binType = 4;
    float temp4_x, temp4_y, temp4_z, temp4_v;
    double temp8_x, temp8_y, temp8_z, temp8_v;
    double sx, sy, sz,sv, ss;
    FILE * FilePointer = fopen(inputfilename, "rb");
    if (FilePointer!=NULL) {   
        lineLength=ReadHeaderLine(FilePointer, line);//read and check the first nonempty line which starts with '#'
        if (lineLength==-1) {// if there are no one line which starts with '#'
            printf("%s has a wrong file format! \n", inputfilename);
        }else{
            sscanf(line, "# %s %s %s %s %s %s", keyW1, keyW2, keyW3, keyW4, keyW5, keyW6 );
            if(strncmp(keyW1, "OOMMF",5)!=0 || strncmp(keyW2, "OVF",  3)!=0 || strncmp(keyW3, "2.0",  3)!=0){
                printf("%s has wrong header of wrong file format! \n", inputfilename);//if the first line isn't "OOMMF OFV 2.0"
                lineLength=-1;
            }
        }
    //READING HEADER
    if (lineLength!=-1){
        do{
            lineLength = ReadHeaderLine(FilePointer, line);
            sscanf(line, "# %s %s %s %s %s %s", keyW1, keyW2, keyW3, keyW4, keyW5, keyW6);
            //printf("%s %s %s\n", keyW1, keyW2, keyW3);Desc: Total simulation time:
            if (strncmp(keyW1, "valuedim:",9)==0) {
               sscanf(keyW2, "%d", &valuedim );
               // printf("valuedim=%d\n", valuedim);              
            }else if (  strncmp(keyW1, "Desc:",5)==0     && 
                strncmp(keyW2, "Total",5)==0     &&
                strncmp(keyW3, "simulation",10)==0  &&
                strncmp(keyW4, "time:",5)==0     &&
                strncmp(keyW6, "s",1)==0) {
                sscanf(keyW5, "%lf", &time );
                time=time*1e9;
               // printf("time=%.6g ns\n", time);
            }else if (strncmp(keyW1, "xnodes:",7)==0) {
                sscanf(keyW2, "%d", &NN );
                // printf("xnodes=%d\n", Nx);
            }else if (strncmp(keyW1, "ynodes:",7)==0) {
                sscanf(keyW2, "%d", &NN );
               // printf("ynodes=%d\n", Ny);             
            }else if (strncmp(keyW1, "znodes:",7)==0) {
               sscanf(keyW2, "%d", &NN );
               // printf("znodes=%d\n", Nz);             
            }else if (strncmp(keyW1, "xstepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dx);
               dx=dx*1e9;
               // printf("dx=%.6g nm\n", dx);               
            }else if (strncmp(keyW1, "ystepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dy);
               dy=dy*1e9;
               // printf("dy=%.6g nm\n", dy);               
            }else if (strncmp(keyW1, "zstepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dz);
               dz=dz*1e9;
               // printf("dz=%.6g nm\n", dz);               
            }
         }while(!(strncmp(keyW1, "Begin:",6)==0 && strncmp(keyW2, "Data",4)==0) && lineLength != -1 );
      }

        //READING DATA
        if (valuedim!=0){
            sscanf(line, "#%*s %s %s %s", keyW1, keyW2, keyW3 );
            int n;
            if (strncmp(keyW2, "Text",4)==0){
            //Text data format
            // printf("...reading data in text format: %s \n", inputfilename);
            for (int k=0; k<Nz; k++){
                for (int j=0; j<Ny; j++){
                    for (int i=0; i<Nx; i++){
                        n = i + j*Nx + k*Nx*Ny;
                        int it = (int) i*na/Nx;
                        int jt = (int) j*nb/Ny;
                        int kt = (int) k*nc/Nz;

                        int nt = it + jt*na + kt*na*nb;
                        int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;

                        ReadDataLine(FilePointer, line);
                        sscanf(line, "%lf %lf %lf %lf", &temp8_x,&temp8_y,&temp8_z,&temp8_v);
                        double norm = 1./sqrt(temp8_x*temp8_x + temp8_y*temp8_y + temp8_z*temp8_z + temp8_v*temp8_v);
                        sx = temp8_x*norm; 
                        sy = temp8_y*norm;
                        sz = temp8_z*norm;
                        sv = temp8_v*norm;

                        ss = (sv>0)? 1.0 : -1.0;
                        b[nt][pos] = (sv>0)? true : false;
                        g[nt][pos]      = sx/(1.0+ss*sv);
                        g[nt][pos+Ns]   = sy/(1.0+ss*sv); 
                        g[nt][pos+2*Ns] = sz/(1.0+ss*sv); 
                    }
                }
            }
        }

        if (strncmp(keyW2, "Binary",6)==0){
            if (strncmp(keyW3, "4",1)==0) binType = 4;
            if (strncmp(keyW3, "8",1)==0) binType = 8;
            
            //Binary data format
            printf("...reading data of binary (%d) format: %s \n", binType, inputfilename);
            if(fread(&temp8_x,binType,1,FilePointer)) {
            // printf("%d \n",int(Sx[0]));//first bit       
                for (int k=0; k<Nz; k++){
                    for (int j=0; j<Ny; j++){
                        for (int i=0; i<Nx; i++){
                        n = i + j*Nx + k*Nx*Ny; 

                        int it = (int) i*na/Nx;
                        int jt = (int) j*nb/Ny;
                        int kt = (int) k*nc/Nz;

                        int nt = it + jt*na + kt*na*nb;
                        int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;


                        if (binType==4){
                            if(!fread(&temp4_x,binType,1,FilePointer)) break;
                            if(!fread(&temp4_y,binType,1,FilePointer)) break;
                            if(!fread(&temp4_z,binType,1,FilePointer)) break;
                            if(!fread(&temp4_v,binType,1,FilePointer)) break;
                            double norm = 1./sqrt((double)temp4_x*(double)temp4_x + (double)temp4_y*(double)temp4_y + (double)temp4_z*(double)temp4_z+ (double)temp4_v*(double)temp4_v);
                            sx = ((double)temp4_x)*norm;
                            sy = ((double)temp4_y)*norm;
                            sz = ((double)temp4_z)*norm; 
                            sv = ((double)temp4_v)*norm;     
                        }else{
                            if(!fread(&temp8_x,binType,1,FilePointer)) break;
                            if(!fread(&temp8_y,binType,1,FilePointer)) break;
                            if(!fread(&temp8_z,binType,1,FilePointer)) break;
                            if(!fread(&temp8_v,binType,1,FilePointer)) break;
                            double norm = 1./sqrt(temp8_x*temp8_x + temp8_y*temp8_y + temp8_z*temp8_z+ temp8_v*temp8_v);
                            sx = temp8_x*norm; 
                            sy = temp8_y*norm;
                            sz = temp8_z*norm;
                            sv = temp8_v*norm;    
                        }

                        ss = (sv>0)? 1.0 : -1.0;
                        b[nt][pos] = (sv>0)? true : false;
                        g[nt][pos]      = sx/(1.0+ss*sv);
                        g[nt][pos+Ns]   = sy/(1.0+ss*sv); 
                        g[nt][pos+2*Ns] = sz/(1.0+ss*sv);  
                    }
                }
            }
        }
    }       
      // printf("Done!\n");
    }
      fclose(FilePointer);



   }else{
        printf("Cannot open file: %s \n", inputfilename);
        exit(3);
    }

}

void ReadOVF1(double** g, bool** b, int** Dimension, char inputfilename[100])
{
   
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;

    

    char     line[256];//whole line of header should be not longer then 256 characters
    int      lineLength = 0;
    int      valuedim = 3;

    int NN;
    
    double  dx, dy, dz, time;
    char    keyW1 [256];//key word 1
    char    keyW2 [256];//key word 2
    char    keyW3 [256];//key word 3
    char    keyW4 [256];//key word 3
    char    keyW5 [256];//key word 3
    char    keyW6 [256];//key word 3
    int     binType = 4;
    float temp4_x, temp4_y, temp4_z, temp4_v;
    double temp8_x, temp8_y, temp8_z, temp8_v;
    double sx, sy, sz,sv, ss;
    FILE * FilePointer = fopen(inputfilename, "rb");
    if (FilePointer!=NULL) {   
        lineLength=ReadHeaderLine(FilePointer, line);//read and check the first nonempty line which starts with '#'
        if (lineLength==-1) {// if there are no one line which starts with '#'
            printf("%s has a wrong file format! \n", inputfilename);
        }else{
            sscanf(line, "# %s %s %s %s %s %s", keyW1, keyW2, keyW3, keyW4, keyW5, keyW6 );
            if(strncmp(keyW1, "OOMMF",5)!=0 || strncmp(keyW2, "OVF",  3)!=0 || strncmp(keyW3, "2.0",  3)!=0){
                printf("%s has wrong header of wrong file format! \n", inputfilename);//if the first line isn't "OOMMF OFV 2.0"
                lineLength=-1;
            }
        }
    //READING HEADER
    if (lineLength!=-1){
        do{
            lineLength = ReadHeaderLine(FilePointer, line);
            sscanf(line, "# %s %s %s %s %s %s", keyW1, keyW2, keyW3, keyW4, keyW5, keyW6);
            //printf("%s %s %s\n", keyW1, keyW2, keyW3);Desc: Total simulation time:
            if (strncmp(keyW1, "valuedim:",9)==0) {
               sscanf(keyW2, "%d", &valuedim );
               // printf("valuedim=%d\n", valuedim);              
            }else if (  strncmp(keyW1, "Desc:",5)==0     && 
                strncmp(keyW2, "Total",5)==0     &&
                strncmp(keyW3, "simulation",10)==0  &&
                strncmp(keyW4, "time:",5)==0     &&
                strncmp(keyW6, "s",1)==0) {
                sscanf(keyW5, "%lf", &time );
                time=time*1e9;
               // printf("time=%.6g ns\n", time);
            }else if (strncmp(keyW1, "xnodes:",7)==0) {
                sscanf(keyW2, "%d", &NN );
                // printf("xnodes=%d\n", Nx);
            }else if (strncmp(keyW1, "ynodes:",7)==0) {
                sscanf(keyW2, "%d", &NN );
               // printf("ynodes=%d\n", Ny);             
            }else if (strncmp(keyW1, "znodes:",7)==0) {
               sscanf(keyW2, "%d", &NN );
               // printf("znodes=%d\n", Nz);             
            }else if (strncmp(keyW1, "xstepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dx);
               dx=dx*1e9;
               // printf("dx=%.6g nm\n", dx);               
            }else if (strncmp(keyW1, "ystepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dy);
               dy=dy*1e9;
               // printf("dy=%.6g nm\n", dy);               
            }else if (strncmp(keyW1, "zstepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dz);
               dz=dz*1e9;
               // printf("dz=%.6g nm\n", dz);               
            }
         }while(!(strncmp(keyW1, "Begin:",6)==0 && strncmp(keyW2, "Data",4)==0) && lineLength != -1 );
      }

        //READING DATA
        if (valuedim!=0){
            sscanf(line, "#%*s %s %s %s", keyW1, keyW2, keyW3 );
            int n;
            if (strncmp(keyW2, "Text",4)==0){
            //Text data format
            // printf("...reading data in text format: %s \n", inputfilename);
            for (int k=0; k<Nz; k++){
                for (int j=0; j<Ny; j++){
                    for (int i=0; i<Nx; i++){
                        n = i + j*Nx + k*Nx*Ny;
                        int it = (int) i*na/Nx;
                        int jt = (int) j*nb/Ny;
                        int kt = (int) k*nc/Nz;

                        int nt = it + jt*na + kt*na*nb;
                        int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;

                        ReadDataLine(FilePointer, line);
                        sscanf(line, "%lf %lf %lf", &temp8_x,&temp8_y,&temp8_z); temp8_v = 0.0;
                        double norm = 1./sqrt(temp8_x*temp8_x + temp8_y*temp8_y + temp8_z*temp8_z + temp8_v*temp8_v);
                        sx = temp8_x*norm; 
                        sy = temp8_y*norm;
                        sz = temp8_z*norm;
                        sv = temp8_v*norm;

                        ss = (sv>0)? 1.0 : -1.0;
                        b[nt][pos] = (sv>0)? true : false;
                        g[nt][pos]      = sx/(1.0+ss*sv);
                        g[nt][pos+Ns]   = sy/(1.0+ss*sv); 
                        g[nt][pos+2*Ns] = sz/(1.0+ss*sv); 
                    }
                }
            }
        }

        if (strncmp(keyW2, "Binary",6)==0){
            if (strncmp(keyW3, "4",1)==0) binType = 4;
            if (strncmp(keyW3, "8",1)==0) binType = 8;
            
            //Binary data format
            printf("...reading data of binary (%d) format: %s \n", binType, inputfilename);
            if(fread(&temp8_x,binType,1,FilePointer)) {
            // printf("%d \n",int(Sx[0]));//first bit       
                for (int k=0; k<Nz; k++){
                    for (int j=0; j<Ny; j++){
                        for (int i=0; i<Nx; i++){
                        n = i + j*Nx + k*Nx*Ny; 

                        int it = (int) i*na/Nx;
                        int jt = (int) j*nb/Ny;
                        int kt = (int) k*nc/Nz;

                        int nt = it + jt*na + kt*na*nb;
                        int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;


                        if (binType==4){
                            if(!fread(&temp4_x,binType,1,FilePointer)) break;
                            if(!fread(&temp4_y,binType,1,FilePointer)) break;
                            if(!fread(&temp4_z,binType,1,FilePointer)) break;
                            // if(!fread(&temp4_v,binType,1,FilePointer)) break;
                            temp4_v = 0.0;
                            double norm = 1./sqrt((double)temp4_x*(double)temp4_x + (double)temp4_y*(double)temp4_y + (double)temp4_z*(double)temp4_z+ (double)temp4_v*(double)temp4_v);
                            sx = ((double)temp4_x)*norm;
                            sy = ((double)temp4_y)*norm;
                            sz = ((double)temp4_z)*norm; 
                            sv = ((double)temp4_v)*norm;     
                        }else{
                            if(!fread(&temp8_x,binType,1,FilePointer)) break;
                            if(!fread(&temp8_y,binType,1,FilePointer)) break;
                            if(!fread(&temp8_z,binType,1,FilePointer)) break;
                            // if(!fread(&temp8_v,binType,1,FilePointer)) break;
                             temp8_v = 0.0; 
                            double norm = 1./sqrt(temp8_x*temp8_x + temp8_y*temp8_y + temp8_z*temp8_z+ temp8_v*temp8_v);
                            sx = temp8_x*norm; 
                            sy = temp8_y*norm;
                            sz = temp8_z*norm;
                            sv = temp8_v*norm;    
                        }

                        ss = (sv>0)? 1.0 : -1.0;
                        b[nt][pos] = (sv>0)? true : false;
                        g[nt][pos]      = sx/(1.0+ss*sv);
                        g[nt][pos+Ns]   = sy/(1.0+ss*sv); 
                        g[nt][pos+2*Ns] = sz/(1.0+ss*sv);  
                    }
                }
            }
        }
    }       
      // printf("Done!\n");
    }
      fclose(FilePointer);



   }else{
        printf("Cannot open file: %s \n", inputfilename);
        exit(3);
    }

}


void ReadOVF2s(double** s, int** Dimension, char inputfilename[100])
{
   
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;

    

    char     line[256];//whole line of header should be not longer then 256 characters
    int      lineLength = 0;
    int      valuedim = 3;

    int NN;
    
    double  dx, dy, dz, time;
    char    keyW1 [256];//key word 1
    char    keyW2 [256];//key word 2
    char    keyW3 [256];//key word 3
    char    keyW4 [256];//key word 3
    char    keyW5 [256];//key word 3
    char    keyW6 [256];//key word 3
    int     binType = 4;
    float temp4_x, temp4_y, temp4_z, temp4_v;
    double temp8_x, temp8_y, temp8_z, temp8_v;
    double sx, sy, sz,sv, ss;
    FILE * FilePointer = fopen(inputfilename, "rb");
    if (FilePointer!=NULL) {   
        lineLength=ReadHeaderLine(FilePointer, line);//read and check the first nonempty line which starts with '#'
        if (lineLength==-1) {// if there are no one line which starts with '#'
            printf("%s has a wrong file format! \n", inputfilename);
        }else{
            sscanf(line, "# %s %s %s %s %s %s", keyW1, keyW2, keyW3, keyW4, keyW5, keyW6 );
            if(strncmp(keyW1, "OOMMF",5)!=0 || strncmp(keyW2, "OVF",  3)!=0 || strncmp(keyW3, "2.0",  3)!=0){
                printf("%s has wrong header of wrong file format! \n", inputfilename);//if the first line isn't "OOMMF OFV 2.0"
                lineLength=-1;
            }
        }
    //READING HEADER
    if (lineLength!=-1){
        do{
            lineLength = ReadHeaderLine(FilePointer, line);
            sscanf(line, "# %s %s %s %s %s %s", keyW1, keyW2, keyW3, keyW4, keyW5, keyW6);
            //printf("%s %s %s\n", keyW1, keyW2, keyW3);Desc: Total simulation time:
            if (strncmp(keyW1, "valuedim:",9)==0) {
               sscanf(keyW2, "%d", &valuedim );
               // printf("valuedim=%d\n", valuedim);              
            }else if (  strncmp(keyW1, "Desc:",5)==0     && 
                strncmp(keyW2, "Total",5)==0     &&
                strncmp(keyW3, "simulation",10)==0  &&
                strncmp(keyW4, "time:",5)==0     &&
                strncmp(keyW6, "s",1)==0) {
                sscanf(keyW5, "%lf", &time );
                time=time*1e9;
               // printf("time=%.6g ns\n", time);
            }else if (strncmp(keyW1, "xnodes:",7)==0) {
                sscanf(keyW2, "%d", &NN );
                // printf("xnodes=%d\n", Nx);
            }else if (strncmp(keyW1, "ynodes:",7)==0) {
                sscanf(keyW2, "%d", &NN );
               // printf("ynodes=%d\n", Ny);             
            }else if (strncmp(keyW1, "znodes:",7)==0) {
               sscanf(keyW2, "%d", &NN );
               // printf("znodes=%d\n", Nz);             
            }else if (strncmp(keyW1, "xstepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dx);
               dx=dx*1e9;
               // printf("dx=%.6g nm\n", dx);               
            }else if (strncmp(keyW1, "ystepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dy);
               dy=dy*1e9;
               // printf("dy=%.6g nm\n", dy);               
            }else if (strncmp(keyW1, "zstepsize:",10)==0) {
               sscanf(keyW2, "%lf", &dz);
               dz=dz*1e9;
               // printf("dz=%.6g nm\n", dz);               
            }
         }while(!(strncmp(keyW1, "Begin:",6)==0 && strncmp(keyW2, "Data",4)==0) && lineLength != -1 );
      }

        //READING DATA
        if (valuedim!=0){
            sscanf(line, "#%*s %s %s %s", keyW1, keyW2, keyW3 );
            int n;
            if (strncmp(keyW2, "Text",4)==0){
            //Text data format
            // printf("...reading data in text format: %s \n", inputfilename);
            for (int k=0; k<Nz; k++){
                for (int j=0; j<Ny; j++){
                    for (int i=0; i<Nx; i++){
                        n = i + j*Nx + k*Nx*Ny;
                        int it = (int) i*na/Nx;
                        int jt = (int) j*nb/Ny;
                        int kt = (int) k*nc/Nz;

                        int nt = it + jt*na + kt*na*nb;
                        int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;

                        ReadDataLine(FilePointer, line);
                        sscanf(line, "%lf %lf %lf %lf", &temp8_x,&temp8_y,&temp8_z,&temp8_v);
                        double norm = 1./sqrt(temp8_x*temp8_x + temp8_y*temp8_y + temp8_z*temp8_z + temp8_v*temp8_v);
                        sx = temp8_x*norm; 
                        sy = temp8_y*norm;
                        sz = temp8_z*norm;
                        sv = temp8_v*norm;

                        s[nt][pos]      = sx;
                        s[nt][pos+Ns]   = sy; 
                        s[nt][pos+2*Ns] = sz; 
                        s[nt][pos+3*Ns] = sv; 
                    }
                }
            }
        }

        if (strncmp(keyW2, "Binary",6)==0){
            if (strncmp(keyW3, "4",1)==0) binType = 4;
            if (strncmp(keyW3, "8",1)==0) binType = 8;
            
            //Binary data format
            printf("...reading data of binary (%d) format: %s \n", binType, inputfilename);
            if(fread(&temp8_x,binType,1,FilePointer)) {
            // printf("%d \n",int(Sx[0]));//first bit       
                for (int k=0; k<Nz; k++){
                    for (int j=0; j<Ny; j++){
                        for (int i=0; i<Nx; i++){
                        n = i + j*Nx + k*Nx*Ny; 

                        int it = (int) i*na/Nx;
                        int jt = (int) j*nb/Ny;
                        int kt = (int) k*nc/Nz;

                        int nt = it + jt*na + kt*na*nb;
                        int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;


                        if (binType==4){
                            if(!fread(&temp4_x,binType,1,FilePointer)) break;
                            if(!fread(&temp4_y,binType,1,FilePointer)) break;
                            if(!fread(&temp4_z,binType,1,FilePointer)) break;
                            if(!fread(&temp4_v,binType,1,FilePointer)) break;
                            double norm = 1./sqrt((double)temp4_x*(double)temp4_x + (double)temp4_y*(double)temp4_y + (double)temp4_z*(double)temp4_z+ (double)temp4_v*(double)temp4_v);
                            sx = ((double)temp4_x)*norm;
                            sy = ((double)temp4_y)*norm;
                            sz = ((double)temp4_z)*norm; 
                            sv = ((double)temp4_v)*norm;     
                        }else{
                            if(!fread(&temp8_x,binType,1,FilePointer)) break;
                            if(!fread(&temp8_y,binType,1,FilePointer)) break;
                            if(!fread(&temp8_z,binType,1,FilePointer)) break;
                            if(!fread(&temp8_v,binType,1,FilePointer)) break;
                            double norm = 1./sqrt(temp8_x*temp8_x + temp8_y*temp8_y + temp8_z*temp8_z+ temp8_v*temp8_v);
                            sx = temp8_x*norm; 
                            sy = temp8_y*norm;
                            sz = temp8_z*norm;
                            sv = temp8_v*norm;    
                        }

                        s[nt][pos]      = sx;
                        s[nt][pos+Ns]   = sy; 
                        s[nt][pos+2*Ns] = sz; 
                        s[nt][pos+3*Ns] = sv;  
                    }
                }
            }
        }
    }       
      // printf("Done!\n");
    }
      fclose(FilePointer);



    }else{
        printf("Cannot open file: %s \n", inputfilename);
        exit(3);
    }

}


void SaveBin(double* Sx, double* Sy, double* Sz,int Nx, int Ny, int Nz, char bin_filename[100]){
    unsigned short int num = 65535;
    struct tfshortint {
        unsigned short int t;
        unsigned short int f;
    };
    FILE * pFile = fopen (bin_filename,"wb");

    for(int k = 0; k < Nz; k++){
        for(int j = 0; j < Ny; j++){
            for(int i = 0; i < Nx; i++){
               int n = i+j*Nx+k*Nx*Ny;
            
               double nx = Sx[n], ny = Sy[n], nz = Sz[n];
               
               double T, F;
               T = acos(nz)/pi;
               F = atan2(ny,nx)/pi;
               if(F <= 0) F += 2.0;
               F /= 2;

               unsigned short int p=0, q=0;
         
               q = T*num;
               p = F*num;

               struct tfshortint my_par = {q, p};

               fwrite(&my_par, sizeof(struct tfshortint), 1, pFile);
               // printf("OK!\n");

            }
       }
   }
   fclose (pFile);
   // printf("Recording to the file %s is done!\n", bin_filename);
}

void SaveOvf(double* Sx, double* Sy, double* Sz,int Nx, int Ny, int Nz, char ovf_filename[100]){
    FILE * pFile = fopen (ovf_filename,"wb");
    if(pFile!=NULL) {   
         fputs ("# OOMMF OVF 2.0\n",pFile);
         fputs ("# Segment count: 1\n",pFile);
         fputs ("# Begin: Segment\n",pFile);
         fputs ("# Begin: Header\n",pFile);
         fputs ("# Title: m\n",pFile);
         fputs ("# meshtype: rectangular\n",pFile);
         fputs ("# meshunit: m\n",pFile);
         fputs ("# xmin: 0\n",pFile);
         fputs ("# ymin: 0\n",pFile);
         fputs ("# zmin: 0\n",pFile);

         snprintf(BuferString,80,"# xmax: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# ymax: %.6g\n",1.0);
         fputs (BuferString,pFile);
         
         snprintf(BuferString,80,"# zmax: %.6g\n",1.0);
         fputs (BuferString,pFile);
         fputs ("# valuedim: 3\n",pFile);
         fputs ("# valuelabels: m_x m_y m_z\n",pFile);
         fputs ("# valueunits: 1 1 1\n",pFile);
         fputs ("# Desc: Total simulation time:  0  s\n",pFile);

         snprintf(BuferString,80,"# xbase: %.6g\n",1.0);
         fputs (BuferString,pFile);           

         snprintf(BuferString,80,"# ybase: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# zbase: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# xnodes: %d\n",Nx);
         fputs (BuferString,pFile);
         snprintf(BuferString,80,"# ynodes: %d\n",Ny);
         fputs (BuferString,pFile);
         snprintf(BuferString,80,"# znodes: %d\n",Nz);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# xstepsize:  %.6g\n",1.0);
         fputs (BuferString,pFile);           

         snprintf(BuferString,80,"# ystepsize: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# zstepsize: %.6g\n",1.0);
         fputs (BuferString,pFile);           

         fputs ("# End: Header\n",pFile);
         fputs ("# Begin: Data Binary 8\n",pFile);
         double Temp1[]= {123456789012345.0};
         fwrite (Temp1, sizeof(double), 1, pFile);
         for (int cn = 0; cn<Nz; cn++)
            {
             for (int bn = 0; bn<Ny; bn++)
                {
                 for (int an = 0; an<Nx; an++)
                    {
                    int n = an+bn*Nx+cn*Nx*Ny;// index of the block
                    double Temp[]= {Sx[n], Sy[n], Sz[n]}; 
                    fwrite (Temp , sizeof(double), 3, pFile);
                    
                 }
             }
         }
        fputs ("# End: Data Binary 4\n",pFile);
        fputs ("# End: Segment\n",pFile);
        fclose (pFile);
    }
    // printf("Recording to the file %s is done!\n", ovf_filename);
}









void SaveOvf2(double* Sx, double* Sy, double* Sz, double* Sv,int Nx, int Ny, int Nz, char ovf_filename[100]){
   FILE * pFile = fopen (ovf_filename,"wb");
    if(pFile!=NULL) {   
         fputs ("# OOMMF OVF 2.0\n",pFile);
         fputs ("# Segment count: 1\n",pFile);
         fputs ("# Begin: Segment\n",pFile);
         fputs ("# Begin: Header\n",pFile);
         fputs ("# Title: m\n",pFile);
         fputs ("# meshtype: rectangular\n",pFile);
         fputs ("# meshunit: m\n",pFile);
         fputs ("# xmin: 0\n",pFile);
         fputs ("# ymin: 0\n",pFile);
         fputs ("# zmin: 0\n",pFile);

         snprintf(BuferString,80,"# xmax: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# ymax: %.6g\n",1.0);
         fputs (BuferString,pFile);
         
         snprintf(BuferString,80,"# zmax: %.6g\n",1.0);
         fputs (BuferString,pFile);
         fputs ("# valuedim: 3\n",pFile);
         fputs ("# valuelabels: m_x m_y m_z\n",pFile);
         fputs ("# valueunits: 1 1 1\n",pFile);
         fputs ("# Desc: Total simulation time:  0  s\n",pFile);

         snprintf(BuferString,80,"# xbase: %.6g\n",1.0);
         fputs (BuferString,pFile);           

         snprintf(BuferString,80,"# ybase: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# zbase: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# xnodes: %d\n",Nx);
         fputs (BuferString,pFile);
         snprintf(BuferString,80,"# ynodes: %d\n",Ny);
         fputs (BuferString,pFile);
         snprintf(BuferString,80,"# znodes: %d\n",Nz);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# xstepsize:  %.6g\n",1.0);
         fputs (BuferString,pFile);           

         snprintf(BuferString,80,"# ystepsize: %.6g\n",1.0);
         fputs (BuferString,pFile);

         snprintf(BuferString,80,"# zstepsize: %.6g\n",1.0);
         fputs (BuferString,pFile);           

         fputs ("# End: Header\n",pFile);
         fputs ("# Begin: Data Binary 8\n",pFile);
         double Temp1[]= {123456789012345.0};
         fwrite (Temp1, sizeof(double), 1, pFile);
         for (int cn = 0; cn<Nz; cn++)
            {
             for (int bn = 0; bn<Ny; bn++)
                {
                 for (int an = 0; an<Nx; an++)
                    {
                    int n = an+bn*Nx+cn*Nx*Ny;// index of the block
                    double Temp[]= {Sx[n], Sy[n], Sz[n], Sv[n]}; 
                    fwrite (Temp , sizeof(double), 4, pFile);
                    
                 }
             }
         }
        fputs ("# End: Data Binary 4\n",pFile);
        fputs ("# End: Segment\n",pFile);
        fclose (pFile);
    }
    // printf("Recording to the file %s is done!\n", ovf_filename);

}






void WriteData2OVF(double** g, bool** b, int** Dimension){
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;
    double* Sx = new double [N];
    double* Sy = new double [N];
    double* Sz = new double [N];
    double* Sv = new double [N];
    double g1, g2, g3, ss, gg, gi;
    for(int it = 0; it<na; it++){
        for(int jt = 0; jt<nb; jt++){
            for(int kt = 0; kt<nc; kt++){
                for(int i = NumOfShelsX; i<=Nx/na - 1 + NumOfShelsX; i++){
                    for(int j = NumOfShelsY; j<=Ny/nb - 1 + NumOfShelsY; j++){
                        for(int k = NumOfShelsZ; k<=Nz/nc - 1 + NumOfShelsZ; k++){
                            int n = i - NumOfShelsX + it*Nx/na + (j - NumOfShelsY + jt*Ny/nb)*Nx + (k - NumOfShelsZ + kt*Nz/nc)*Nx*Ny;
                            int nt = it+jt*na+kt*na*nb;
                            int pos = i+j*N1+k*N1*N2; 
                            g1 = g[nt][pos];
                            g2 = g[nt][pos+Ns];
                            g3 = g[nt][pos+2*Ns];
                            ss = (b[nt][pos])? 1.0 : -1.0;
                            gg = g1*g1+g2*g2+g3*g3;
                            gi = 1./(1.+gg);
                     
                            Sx[n] = 2.*g1*gi;
                            Sy[n] = 2.*g2*gi;
                            Sz[n] = 2.*g3*gi;
                            Sv[n] = ss*(1.-gg)*gi;

                    // printf("%0.20f, %0.20f, %0.20f, %0.20f\n",Sx[n],Sy[n],Sz[n],Sv[n]);

                        }
                    }
                }
            }
        }
    }
    
    char *out1 = NULL;
    int p1 = asprintf(&out1, "%s.ovf", FinalOvfFile);  
    SaveOvf(Sx,Sy,Sz,Nx,Ny,Nz,out1);
    free(out1);

    char *out2 = NULL;
    int p2 = asprintf(&out2, "%s.ovf2", FinalOvfFile);  
    SaveOvf2(Sx,Sy,Sz,Sv,Nx,Ny,Nz,out2);    
    free(out2);

    
  
   delete [] Sx; Sx = 0;
   delete [] Sy; Sy = 0;
   delete [] Sz; Sz = 0;
   delete [] Sv; Sv = 0;
}








float color_function(float H ){
    float h, result=0;
    if (H>360){
        h=H-360.f;
    }else{
        h=H;
    }
    if (60<=h && h<180){
        result=1.0f;
    } else if (240<=h && h<360){
        result=0.0f;
    }
    if (0<=h && h<60){
        result=h/60.0f;
    }else if (180<=h && h<240){
        result=4.0f-h/60.0f;
    }
    return result;
}

void HSVtoRGB(float Vec[3], float rgb[3], int inV, int inH ){
    float S=sqrt(Vec[0]*Vec[0]+Vec[1]*Vec[1]+Vec[2]*Vec[2]);
    float F = atan2(Vec[1]/S,Vec[0]/S);
    float H = inH*360+(1-2*inH)*(F > 0 ? F : (2*pi + F))*R2D;
    float maxV, minV, dV;
    if ((1-2*inV)*Vec[2]/S<0){
        // maxV = 1 - fabs(Vec[2]/S);
        maxV = 1 - fabs(Vec[2]/S);
        minV=0;
    }else{
        maxV = 1;
        minV = fabs(Vec[2]/S);       
    }
    maxV = 0.5+S*(maxV-0.5);
    minV = 0.5-S*(0.5-minV);


    dV = maxV-minV;

    

    rgb[0] = color_function(H+120)*dV+minV;//rad
    rgb[1] = color_function(H    )*dV+minV;//green
    rgb[2] = color_function(H-120)*dV+minV;//blue
    
}



void SaveImageM(double *Sx, double *Sy, double *Sz, char *outputfilename,int Nx,int Ny){

   int Scale = 1;
  bitmap_image image(Scale*Nx,Scale*Ny);
  // bitmap_image image(Nx,Ny);
   
  image.set_all_channels(0, 0, 0); // set background to black
  image_drawer draw(image);

  float rgb[3],vec[3]; 
  int BoxCol=1;

   for(int i = 0; i < Nx; i++){
      for(int j = 0; j < Ny; j++){
         int n = i+j*Nx;

         rgb[0] = 0; rgb[1] = 0; rgb[2] = 0;
         vec[0] = Sx[n]; vec[1] = Sy[n]; vec[2] = Sz[n];
         
         HSVtoRGB(vec, rgb, 0, 1);//1,0
          
         
         for(int ix = 0; ix<Scale; ix++){
            for(int iy = 0; iy<Scale; iy++){
                image.set_pixel(Scale*i+ix,   Scale*(Ny-j-1)+iy,   rgb[0]*255, rgb[1]*255, rgb[2]*255);
            }
         }
         // image.set_pixel(i, Ny-j-1, rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i, 3*(Ny-j-1), rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i+1, 3*(Ny-j-1), rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i+2, 3*(Ny-j-1), rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i, 3*(Ny-j-1)+1, rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i, 3*(Ny-j-1)+2, rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i+1, 3*(Ny-j-1)+1, rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i+1, 3*(Ny-j-1)+2, rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i+2, 3*(Ny-j-1)+1, rgb[0]*255, rgb[1]*255, rgb[2]*255);
         // image.set_pixel(3*i+2, 3*(Ny-j-1)+2, rgb[0]*255, rgb[1]*255, rgb[2]*255);

         

         
      }
   }
      
      image.save_image(outputfilename);
      // printf("Image saved to %s\n",outputfilename );
}


void WriteImageGB(double** g, bool** b,int** Dimension, int slice, char bmp_filename[100])
{  
   int Nx = Dimension[0][0];
   int Ny = Dimension[0][1];
   int Nz = Dimension[0][2];
   int NumOfShelsX = Dimension[1][0];
   int NumOfShelsY = Dimension[1][1];
   int NumOfShelsZ = Dimension[1][2];
   int na = Dimension[2][0];
   int nb = Dimension[2][1];
   int nc = Dimension[2][2];

   int N1 = 2*NumOfShelsX+Nx/na;
   int N2 = 2*NumOfShelsY+Ny/nb;
   int N3 = 2*NumOfShelsZ+Nz/nc;

   int N = Nx*Ny;
   int NumOfThreads = na*nb*nc;
   int Ns = N1*N2*N3;
   double* Sx = new double [N];
   double* Sy = new double [N];
   double* Sz = new double [N];
   for(int it = 0; it<na; it++){
      for(int jt = 0; jt<nb; jt++){
         for(int kt = 0; kt<nc; kt++){
            for(int i = NumOfShelsX; i<=Nx/na - 1 + NumOfShelsX; i++){
               for(int j = NumOfShelsY; j<=Ny/nb - 1 + NumOfShelsY; j++){
                  for(int k = NumOfShelsZ; k<=Nz/nc - 1 + NumOfShelsZ; k++){
                    if((k - NumOfShelsZ + kt*Nz/nc)==slice){
                        int n = i - NumOfShelsX + it*Nx/na + (j - NumOfShelsY + jt*Ny/nb)*Nx;
                         int nt = it+jt*na+kt*na*nb;
                         int pos = i+j*N1+k*N1*N2;
                         double g1 = g[nt][pos], g2 = g[nt][pos+Ns];
                         double ss = (b[nt][pos])? 1.0 : -1.0;
                         double gg = 1./(1.0+g1*g1+g2*g2); 
                         Sx[n] = 2.0*g1*gg;
                         Sy[n] = 2.0*g2*gg;
                         Sz[n] = ss*(1.0-g1*g1-g2*g2)*gg;
                    }
                     
                  }
               }
            }
         }
      }
   }
   SaveImageM(Sx,Sy,Sz,bmp_filename,Nx,Ny);
   delete [] Sx; Sx = 0;
   delete [] Sy; Sy = 0;
   delete [] Sz; Sz = 0;
}




void BP_ini(double** g, bool** b,int** Dimension, double** Parameters){
    
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;
    double kappa = Parameters[7][1], t, f, p;
    double pref = pi*sqrt(3./(24-pi*pi))*sqrt(2.*kappa);
    double nx, ny,nz,nv, ss,mx,my,mz;

    for(int i = 0; i<Nx; i++){
        for(int j = 0; j<Ny; j++){
            for(int k = 0; k<Nz; k++){


            double rx = -0.5*Parameters[4][0] + Parameters[4][0]*i/Dimension[0][0]+1e-8;
            double ry = -0.5*Parameters[4][1] + Parameters[4][1]*j/Dimension[0][1]+1e-8;
            double rz = -0.5*Parameters[4][2] + Parameters[4][2]*k/Dimension[0][2]+1e-8;
            double r1 = sqrt(rx*rx+ry*ry), r2 = sqrt(rx*rx+ry*ry+rz*rz);

            int it = (int) i*na/Nx;
            int jt = (int) j*nb/Ny;
            int kt = (int) k*nc/Nz;

            int nt = it + jt*na + kt*na*nb;
            int pos = i - it*Nx/na + NumOfShelsX + (j - jt*Ny/nb + NumOfShelsY)*N1 + (k - kt*Nz/nc + NumOfShelsZ)*N1*N2;


            ///Skyrme spherical in the cone phase
            t = 1.0*M_PI*(1.0-r2);
            if(r2 > 1.0) t = 0.0;

            f = atan2(ry, rx) + 0.5*M_PI + tpi*rz;

            nx = cos(f)*sin(t);
            ny = sin(f)*sin(t);
            nv = rz*sin(t)/r2;
            nz = cos(t);

            //cone phase
            // nx = 0.0; ny = 0.0; nz = 0.8; nv = 0.6;

            double Th = 0.5*M_PI;
            double mx = nx*cos(Th) - nz*sin(Th);
            double mz = nz*cos(Th) + nx*sin(Th);
            double my = ny;

            nx = mx*cos(tpi*rz) - my*sin(tpi*rz);
            ny = my*cos(tpi*rz) + mx*sin(tpi*rz);
            nz = mz;

            ///End Skyrme

            ss = (nv>0)? 1.0 : -1.0;
            b[nt][pos] = (nv>0)? true : false;
            nv = 1./(1.+ss*nv);
            g[nt][pos]      = nx*nv; 
            g[nt][pos+Ns]   = ny*nv;
            g[nt][pos+2*Ns] = nz*nv;


         }
      }
   }

}







void UpdateBoundaryGB(double** g, bool** b, int** Dimension, int dim)
{
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;
    for(int nt = 0; nt<NumOfThreads; nt++){
        int kt = (int)nt/(na*nb);
        int jt = (int)(nt - kt*na*nb)/na;
        int it = nt-jt*na-kt*na*nb;
        // printf("(%i, %i, %i)\n",it,jt,kt );
        //boundary in x-direction
        for(int ks = 0; ks<NumOfShelsX; ks++){
            int ntL = (it-1+na)%na + jt*na + kt*na*nb;
            int ntG = (it+1)%na    + jt*na + kt*na*nb;
            for(int j = NumOfShelsY; j<Ny/nb + NumOfShelsY; j++){
                for(int k = NumOfShelsZ; k<Nz/nc + NumOfShelsZ; k++){
                    int n1 = ks + j*N1 + k*N1*N2;
                    int n2 = Nx/na + ks + j*N1+k*N1*N2;
                    int n3 = Nx/na + NumOfShelsX + ks + j*N1+k*N1*N2;
                    int n4 = NumOfShelsX + ks + j*N1+k*N1*N2;
                    for(int n = 0; n < dim*Ns; n+=Ns){
                        g[nt][n1 + n] = g[ntL][n2 + n];
                        g[nt][n3 + n] = g[ntG][n4 + n];
                    }
                    b[nt][n1] = b[ntL][n2];
                    b[nt][n3] = b[ntG][n4];
                }
            }
        }

        //boundary in y-direction
        for(int ks = 0; ks<NumOfShelsY; ks++){
            int ntL = it + ((jt-1+nb)%nb)*na + kt*na*nb;
            int ntG = it + ((jt+1)%nb)*na    + kt*na*nb;
            for(int i = NumOfShelsX; i<Nx/na + NumOfShelsX; i++){
                for(int k = NumOfShelsZ; k<Nz/nc + NumOfShelsZ; k++){
                    int n1 = i + ks*N1 + k*N1*N2;
                    int n2 = i + (Ny/nb + ks)*N1+k*N1*N2;
                    int n3 = i + (Ny/nb + NumOfShelsY + ks)*N1+k*N1*N2;
                    int n4 = i + (NumOfShelsY + ks)*N1+k*N1*N2;
                    for(int n = 0; n < dim*Ns; n+=Ns){
                        g[nt][n1 + n] = g[ntL][n2 + n];
                        g[nt][n3 + n] = g[ntG][n4 + n];
                    }
                    b[nt][n1] = b[ntL][n2];
                    b[nt][n3] = b[ntG][n4];
                }
            }
        }

        //boundary in z-direction
        for(int ks = 0; ks<NumOfShelsZ; ks++){
            int ntL = it + jt*na + ((kt-1+nc)%nc)*na*nb;
            int ntG = it + jt*na + ((kt+1)%nc)*na*nb;
            for(int i = NumOfShelsX; i<Nx/na + NumOfShelsX; i++){
                for(int j = NumOfShelsY; j<Ny/nb + NumOfShelsY; j++){
                    int n1 = i + j*N1 + ks*N1*N2;
                    int n2 = i + j*N1 + (Nz/nc + ks)*N1*N2;
                    int n3 = i + j*N1 + (Nz/nc + NumOfShelsZ + ks)*N1*N2;
                    int n4 = i + j*N1 + (NumOfShelsZ + ks)*N1*N2;
                    // for(int n = 0; n < 3*Ns; n+=Ns){
                    //  if(bc[2]){
                    //      s[nt][n1 + n] = s[ntL][n2 + n];
                    //      s[nt][n3 + n] = s[ntG][n4 + n];
                    //  }else if(kt == 0){
                    //      s[nt][n3 + n] = s[ntG][n4 + n];
                    //  }else if(kt == nc-1){
                    //      s[nt][n1 + n] = s[ntL][n2 + n];
                    //  }
                        
                    // }
                    for(int n = 0; n < dim*Ns; n+=Ns){
                        g[nt][n1 + n] = g[ntL][n2 + n];
                        g[nt][n3 + n] = g[ntG][n4 + n];
                    }
                    b[nt][n1] = b[ntL][n2];
                    b[nt][n3] = b[ntG][n4];
                }
            }
        }



    }
}






auto EffectiveFieldGB_4D(double** g, bool** b, double** de, int** Dimension, double** Parameters,double* JD,int nt)
{
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int kk;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;
    int num = NumOfShelsX+NumOfShelsY+NumOfShelsZ;
    double dnx, dny, dnz, dnv;
    double nx,ny,nz,nv, mx,my,mz,mv,px,py,pz,pv;
    double g1, g2,g3,gi, ss, gg;

    bool pbcx = (Dimension[8][0])? true : false;
    bool pbcy = (Dimension[8][1])? true : false;
    bool pbcz = (Dimension[8][2])? true : false;
    int kt = (int)(nt/(na*nb));
    int jt = (int)(nt - kt*na*nb)/na;
    int it = nt-jt*na-kt*na*nb;

    

    double Lx = Parameters[4][0], Ly = Parameters[4][1], Lz = Parameters[4][2];
    double dx = Lx/Nx, dy = Ly/Ny, dz = Lz/Nz;
    double hx = Parameters[3][0], hy = Parameters[3][1], hz = Parameters[3][2];
    double kx = Parameters[2][0], ky = Parameters[2][1], kz = Parameters[2][2];
    double Jx = Parameters[0][0], Jy = Parameters[0][1], Jz = Parameters[0][2];
    double Dx = Parameters[1][0], Dy = Parameters[1][1], Dz = Parameters[1][2];
    double h = sqrt(hx*hx+hy*hy+hz*hz);
    double he = tpi*tpi*dx*dy*dz, kappa = Parameters[7][1], ku = Parameters[7][2];

    bool MicroBC = (Dimension[11][1])? true : false;
    de[nt][0] = 0.0;
    for(int i = NumOfShelsX; i < Nx/na + NumOfShelsX; i++){
        for(int j = NumOfShelsY; j < Ny/nb + NumOfShelsY; j++){
            for(int k = NumOfShelsZ; k < Nz/nc + NumOfShelsZ; k++){ 
                //single spin interactions: Zeeman + anisotropy
                int pos = i+j*N1+k*N1*N2;
                g1 = g[nt][pos];
                g2 = g[nt][pos+Ns];
                g3 = g[nt][pos+2*Ns];
                ss = (b[nt][pos])? 1.0 : -1.0;
                gg = g1*g1+g2*g2+g3*g3; 
                gi = 1./(1.+gg);
                px = 2.0*g1*gi;
                py = 2.0*g2*gi;
                pz = 2.0*g3*gi;
                pv = ss*(1.0-gg)*gi;

                dnx = -he*(hx+2.*kx*px);
                dny = -he*(hy+2.*ky*py);
                dnz = -he*(hz+2.*kz*pz);
                dnv =  he*2.*kappa*pv - he*ku*64.0*pv*(16.0*pv*pv-13.0)/9.0;

                // double nv4 = (16.0*nv*nv-13.0)*(16.0*nv*nv-13.0)/9.0;
                // res += res0*he*((h-hx*nx-hy*ny-hz*nz) + (kz-kx*nx*nx-ky*ny*ny-kz*nz*nz) + kappa*nv*nv - ku*nv4);

                //for open BC only
                if(MicroBC){
                if(!pbcx && it == 0 && i == NumOfShelsX){
                    dnx *= 0.5;   dny *= 0.5;   dnz *= 0.5; dnv *= 0.5; 
                }
                if(!pbcx && it == (na-1) && i == (Nx/na + NumOfShelsX-1)){
                    dnx *= 0.5;   dny *= 0.5;   dnz *= 0.5; dnv *= 0.5; 
                }
                if(!pbcy && jt == 0 && j == NumOfShelsY){
                    dnx *= 0.5;   dny *= 0.5;   dnz *= 0.5; dnv *= 0.5; 
                }
                if(!pbcy && jt == (nb-1) && j == (Ny/nb + NumOfShelsY-1)){
                    dnx *= 0.5;   dny *= 0.5;   dnz *= 0.5; dnv *= 0.5; 
                }
                if(!pbcz && kt == 0 && k == NumOfShelsZ){
                    dnx *= 0.5;   dny *= 0.5;   dnz *= 0.5; dnv *= 0.5;
                }
                if(!pbcz && kt == (nc-1) && k == (Nz/nc + NumOfShelsZ-1)){
                    dnx *= 0.5;   dny *= 0.5;   dnz *= 0.5; dnv *= 0.5;
                }
                }
                   

                //pairwise spin interactions: Exchange and DMI
                //x-neighbours
                for(int q = 0; q < NumOfShelsX; q++){
                    int n = i-q-1 + j*N1 + k*N1*N2;
                    g1 = g[nt][n];
                    g2 = g[nt][n+Ns];
                    g3 = g[nt][n+2*Ns];
                    ss = (b[nt][n])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg); 
                    mx = 2.0*g1*gi;
                    my = 2.0*g2*gi;
                    mz = 2.0*g3*gi;
                    mv = ss*(1.0-gg)*gi;
                    if(!pbcx && it == 0 && i == NumOfShelsX){
                        mx *= 0.;   my *= 0.;   mz *= 0.;   mv *= 0.;
                    }

                    n = i+q+1 + j*N1 + k*N1*N2;
                    g1 = g[nt][n];
                    g2 = g[nt][n+Ns];
                    g3 = g[nt][n+2*Ns];
                    ss = (b[nt][n])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg); 
                    nx = 2.0*g1*gi;
                    ny = 2.0*g2*gi;
                    nz = 2.0*g3*gi;
                    nv = ss*(1.0-gg)*gi;
                    if(!pbcx && it == (na-1) && i == (Nx/na + NumOfShelsX-1)){
                        nx *= 0.;   ny *= 0.;   nz *= 0.;   nv *= 0.;
                    }
                    double prefJ = -dy*dz*JD[q]*Jx/dx; 
                    double prefD = tpi*dy*dz*JD[q+num]*Dx;

                    dnx += prefJ*(nx + mx) + prefD*(pv-nv);
                    dny += prefJ*(ny + my) + prefD*(mz-nz);
                    dnz += prefJ*(nz + mz) + prefD*(ny-my);
                    dnv += prefJ*(nv + mv) + prefD*(px-mx);


                    // res += Jx*prefJ*(nx*mx+ny*my+nz*mz+nv*mv-1.0) + Dx*prefD*(ny*mz-nz*my + nv*(nx-mx));
                }
                //y-neighbours
                for(int q = 0; q < NumOfShelsY; q++){
                    int n = i + (j-q-1)*N1 + k*N1*N2;
                    g1 = g[nt][n];
                    g2 = g[nt][n+Ns];
                    g3 = g[nt][n+2*Ns];
                    ss = (b[nt][n])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg); 
                    mx = 2.0*g1*gi;
                    my = 2.0*g2*gi;
                    mz = 2.0*g3*gi;
                    mv = ss*(1.0-gg)*gi;
                    if(!pbcy && jt == 0 && j == NumOfShelsY){
                        mx *= 0.;   my *= 0.;   mz *= 0.;   mv *= 0.;
                    }

                    n = i + (j+q+1)*N1 + k*N1*N2;
                    g1 = g[nt][n];
                    g2 = g[nt][n+Ns];
                    g3 = g[nt][n+2*Ns];
                    ss = (b[nt][n])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg); 
                    nx = 2.0*g1*gi;
                    ny = 2.0*g2*gi;
                    nz = 2.0*g3*gi;
                    nv = ss*(1.0-gg)*gi;
                    if(!pbcy && jt == (nb-1) && j == (Ny/nb + NumOfShelsY-1)){
                        nx *= 0.;   ny *= 0.;   nz *= 0.;   nv *= 0.;
                    }
                    double prefJ = -dx*dz*JD[q+NumOfShelsX]*Jy/dy; 
                    double prefD = tpi*dx*dz*JD[q+NumOfShelsX+num]*Dy;

                    
                

                    dnx += prefJ*(nx + mx) + prefD*(nz-mz);
                    dny += prefJ*(ny + my) + prefD*(pv-nv); 
                    dnz += prefJ*(nz + mz) + prefD*(mx-nx); 
                    dnv += prefJ*(nv + mv) + prefD*(py-my);
                }
                //z-neighbours
                for(int q = 0; q < NumOfShelsZ; q++){
                    int n = i + j*N1 + (k-q-1)*N1*N2;
                    g1 = g[nt][n];
                    g2 = g[nt][n+Ns];
                    g3 = g[nt][n+2*Ns];
                    ss = (b[nt][n])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg); 
                    mx = 2.0*g1*gi;
                    my = 2.0*g2*gi;
                    mz = 2.0*g3*gi;
                    mv = ss*(1.0-gg)*gi;
                    double prefJ = -dx*dy*JD[q+NumOfShelsX+NumOfShelsY]*Jz/dz; 
                    double prefD = tpi*dx*dy*JD[q+NumOfShelsX+NumOfShelsY+num]*Dz;
                    if(!pbcz && kt == 0 && k == NumOfShelsZ){
                        mx *= 0.;   my *= 0.;   mz *= 0.; mv *= 0.; 
                    }

                    n = i + j*N1 + (k+q+1)*N1*N2;
                    g1 = g[nt][n];
                    g2 = g[nt][n+Ns];
                    g3 = g[nt][n+2*Ns];
                    ss = (b[nt][n])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg); 
                    nx = 2.0*g1*gi;
                    ny = 2.0*g2*gi;
                    nz = 2.0*g3*gi;
                    nv = ss*(1.0-gg)*gi;

                    
                    if(!pbcz && kt == (nc-1) && k == (Nz/nc + NumOfShelsZ-1)){
                        nx *= 0.;   ny *= 0.;   nz *= 0.; nv *= 0.; 
                    }
                    
                    
                    dnx += prefJ*(nx + mx) + prefD*(my-ny);
                    dny += prefJ*(ny + my) + prefD*(nx-mx);
                    dnz += prefJ*(nz + mz) + prefD*(pv-nv);
                    dnv += prefJ*(nv + mv) + prefD*(pz-mz);

                }
                ss = (b[nt][pos])? 1.0 : -1.0;

                de[nt][pos]         = (-dnx*(1.0+ss*pv - px*px) + dny*px*py + dnz*px*pz + dnv*px*(ss+pv));
                de[nt][pos+Ns]      = (dnx*px*py - dny*(1.0+ss*pv - py*py) + dnz*py*pz + dnv*py*(ss+pv));
                de[nt][pos+2*Ns]    = (dnx*px*pz + dny*py*pz - dnz*(1.0+ss*pv - pz*pz) + dnv*pz*(ss+pv));
                
                de[nt][0] += de[nt][pos]*de[nt][pos] + de[nt][pos+Ns]*de[nt][pos+Ns] + de[nt][pos+2*Ns]*de[nt][pos+2*Ns];
            }
        }
    }

}

double GetEffectiveFieldGB_4D(double** g, bool** b,double** de,int** Dimension, double** Parameters,double* JD){

    int NumOfThreads = Dimension[4][1];
    vector<future<void>> futureVec(NumOfThreads);

    for(int n = 0; n < NumOfThreads; ++n)
        futureVec[n] = async(EffectiveFieldGB_4D,g,b,de,Dimension,Parameters,JD,n);
    for(int n = 0; n < NumOfThreads; ++n)
        futureVec[n].get();

    double grad = 0.0;
    for(int n = 0; n < NumOfThreads; ++n)
        grad += de[n][0];

    return grad;
}





auto Magnetization_4D(double** g, bool **b,  int** Dimension, int nt)
{
   int Nx = Dimension[0][0];
   int Ny = Dimension[0][1];
   int Nz = Dimension[0][2];
   int NumOfShelsX = Dimension[1][0];
   int NumOfShelsY = Dimension[1][1];
   int NumOfShelsZ = Dimension[1][2];
   int na = Dimension[2][0];
   int nb = Dimension[2][1];
   int nc = Dimension[2][2];

   int N1 = 2*NumOfShelsX+Nx/na;
   int N2 = 2*NumOfShelsY+Ny/nb;
   int N3 = 2*NumOfShelsZ+Nz/nc;

   int N = Nx*Ny*Nz;
   int NumOfThreads = na*nb*nc;
   int Ns = N1*N2*N3;

   double Mx = 0.0, My = 0.0, Mz = 0.0, Mv = 0.0;
   double g1, g2,g3, ss,gg, gi;
   for(int i = NumOfShelsX; i < Nx/na + NumOfShelsX; i++){
      for(int j = NumOfShelsY; j < Ny/nb + NumOfShelsY; j++){
         for(int k = NumOfShelsZ; k < Nz/nc + NumOfShelsZ; k++){
            int pos = i+j*N1+k*N1*N2;
            g1 = g[nt][pos];
            g2 = g[nt][pos+Ns];
            g3 = g[nt][pos+2*Ns];
            ss = (b[nt][pos])? 1.0 : -1.0;
            gg = g1*g1+g2*g2+g3*g3;
            gi = 1./(1.+gg); 
            Mx += 2.0*g1*gi;
            My += 2.0*g2*gi;
            Mz += 2.0*g3*gi;
            Mv += (ss*(1.0-gg)*gi);
         }
      }
   }
   g[nt][0] = Mx; g[nt][Ns] = My; g[nt][2*Ns] = Mz;  g[nt][Nx/na + NumOfShelsX] = Mv;
}

void GetMagnetization_4D(double** g, bool** b, int** Dimension){
    int NumOfThreads = Dimension[4][1];
    int Ns = Dimension[4][2];
    int Nx = Dimension[0][0];
    int na = Dimension[2][0];
    int NumOfShelsX = Dimension[1][0];
    vector<future<void>> futureVec(NumOfThreads);
    for(int n = 0; n < NumOfThreads; ++n)
        futureVec[n] = async(Magnetization_4D,g,b,Dimension,n);
    for(int n = 0; n < NumOfThreads; ++n)
        futureVec[n].get();
   
    for(int n = 1; n < NumOfThreads; ++n){
        g[0][0]     += g[n][0];
        g[0][Ns]    += g[n][Ns];
        g[0][2*Ns]  += g[n][2*Ns];
        g[0][Nx/na + NumOfShelsX] += g[n][Nx/na + NumOfShelsX];
    }
}











auto EnergyGB_4D(double** g, bool** b, int** Dimension, double** Parameters, double* JD, int nt)
{  
    int Nx = Dimension[0][0];
    int Ny = Dimension[0][1];
    int Nz = Dimension[0][2];
    int NumOfShelsX = Dimension[1][0];
    int NumOfShelsY = Dimension[1][1];
    int NumOfShelsZ = Dimension[1][2];
    int na = Dimension[2][0];
    int nb = Dimension[2][1];
    int nc = Dimension[2][2];

    int N1 = 2*NumOfShelsX+Nx/na;
    int N2 = 2*NumOfShelsY+Ny/nb;
    int N3 = 2*NumOfShelsZ+Nz/nc;

    int N = Nx*Ny*Nz;
    int NumOfThreads = na*nb*nc;
    int Ns = N1*N2*N3;
    int num = NumOfShelsX+NumOfShelsY+NumOfShelsZ;
    double res = 0.0;
    double nx,ny,nz, nv, mx,my,mz, mv;
    double g1, g2, g3, ss, gg, gi;

    bool pbcx = (Dimension[8][0])? true : false;
    bool pbcy = (Dimension[8][1])? true : false;
    bool pbcz = (Dimension[8][2])? true : false;
    int kt = (int)(nt/(na*nb));
    int jt = (int)(nt - kt*na*nb)/na;
    int it = nt-jt*na-kt*na*nb;

    int kk;
    bool MicroBC = (Dimension[11][1])? true : false;

    double Lx = Parameters[4][0], Ly = Parameters[4][1], Lz = Parameters[4][2];
    double dx = Lx/Nx, dy = Ly/Ny, dz = Lz/Nz;
    double hx = Parameters[3][0], hy = Parameters[3][1], hz = Parameters[3][2];
    double kx = Parameters[2][0], ky = Parameters[2][1], kz = Parameters[2][2];
    double Jx = Parameters[0][0], Jy = Parameters[0][1], Jz = Parameters[0][2];
    double Dx = Parameters[1][0], Dy = Parameters[1][1], Dz = Parameters[1][2];
    double h = sqrt(hx*hx+hy*hy+hz*hz);
    double he = tpi*tpi*dx*dy*dz;
    double kappa = Parameters[7][1], ku = Parameters[7][2];
   
    for(int k = NumOfShelsZ; k < Nz/nc + NumOfShelsZ; k++){
        for(int j = NumOfShelsY; j < Ny/nb + NumOfShelsY; j++){
            for(int i = NumOfShelsX; i < Nx/na + NumOfShelsX; i++){
            //single spin interactions: Zeeman+anisotropy
            int pos = i+j*N1+k*N1*N2;
            g1 = g[nt][pos];
            g2 = g[nt][pos+Ns];
            g3 = g[nt][pos+2*Ns];
            ss = (b[nt][pos])? 1.0 : -1.0;
            gg = g1*g1+g2*g2+g3*g3;
            gi = 1./(1.+gg); 
            nx = 2.0*g1*gi;
            ny = 2.0*g2*gi;
            nz = 2.0*g3*gi;
            nv = ss*(1.0-gg)*gi;

            double res0 = 1.0;

            if(MicroBC){
            if(!pbcx && it == 0 && i == NumOfShelsX){
                res0 *= 0.5; 
            }
            if(!pbcx && it == (na-1) && i == (Nx/na + NumOfShelsX-1)){
                res0 *= 0.5; 
            }
            if(!pbcy && jt == 0 && j == NumOfShelsY){
                res0 *= 0.5; 
            }
            if(!pbcy && jt == (nb-1) && j == (Ny/nb + NumOfShelsY-1)){
                res0 *= 0.5; 
            }
            if(!pbcz && kt == 0 && k == NumOfShelsZ){
                res0 *= 0.5;
            }
            if(!pbcz && kt == (nc-1) && k == (Nz/nc + NumOfShelsZ-1)){
                res0 *= 0.5;
            }
            }
            double nv4 = (16.0*nv*nv-13.0)*(16.0*nv*nv-13.0)/9.0;
            res += res0*he*((h-hx*nx-hy*ny-hz*nz) + (kz-kx*nx*nx-ky*ny*ny-kz*nz*nz) + kappa*nv*nv - ku*nv4);

            //pairwise spin interactions: Exchange and DMI
            //x-neighbours
            for(int q = 0; q < NumOfShelsX; q++){
                int n = i-q-1 + j*N1 + k*N1*N2;
                g1 = g[nt][n];
                g2 = g[nt][n+Ns];
                g3 = g[nt][n+2*Ns];
                ss = (b[nt][n])? 1.0 : -1.0;
                gg = g1*g1+g2*g2+g3*g3;
                gi = 1./(1.+gg); 
                mx = 2.0*g1*gi;
                my = 2.0*g2*gi;
                mz = 2.0*g3*gi;
                mv = ss*(1.0-gg)*gi;
                double prefJ = -dy*dz*JD[q]/dx; 
                double prefD = tpi*dy*dz*JD[q+num];
                if(!pbcx && it == 0 && i == NumOfShelsX){
                    prefJ *= 0.;   prefD *= 0.;   
                }
                res += Jx*prefJ*(nx*mx+ny*my+nz*mz+nv*mv-1.0) + Dx*prefD*(ny*mz-nz*my + nv*(nx-mx));

            }
            //y-neighbours
            for(int q = 0; q < NumOfShelsY; q++){
                int n = i + (j-q-1)*N1 + k*N1*N2;
                g1 = g[nt][n];
                g2 = g[nt][n+Ns];
                g3 = g[nt][n+2*Ns];
                ss = (b[nt][n])? 1.0 : -1.0;
                gg = g1*g1+g2*g2+g3*g3;
                gi = 1./(1.+gg); 
                mx = 2.0*g1*gi;
                my = 2.0*g2*gi;
                mz = 2.0*g3*gi;
                mv = ss*(1.0-gg)*gi;              
                double prefJ = -dx*dz*JD[q+NumOfShelsX]/dy; 
                double prefD = tpi*dx*dz*JD[q+NumOfShelsX+num];
                if(!pbcy && jt == 0 && j == NumOfShelsY){
                    prefJ *= 0.;   prefD *= 0.;  
                }

               
                res += Jy*prefJ*(nx*mx+ny*my+nz*mz+nv*mv-1.0)+Dy*prefD*(nz*mx-nx*mz  + nv*(ny-my));
            }
            //z-neighbours
            for(int q = 0; q < NumOfShelsZ; q++){
                int n = i + j*N1 + (k-q-1)*N1*N2;
                g1 = g[nt][n];
                g2 = g[nt][n+Ns];
                g3 = g[nt][n+2*Ns];
                ss = (b[nt][n])? 1.0 : -1.0;
                gg = g1*g1+g2*g2+g3*g3;
                gi = 1./(1.+gg); 
                mx = 2.0*g1*gi;
                my = 2.0*g2*gi;
                mz = 2.0*g3*gi;
                mv = ss*(1.0-gg)*gi;
                double prefJ = -dx*dy*JD[q+NumOfShelsX+NumOfShelsY]/dz; 
                double prefD = tpi*dx*dy*JD[q+NumOfShelsX+NumOfShelsY+num];
                if(!pbcz && kt == 0 && k == NumOfShelsZ){
                    prefJ *= 0.;   prefD *= 0.;   
                }
                res += Jz*prefJ*(nx*mx+ny*my+nz*mz+nv*mv-1.0) + Dz*prefD*(nx*my-ny*mx + nv*(nz-mz));
            }
            
         }
      }
   }
   g[nt][0]=res;
}


double GetEnergyGB_4D(double** g, bool** b, int** Dimension, double** Parameters,double* JD){

   int NumOfThreads = Dimension[4][1];
   vector<future<void>> futureVec(NumOfThreads);

   double res = 0;
   for(int n = 0; n < NumOfThreads; ++n)
      futureVec[n] = async(EnergyGB_4D,g,b,Dimension,Parameters,JD,n);
   for(int n = 0; n < NumOfThreads; ++n)
      futureVec[n].get();
   for(int n = 0; n < NumOfThreads; ++n)
      res += g[n][0];
   return (res);
}




auto MoveAlongGradGB4D(double** g, bool** b,double** de, bool* remap,double step, int** Dimension, int nt){
    double g1, g2, g3;
    remap[nt] = true;
   int Nx = Dimension[0][0];
   int Ny = Dimension[0][1];
   int Nz = Dimension[0][2];
   int NumOfShelsX = Dimension[1][0];
   int NumOfShelsY = Dimension[1][1];
   int NumOfShelsZ = Dimension[1][2];
   int na = Dimension[2][0];
   int nb = Dimension[2][1];
   int nc = Dimension[2][2];

   int N1 = 2*NumOfShelsX+Nx/na;
   int N2 = 2*NumOfShelsY+Ny/nb;
   int N3 = 2*NumOfShelsZ+Nz/nc;

   int N = Nx*Ny*Nz;
   int NumOfThreads = na*nb*nc;
   int Ns = N1*N2*N3;

   bool fbcx = (Dimension[10][0])? true : false;
   bool fbcy = (Dimension[10][1])? true : false;
   bool fbcz = (Dimension[10][2])? true : false;
   int kt = (int)(nt/(na*nb));
   int jt = (int)(nt - kt*na*nb)/na;
   int it = nt-jt*na-kt*na*nb;



    for(int k = NumOfShelsZ; k < Nz/nc + NumOfShelsZ; k++){
        for(int j = NumOfShelsY; j < Ny/nb + NumOfShelsY; j++){
            for(int i = NumOfShelsX; i < Nx/na + NumOfShelsX; i++){
         
            
                int pos = i+j*N1+k*N1*N2;

                bool cond = true;
                if(fbcx && ((it == 0 && i == NumOfShelsX) || (it == na-1 && i == Nx/na + NumOfShelsX - 1))) cond = false;
                if(fbcy && ((jt == 0 && j == NumOfShelsY) || (jt == nb-1 && j == Ny/nb + NumOfShelsY - 1))) cond = false;
                if(fbcz && ((kt == 0 && k == NumOfShelsZ) || (kt == nc-1 && k == Nz/nc + NumOfShelsZ - 1))) cond = false;

                if(cond){

                g[nt][pos]      += step*de[nt][pos];
                g[nt][pos+Ns]   += step*de[nt][pos+Ns];
                g[nt][pos+2*Ns] += step*de[nt][pos+2*Ns];

                g1 = g[nt][pos];
                g2 = g[nt][pos+Ns];
                g3 = g[nt][pos+2*Ns];
                if(remap[nt] && (abs(g1)>2.0 || abs(g2)>2.0 || abs(g3)>2.0)) remap[nt] = false;
                }
               
            }
        }
    }
}

void MakeOneStepGB4D(double** g, bool** b, double** de, bool* remap, double step, int** Dimension){

   int NumOfThreads = Dimension[4][1];
   vector<future<void>> futureVec(NumOfThreads);
   for(int n = 0; n < NumOfThreads; ++n)
         futureVec[n] = async(MoveAlongGradGB4D,g,b,de,remap,step,Dimension,n);
      for(int n = 0; n < NumOfThreads; ++n)
         futureVec[n].get();
}


auto MoveAlongGradGB_4D(double** g, bool** b,double** de, double step, int** Dimension, int nt, bool fix){

   int Nx = Dimension[0][0];
   int Ny = Dimension[0][1];
   int Nz = Dimension[0][2];
   int NumOfShelsX = Dimension[1][0];
   int NumOfShelsY = Dimension[1][1];
   int NumOfShelsZ = Dimension[1][2];
   int na = Dimension[2][0];
   int nb = Dimension[2][1];
   int nc = Dimension[2][2];

   int N1 = 2*NumOfShelsX+Nx/na;
   int N2 = 2*NumOfShelsY+Ny/nb;
   int N3 = 2*NumOfShelsZ+Nz/nc;

   int N = Nx*Ny*Nz;
   int NumOfThreads = na*nb*nc;
   int Ns = N1*N2*N3;

   bool temp = true;

   double g1,g2,g3,gg,gi,ss,nx,ny,nz,nv;

    for(int k = NumOfShelsZ; k < Nz/nc + NumOfShelsZ; k++){
        for(int j = NumOfShelsY; j < Ny/nb + NumOfShelsY; j++){
            for(int i = NumOfShelsX; i < Nx/na + NumOfShelsX; i++){

                if(fix){
                    int kt = (int)nt/(na*nb);
                    int jt = (int)(nt - kt*na*nb)/na;
                    int it = nt-jt*na-kt*na*nb;

                    int ii = i - NumOfShelsX + it*Nx/na;
                    int jj = j - NumOfShelsY + jt*Ny/nb;
                    int kk = k - NumOfShelsZ + kt*Nz/nc;
                        
                    if((ii*jj*kk) == 0 || ii == (Nx-1) || jj == (Ny-1) || kk == (Nz-1)) temp = false;
                    }
         
                
                if(temp){
                    int pos = i+j*N1+k*N1*N2;

                    g[nt][pos]      += step*de[nt][pos];
                    g[nt][pos+Ns]   += step*de[nt][pos+Ns];
                    g[nt][pos+2*Ns] += step*de[nt][pos+2*Ns];

                    g1 = g[nt][pos]; g2 = g[nt][pos+Ns]; g3 = g[nt][pos+2*Ns];
                    ss = (b[nt][pos])? 1.0 : -1.0;
                    gg = g1*g1+g2*g2+g3*g3;
                    gi = 1./(1.+gg);
                    nx = 2.*g1*gi; ny = 2.*g2*gi; nz = 2.*g3*gi; nv = ss*(1.-gg)*gi;
                    b[nt][pos] = (nv>0.0)? true : false;
                    ss = (b[nt][pos])? 1.0 : -1.0;
                    nv = 1./(1.+ss*nv);
                    g[nt][pos] = nx*nv; g[nt][pos+Ns] = ny*nv; g[nt][pos+2*Ns] = nz*nv;
                }
                


               
            }
        }
    }
}

void MakeOneStepGB_4D(double** g, bool** b, double** de, double step, int** Dimension, bool fix){

   int NumOfThreads = Dimension[4][1];
   vector<future<void>> futureVec(NumOfThreads);
   for(int n = 0; n < NumOfThreads; ++n)
         futureVec[n] = async(MoveAlongGradGB_4D,g,b,de,step,Dimension,n, fix);
      for(int n = 0; n < NumOfThreads; ++n)
         futureVec[n].get();
}





void StepestDescent4D(double** g, bool** b, double** de, int** Dimension, double** Parameters, double* JD){
    int NumOfIter = Dimension[5][1];
    int Iter = Dimension[5][2];
    int Nx = Dimension[0][0];
    int NumOfShelsX = Dimension[1][0];
    int na = Dimension[2][0];
    int Ns = Dimension[4][2];

    double dt = Parameters[5][1], e0=1.0e10, energy;
    double grad, tol = Parameters[5][2];
    double dV = Parameters[4][0]*Parameters[4][1]*Parameters[4][2]/(Dimension[0][0]*Dimension[0][1]*Dimension[0][2]);

    FILE * pFile;
    pFile = fopen (ConvergenceInfoFile,"w");
    fclose (pFile);
    
    double Mx = 0.0, My = 0.0, Mz = 0.0, Mv = 0.0;
    for(int k = 0; k<NumOfIter; k++){
        UpdateBoundaryGB(g,b,Dimension,3);
        grad = GetEffectiveFieldGB_4D(g, b, de, Dimension, Parameters, JD)*dV;
        if(k%Iter==0){
            energy = GetEnergyGB_4D(g,b,Dimension,Parameters,JD);
            GetMagnetization_4D(g,b,Dimension);
            Mx = g[0][0]*dV; My = g[0][Ns]*dV; Mz = g[0][2*Ns]*dV; Mv = g[0][Nx/na + NumOfShelsX]*dV;
         
            printf("%d, %f, %0.3e, %f, %f, %f, %f\n", k/Iter, energy, grad, Mx, My, Mz, Mv);
            if(Dimension[6][1]){
                pFile = fopen (ConvergenceInfoFile,"a+");
                snprintf(BuferString,400,"%d, %0.20f, %0.20f, %0.20f, %0.20f, %0.20f, %0.20f\n",k/Iter,energy, grad,Mx,My,Mz,Mv);
                fputs (BuferString,pFile);
                fclose (pFile);
            }
            // if(Dimension[6][2]) WriteImage(s,Dimension,0,IntermediateBmpFile);
            if(Dimension[7][0]) WriteData2OVF(g,b,Dimension);
            if(k==0){
                e0 = energy;
            }else{
                if(e0<energy){
                    printf("Smth is wrong! Probably the step (dt) is too big.\n");
                    exit(-3);
                }
                else{
                    e0 = energy;
                }
            }
        }

        if(grad < tol){
            k = NumOfIter;
        }
      
        MakeOneStepGB_4D(g, b, de, dt, Dimension, false);
      
   }

    WriteData2OVF(g,b,Dimension);
    
}




int main()
{

int **Dimension = new int*[DIM];
for(int i = 0; i<DIM; i++){
	Dimension[i] = new int[3];
}
double **Parameters = new double*[PHYS];
for(int i = 0; i<PHYS; i++){
   Parameters[i] = new double[3];
}

double *tempJD = new double[2*3*MaxNumOfShells];
for(int i = 0; i<6*MaxNumOfShells; i++){
   tempJD[i] = 0.0;
}


readConfigFile(Dimension,Parameters,tempJD);
double *JD = new double[2*(Dimension[1][0]+Dimension[1][1]+Dimension[1][2])];

checkDimension(Dimension,Parameters,tempJD, JD);


int Nx = Dimension[0][0];
int Ny = Dimension[0][1];
int Nz = Dimension[0][2];
int NumOfShelsX = Dimension[1][0];
int NumOfShelsY = Dimension[1][1];
int NumOfShelsZ = Dimension[1][2];
int na = Dimension[2][0];
int nb = Dimension[2][1];
int nc = Dimension[2][2];

int N1 = 2*NumOfShelsX+Nx/na;
int N2 = 2*NumOfShelsY+Ny/nb;
int N3 = 2*NumOfShelsZ+Nz/nc;

int N = Nx*Ny*Nz;
int NumOfThreads = na*nb*nc;
int Ns = N1*N2*N3;


double Lx = Parameters[4][0], Ly = Parameters[4][1], Lz = Parameters[4][2];
double dx = Lx/Nx, dy = Ly/Ny, dz = Lz/Nz;
double dt = Parameters[5][1];





double time = 0.0;
auto start = chrono::system_clock::now();
auto stop = chrono::system_clock::now();

if(Dimension[6][0]==1){
    printf("Running SD for 4D model with dt = %f\n",Parameters[5][1]);
    double **DE  = new double*[NumOfThreads];
    double **G   = new double*[NumOfThreads];
    bool   **B   = new bool*[NumOfThreads];
    for(int i = 0; i < NumOfThreads; ++i){
        DE[i] = new double[3*Ns];
        G[i] = new double[3*Ns];
        B[i] = new bool[Ns];
    }
    printf("\n");
    if(Dimension[5][0]){
        char *inputfilename = NULL;
        int p = asprintf(&inputfilename, "%s.ovf2", InputOvf);  
        printf("Read initial state from %s\n", inputfilename);
        ReadOVF2(G,B,Dimension, inputfilename);
        free(inputfilename);
    }else{
        printf("Start from ansatz.\n");
        BP_ini(G,B,Dimension,Parameters);
    }
    printf("\n");

    start = chrono::system_clock::now();
    StepestDescent4D(G, B, DE, Dimension, Parameters, JD);
    stop = chrono::system_clock::now();
    time += chrono::duration<double>(stop-start).count();
    printf("Total time: %f\n",time);


    for(int i = 0; i < NumOfThreads; ++i)
        delete [] DE[i];
    delete [] DE;

    for(int i = 0; i < NumOfThreads; ++i)
        delete [] B[i];
    delete [] B;

    for(int i = 0; i < NumOfThreads; ++i)
        delete [] G[i];
    delete [] G;
}else{
    printf("Type of simulations is not choosen.\n");
}




for(int i = 0; i < DIM; ++i)
    delete [] Dimension[i];
delete [] Dimension;
for(int i = 0; i < PHYS; ++i)
    delete [] Parameters[i];
delete [] Parameters;
delete [] JD;

delete [] tempJD;
return 0;
}