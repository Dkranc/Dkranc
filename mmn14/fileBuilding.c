#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"

/*local functions*/
void buildFiles(char *fileName);
void createObj(char *fileName);
void createEnt(char *fileName);


/*global variables*/
char fileExt[maxLine];
FILE *fExt;
int extIsOpend=0;



/*build the output files*/
void buildFiles(char *fileName){
 char fileObj[maxLine];
 char fileEnt[maxLine];

 /*build the object file*/
 sprintf(fileObj, "%s%s", fileName, ".ob");
 createObj(fileObj);

 /*create the ent file*/
 sprintf(fileEnt, "%s%s", fileName, ".ent");
 createEnt(fileEnt);

 /*we need to delete all lists so next file can run!!*/
 prevLabel=NULL;
 headLabel=NULL;
 iteratorLabel=NULL;

 prevData=NULL;
 headData=NULL;
 iteratorData=NULL;

 prevInstruction=NULL;
 headInstruction=NULL;
 iteratorInstruction=NULL;

 prevWord=NULL;
 headWord=NULL;
 iteratorWord=NULL;
}



/*creates the object file*/
void createObj(char *fileName){
 FILE *fObj;
 char *wordVal;
 int address=100;

 /*open the object file*/
 fObj =fopen(fileName, "w");

 /*print the first line to the file*/
 fprintf(fObj, "		%d %d\n",FIC-100,DC);/*printing the final sizes of arrays to object file*/

 /*print the special base words to the file*/
 iteratorWord=headWord;
 while(iteratorWord!=NULL){
  wordVal=getWordVal(iteratorWord->val);/*get the special string type*/
  fprintf(fObj, "%04d %s\n",address,wordVal);/*adding code to file*/
  address++;

  /*clean word string for next line*/  
  strcpy(wordVal,"");
  iteratorWord=iteratorWord->next;
 }
 fclose(fObj);
}




/*creates the entry file if there are entries. */
void createEnt(char *fileName){
 FILE *fEnt;
 int hasEntry=0;

 /*see if there are any entry labels in the label list*/
 iteratorLabel=headLabel;
 while (iteratorLabel!=NULL){
  if(iteratorLabel->entOrExt==ent) hasEntry=1;
  iteratorLabel=iteratorLabel->next;
 }

 /*open the file if we have entries*/
 if(hasEntry==1){
  fEnt =fopen(fileName, "w");
   /*iterate the label list*/
   iteratorLabel=headLabel;
   while (iteratorLabel!=NULL){
    if(iteratorLabel->entOrExt==ent) {
       fprintf(fEnt, "%s,%04d,%04d\n",iteratorLabel->name,iteratorLabel->valBase,iteratorLabel->valOffset);/*adding address number to file with the labe lname*/
    }
    iteratorLabel=iteratorLabel->next;
   }
  fclose(fEnt);
 }
}



/*this function writes to the ext file. this function is called from the getters and setters file when we discover a ext label*/
void writeToExt(int address,char *labelName,char bOrO){
 
 if(extIsOpend==0){/*if file is closed we need to open it*/
  sprintf(fileExt, "%s%s", fileName, ".ext");
  fExt =fopen(fileExt, "w");
  extIsOpend=1;
  
  fprintf(fExt,"%s BASE %d\n",labelName,address+2); /*write the first line*/
 }
 else{/*if the file is already open*/
  if(bOrO=='B'){ /*base address*/
    fprintf(fExt,"%s BASE %d\n",labelName,address+2); 
  }
  else{/*if its an offset address*/
    fprintf(fExt,"%s OFFSET %d\n",labelName,address+3); 
  }
 }
}
