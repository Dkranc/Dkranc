#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"


/*this module has both reading methods in it. the reading methods go over the after macros file and build the machine language according to the assembly commands.*/

/*declaring global variables*/
int firstRead(char *fileName);
int secondRead(char *fileName);
int lineCounter=1;
int FIC=0;
int FDC=0;


/*the first read method reads the .am file by sending it to the line reader fucntion.
 if any error appears then the assembling procces will not continue after the first reading */
int firstRead(char *fileName){

 FILE *fp;
 char line[maxLine];
 int errorsFound=0;


 /*reding all the lines from the expanded file*/
 fp=fopen(fileName, "r");
 while ( fgets(line, maxLine-1, fp) != NULL){

  if(hasText(line)==0)/*if theres no text then we skip the line*/

   if(readLine(line)==1){/*the function read line returns 1 if theres an error*/
    printf("errors found in line: %d %s\n,check in expanded macro file to find line\n",lineCounter,line);
    errorsFound=1;
   }
  lineCounter++;
 }
 FIC=IC;/*get the final number of the instruction size*/

 printf("first read done\n");  
 

 if(errorsFound==1) printf("%s","errors found in file\n");/*if we found an error*/

 lineCounter=1;/*reset the counter for second read*/
 return errorsFound;

}



/*the second read function reads the .am file for the second time,and sends each line to the readlineTwo method that creats more data for our machine language*/
int secondRead(char *fileName){

 FILE *fp;
 char line[maxLine];
 int errorsFound=0;

 /*reding all the lines from the expanded file*/
 fp=fopen(fileName, "r");
 while ( fgets(line, maxLine-1, fp) != NULL){

  if(hasText(line)==0)
  if(readLineTwo(line)==1){/*if we found an error*/
    printf("errors found in line: %d ,check in expanded macro file to find line\n",lineCounter);
    errorsFound=1;
   }
   lineCounter++;
 }
 
 printf("second read done\n");   
 

 if(errorsFound==1) printf("%s","errors found in file\n");

 nillAddress();/*reset the address counter after we filled the word list with values*/

 lineCounter=0;/*reset the counter for the next file to come*/
 
 return errorsFound;
}

