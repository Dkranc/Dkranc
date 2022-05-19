#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "functions.h"


/*declare global variables to help us with data and instruction counting*/
int IC=100;
int DC=0;




/*initializing a linked list of lables*/
Label *prevLabel=NULL;
Label *headLabel=NULL;
Label *iteratorLabel=NULL;

/*initializing a linked list of Data nodes*/
Data *prevData=NULL;
Data *headData=NULL;
Data *iteratorData=NULL;

/*initializing a linked list of Instruction nodes*/
Instruction *prevInstruction=NULL;
Instruction *headInstruction=NULL;
Instruction *iteratorInstruction=NULL;


/*declaring local methods*/
int readLine(char* line);
int dealWithLabel(char *line);
int dealWithData(char *line,int isLabel);
int hasParantecies(char *para);
int isRegister(char *para);
int twoParaFunc(char* line,int isLabel,enum Func func);
int oneParaFunc(char* line,int isLabel,enum Func func);
int noParaFunc(char* line,int isLabel,enum Func func);
int dealWithExtern(char *line,int isLabel);
int partOfList(char *label);
int addWords(int addressTypeOne,int addressTypeTwo,char *firstPara,char *secondPara);



/*this method reads a line that is sent form the first reading of the expanded file.
and sends the function in the line to the right function in our program that can create instructions or data .*/
int readLine(char* line){
 char *firstWord;
 char *secondWord;
 int errorFound=0;
 int isLabel=0;

 enum Func func=-1;/*setting the func to -1 to be ready to accept a new function. if no function was located then we return an error*/


 if(checkIfComment(line)==1){/*we want to skip the line if its a comment*/
  return errorFound;
 }

 /*get the first and second word from the line*/
 firstWord=getFirstWord(line);
 secondWord=getSecondWord(line);/*used to get the next word so we know what to after the label*/

 /*check if we have a label defined in our line*/
 if(checkIfLabel(firstWord)==1){
   isLabel=1;
 } 

 /*check that both words are legal*/
 if(firstWord==NULL||secondWord==NULL) {
   if(secondWord==NULL&&(strcmp(firstWord,"stop")==0||strcmp(firstWord,"rts")==0)) {/*we check if the stop method was called*/
      if(strcmp(firstWord,"rts")==0)   func=rts;
      else func=stop;    
      errorFound=noParaFunc(line,isLabel,func);/*we want to check the special case of a stop or rts function that is just on word with no label*/
   }
   else  errorFound=1;/*if the second word was null and first one wasnt stop or rts , or if the first word was null*/
  }

 /*we now know that the words are legal so we check what function the user wnats to use*/
 else if(strcmp(firstWord,".data")==0||strcmp(firstWord,".string")==0||strcmp(secondWord,".data")==0||strcmp(secondWord,".string")==0){   
    errorFound=dealWithData(line,isLabel);/*sending the line to be read by the data interpriter*/
 }

 else if(strcmp(firstWord,".entry")==0||strcmp(secondWord,".entry")==0){
    if(isLabel==1) printf("waring: label defined on entry line\n");
    errorFound=checkLegalEntry(line,isLabel);/*we check that the entry line is valid*/
    if(errorFound==1) printf("ilegal entry statement: %s\n",line);
 }

 else if(strcmp(firstWord,".extern")==0||strcmp(secondWord,".extern")==0){
    if(isLabel==1) printf("waring: label defined on entry line\n");
    errorFound=dealWithExtern(line,isLabel);/*if we have extern we send the line to extern function*/
 }
 
 /*now we check what function is called*/ 
 else if(strcmp(firstWord,"mov")==0||strcmp(secondWord,"mov")==0)   func=mov;

 else if(strcmp(firstWord,"cmp")==0||strcmp(secondWord,"cmp")==0)   func=cmp;

 else if(strcmp(firstWord,"add")==0||strcmp(secondWord,"add")==0)   func=add;

 else if(strcmp(firstWord,"sub")==0||strcmp(secondWord,"sub")==0)   func=sub;

 else if(strcmp(firstWord,"lea")==0||strcmp(secondWord,"lea")==0)   func=lea;

 else if(strcmp(firstWord,"clr")==0||strcmp(secondWord,"clr")==0)   func=clr;

 else if(strcmp(firstWord,"not")==0||strcmp(secondWord,"not")==0)   func=not;

 else if(strcmp(firstWord,"inc")==0||strcmp(secondWord,"inc")==0)   func=inc;

 else if(strcmp(firstWord,"dec")==0||strcmp(secondWord,"dec")==0)   func=dec;

 else if(strcmp(firstWord,"jmp")==0||strcmp(secondWord,"jmp")==0)   func=jmp;

 else if(strcmp(firstWord,"bne")==0||strcmp(secondWord,"bne")==0)   func=bne;

 else if(strcmp(firstWord,"jsr")==0||strcmp(secondWord,"jsr")==0)   func=jsr;

 else if(strcmp(firstWord,"red")==0||strcmp(secondWord,"red")==0)   func=red;

 else if(strcmp(firstWord,"prn")==0||strcmp(secondWord,"prn")==0)   func=prn;

 else if(strcmp(secondWord,"rts")==0)   func=rts;/*we only check the second word because the first word was checked in the first comparison*/

 else if(strcmp(secondWord,"stop")==0)  func=stop;/*we only check the second word because the first word was checked in the first comparison*/

 else {/*if the line dosend contain any of the functions*/ 
   printf("%s %s \n","ilegal line", line);
   errorFound=1;
   return errorFound;
 }
 
 /*send the line to the right method with the righ tammount of parameters.*/
 if(func>=0&&func<=4) errorFound=twoParaFunc(line,isLabel,func);

 else if(func>=5&&func<=13)  errorFound=oneParaFunc(line,isLabel,func);

 else if(func>=14&&func<=15)  errorFound=noParaFunc(line,isLabel,func);


 return errorFound;
}




/*returns the number of extra words we need to add to the datalist and instruction list*/
int addNumOfWordsByAddressingType(int addressTypeOne){
  int ret;

 switch(addressTypeOne){
   case 0: {
             ret=1;
		break;}
   case 1: {
             ret=2;
		break;}
   case 2: {
             ret=2;
		break;}
   case 3: {
             ret=0;
		break;}
 }
 return ret;
}



/*returns 1 if the label is int the list and 0 if not */
int partOfList(char *label){
 int inList=0;

 iteratorLabel=headLabel;
 /*iterate label list to find the label name*/
 while(iteratorLabel!=NULL){
   if(strcmp(iteratorLabel->name,label)==0) inList=1;
   iteratorLabel=iteratorLabel->next;
 }
 return inList;
}



/*this method checks if a parameter has parantecies in it. if so it returns 1 else 0*/
int hasParantecies(char *para){
 int i=0;

 while (i<strlen(para)&&para[i]!='[') i++;
 
 if(i<strlen(para)) return 1;/*if has [ after label name*/

 else return 0;/*if dosent have [ after label name*/
}



/*checks if the parameter is a register. returns 1 if so . else returns 0*/
int isRegister(char *para){

 if(!strcmp(para,"r1")||!strcmp(para,"r2")||!strcmp(para,"r3")||!strcmp(para,"r4")||!strcmp(para,"r5")||!strcmp(para,"r6")||!strcmp(para,"r7")||!strcmp(para,"r8")||!strcmp(para,"r9")||!strcmp(para,"r10")||!strcmp(para,"r11")||!strcmp(para,"r12")||!strcmp(para,"r13")||!strcmp(para,"r14")||!strcmp(para,"r15"))  return 1;/*if the parameter is a register*/
 
 else return 0;
}



/*functions with 2 arguments. returns 1 if any errors were found. the function gets the fucntion number declared by user and adds the instructions to the list.*/
int twoParaFunc(char* line,int isLabel,enum Func func){
 char *firstPara;
 char *secondPara;
 int addressTypeOne;
 int addressTypeTwo;
 int numOfWords=2;/*we start with at least 2 words in a two parameter method*/
 int mask;
 char *firstWord;
 int wordCount=0;
 struct Instruction* newInstruction;/*this will be used to add new instructions*/

 /*get the parameters*/
 firstPara=getFirstPara(line,isLabel);
 secondPara=getSecondPara(line,isLabel);

 /*check if parameters are legal*/
 if(firstPara==NULL||secondPara==NULL) return 1;/*make sure that both parameters are legal and line is ok*/
 
 /*no errors were found and we have both parameter names. now we check the addressing type and create the label if needed*/

 if(isLabel==1){
           firstWord=getFirstWord(line);
           firstWord=getLabelName(firstWord);
           if(checkIfValidLabel(firstWord,1)==0){/*we send 1 because its a new label. this is how we chek the label name is legal*/ 
                /*create new label*/              
           	struct Label* newLabel = (struct Label*)malloc(sizeof(struct Label));
           	newLabel->name=firstWord;          	
           	newLabel->valDec=IC; 
           	newLabel->valBase=getBaseAddress(IC);
           	newLabel->valOffset=getOffsetAddress(IC);
                newLabel->type=code;

          	/*movig  th pointers of the linked list*/
          	if (prevLabel==NULL) {
           	 headLabel=newLabel;
          	}
          	else {
           	 prevLabel->next=newLabel;
          	}
		newLabel->next=NULL;
                prevLabel=newLabel;
	   }
           else{/*if its a non valid label*/
                printf("Ilegal label name: %s\n",firstWord);
 		return 1;
           }
    } 
    /*label was added if needed*/

 /*get addressing types for the parameters*/
 addressTypeOne=getAddressingType(firstPara);
 addressTypeTwo=getAddressingType(secondPara);

 /*check if the addressing type is valid acording to the chart*/
 if(addressTypeOne==4|| addressTypeTwo==4) return 1;

 if((func==0||func==3||func==2||func==4)&&addressTypeTwo==0) return 1;

 if(func==4&&(addressTypeOne==0||addressTypeOne==3)) return 1;


 /*now that we know addressing types we know how many words in memory we need to allocate.*/
 numOfWords+=addNumOfWordsByAddressingType(addressTypeOne);
 numOfWords+=addNumOfWordsByAddressingType(addressTypeTwo);
 
 /*we know how many words we need ,now we add the first 2 words to the instruction list*/


 /*create the firts word of the method with the opcode*/ 
 newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
 newInstruction->val=setOpCode(func);
 mask=A;
 newInstruction->val=newInstruction->val|mask;/*setting the ARE type*/
 newInstruction->valBase=getBaseAddress(IC);
 newInstruction->valOffset=getOffsetAddress(IC);
 wordCount++;/*keeping track of number of words added*/
 IC++;/*moving the instruction counter forward*/
    
 /*movig  th pointers of the linked list*/
 if (prevInstruction==NULL) {
   headInstruction=newInstruction;
 }
 else {
   prevInstruction->next=newInstruction;
 } 
 newInstruction->next=NULL;
 prevInstruction=newInstruction;

 /*create the second word of the method with function registers and addressing types*/
 newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
 newInstruction->val=setFuncWord(func,addressTypeOne,addressTypeTwo,firstPara,secondPara);
 newInstruction->valBase=getBaseAddress(IC);
 newInstruction->valOffset=getOffsetAddress(IC);

 wordCount++;/*keeping track of number of words added*/
 IC++;/*moving the instruction counter forward*/

 /*movig  the pointers of the linked list. it cant be the first word*/
 prevInstruction->next=newInstruction;
 newInstruction->next=NULL;
 prevInstruction=newInstruction;

 /*adding the extra words to the list if needed*/
 if(wordCount<numOfWords)   addWords(addressTypeOne,addressTypeTwo,firstPara,secondPara);
  
 return 0;
}



/*functions with 1 argument. returns 1 if any errors were found. the function gets the fucntion number declared by user and adds the instructions to the list.*/
int oneParaFunc(char* line,int isLabel,enum Func func){
 char *firstPara;
 int addressTypeOne;
 int numOfWords=2;/*we start with at least 2 words in a one parameter method*/
 int mask;
 char *firstWord;
 int wordCount=0;
 struct Instruction* newInstruction;
 
 /*get parameter name*/
 firstPara=getFirstPara(line,isLabel);

 /*check if the parameter is valid*/
 if (checkIfValidLine(line,isLabel)==1||firstPara==NULL) return 1;/*if the line isnt valid*/

 /*adding a label if needed*/
 if(isLabel==1){
           firstWord=getFirstWord(line);
           firstWord=getLabelName(firstWord);
           if(checkIfValidLabel(firstWord,1)==0){       /*we send 1 because its a new label declaratoin*/        
           	struct Label* newLabel = (struct Label*)malloc(sizeof(struct Label));
           	newLabel->name=firstWord;          	
           	newLabel->valDec=IC; 
           	newLabel->valBase=getBaseAddress(IC);
           	newLabel->valOffset=getOffsetAddress(IC);
                newLabel->type=code;
          	/*movig  th pointers of the linked list*/
          	if (prevLabel==NULL) {
           	 headLabel=newLabel;
          	}
          	else {
           	 prevLabel->next=newLabel;
          	}
		newLabel->next=NULL;
                prevLabel=newLabel;
	   }
           else{/*if the label isnt legal*/
                printf("Ilegal label name: %s\n",firstWord);
 		return 1;
           }
    }

 /*get addressing type*/
 addressTypeOne=getAddressingType(firstPara);

 /*check if the addressing type is valid acording to the chart*/
 if(addressTypeOne==4) return 1;

 if(((func>=5&&func<=8)||func==12)&&(addressTypeOne==0)) return 1;

 if((func>=9&&func<=11)&&(addressTypeOne==0||addressTypeOne==3)) return 1;


 /*now that we know addressing type we know how many words in memory we need to allocate.*/
 numOfWords+=addNumOfWordsByAddressingType(addressTypeOne);

/*we know how many words we need now we add the first 2 words to the instruction list*/


 /*create the firts word of the method with the opcode*/
 newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
 newInstruction->val=setOpCode(func);
 mask=A;
 newInstruction->val=newInstruction->val|mask;/*setting the ARE type*/
 newInstruction->valBase=getBaseAddress(IC);
 newInstruction->valOffset=getOffsetAddress(IC);
 wordCount++;/*keeping track of number of words added*/
 IC++;/*moving the instruction counter forward*/
    
 /*movig  th pointers of the linked list*/
 if (prevInstruction==NULL) {
   headInstruction=newInstruction;
 }
 else {
   prevInstruction->next=newInstruction;
 }
 newInstruction->next=NULL;
 prevInstruction=newInstruction;

 /*create the second word of the method with function registers and addressing types*/
 newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
 newInstruction->val=setFuncWord(func,0,addressTypeOne,"",firstPara);/*we send 0 as the first address type and an empty string so only the target register is added to the function word*/
 newInstruction->valBase=getBaseAddress(IC);
 newInstruction->valOffset=getOffsetAddress(IC);

 wordCount++;/*keeping track of number of words added*/
 IC++;/*moving the instruction counter forward*/

 /*movig  the pointers of the linked list*/
 prevInstruction->next=newInstruction;
 newInstruction->next=NULL;
 prevInstruction=newInstruction;


 /*adding the extra words to the list if needed*/
 if(wordCount<numOfWords)   addWords(addressTypeOne,3,firstPara,"");/*we send 3  to the second register so no words will be added for the second addressing type*/

 return 0;
}



/*functions with no arguments returns 1 if any errors were found. the function gets the fucntion number declared by user and adds the instructions to the list.*/
int noParaFunc(char* line,int isLabel,enum Func func){
 int mask;
 char *firstWord;
 struct Instruction* newInstruction;
 
 /*check if the rest of the line is valid*/
 if (checkIfValidLineNoPara(line,isLabel)==1) return 1;/*if the line isnt valid*/

 /*adding a label if needed*/
 if(isLabel==1){
           firstWord=getFirstWord(line);
           firstWord=getLabelName(firstWord);
           if(checkIfValidLabel(firstWord,1)==0){/*we send 1 because its a new label*/               
           	struct Label* newLabel = (struct Label*)malloc(sizeof(struct Label));
           	newLabel->name=firstWord;          	
           	newLabel->valDec=IC; 
           	newLabel->valBase=getBaseAddress(IC);
           	newLabel->valOffset=getOffsetAddress(IC);
                newLabel->type=code;
          	/*movig  th pointers of the linked list*/
          	if (prevLabel==NULL) {
           	 headLabel=newLabel;
          	}
          	else {
           	 prevLabel->next=newLabel;
          	}
		newLabel->next=NULL;
                prevLabel=newLabel;
	   }
           else{/*if the label name isnt valid*/
                printf("Ilegal label name: %s\n",firstWord);
 		return 1;
           }
    }



 /*create the firts word of the method with the opcode/ thers only one word*/
 newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
 newInstruction->val=setOpCode(func);
 mask=A;
 newInstruction->val=newInstruction->val|mask;/*setting the ARE type*/
 newInstruction->valBase=getBaseAddress(IC);
 newInstruction->valOffset=getOffsetAddress(IC);

 IC++;/*moving the instruction counter forward*/
    
 /*movig  th pointers of the linked list*/
 if (prevInstruction==NULL) {
    headInstruction=newInstruction;
 }
 else {
   prevInstruction->next=newInstruction;
 }
 newInstruction->next=NULL;
 prevInstruction=newInstruction;

 return 0;
}


/*sets the bit of the opcode word to the right value*/
int setOpCode(enum Func func){
 int ret;


 if(func==0||func==1||func==4||func==12||func==13||func==14||func==15) ret = 1<<(func);/*move the 1 bit to right place in ret*/

 else if(func>=2&&func<=3) ret=1<<2;/*move 1 bit to second place*/

 else if(func>=5&&func<=8) ret=1<<5;
 
 else if(func>=9&&func<=11) ret=1<<9;

 return ret;
}




/*the function checks if the extern statment is legal and if it is it creats a new label that the extern type */
int dealWithExtern(char *line,int isLabel){
 int errorFound=0;
 char *labelName=(char *)malloc(MAXLABEL * sizeof(char));
 int i=0;
 int charCount=0;

 /*get the label name*/
 if (isLabel)
 while(i<strlen(line)&&line[i]!=':') i++;/*reach end of label*/
 i++;/*get first char after label*/

 while(line[i]!='n') i++;/*reaching end of extern statement*/
 i++;/*we want to have the first white space*/

 while (line[i]==' '||line[i]=='\t') i++;/*skip white spaces*/

 while(line[i]!=' '&&line[i]!='\t'&&line[i]!='\n'){
	if(charCount<MAXLABEL){
		labelName[charCount]=line[i];
		i++;
		charCount++;
	}
        else errorFound=1;
 }
 labelName[charCount]='\0';

 /*check for extra text*/
 while (line[i]==' '||line[i]=='\t') i++;/*finish reading line to find errors*/
 
 if (charCount==0||line[i]!='\n') {
  errorFound=1;
  printf("Ilegal extern statment");
 }
 /*if the line is ok create a new label with extern type*/
 else{
	if(checkIfValidLabel(labelName,1)==0){   /*we send 1 because we want to create a new label*/         
           	struct Label* newLabel = (struct Label*)malloc(sizeof(struct Label));
           	newLabel->name=labelName;
           	newLabel->valDec=0; 
           	newLabel->valBase=0;
           	newLabel->valOffset=0;
                newLabel->entOrExt=ext;

          	/*movig  th pointers of the linked list*/
          	if (prevLabel==NULL) {
           	 headLabel=newLabel;
          	}
          	else {
           	 prevLabel->next=newLabel;
          	}
                newLabel->next=NULL;
                prevLabel=newLabel;
	 }
         else{
                printf("Ilegal label name: %s",labelName);
 		errorFound=1;;
         }
 }

 return errorFound;
}




/*inputs the data or string from user in to the data array*/
int dealWithData(char *line,int isLabel){/*adds all data in command to the data list. returns number of data pieces collected, 1 if error acord*/
 char *firstWord;
 char *secondWord;
 int i=0;
 char wordNum[6];/*the largest size of number we can have with a sign*/
 int num;
 int mask=0;
 int c=0;
 struct Data* newData;

 /*addint label if needed*/
 if(isLabel==1){
           firstWord=getFirstWord(line);
           firstWord=getLabelName(firstWord);
           if(checkIfValidLabel(firstWord,1)==0){               
           	struct Label* newLabel = (struct Label*)malloc(sizeof(struct Label));
           	newLabel->name=firstWord;          	
                newLabel->type=data;
                newLabel->oldAddress=DC;/*the address will not be this one but we need a way to find the label. so we give it a value form DC*/

          	/*movig  the pointers of the linked list*/
          	if (prevLabel==NULL) {
           	 headLabel=newLabel;
          	}
          	else {
           	 prevLabel->next=newLabel;
          	}
		newLabel->next=NULL;
                prevLabel=newLabel;
	   }
           else{
                printf("Ilegal label name: %s",firstWord);
 		return 1;
           }
    }

 /*get the first and second words*/
 firstWord=getFirstWord(line);
 secondWord=getSecondWord(line);

 /*deal with data type*/
 if(strcmp(secondWord,".data")==0||strcmp(firstWord,".data")==0){/*if we need to input numbers*/
   if(isLabel==1) {
    while(i<strlen(line)&&line[i]!=':') i++;
    i++;/*first char after the :*/
   }
   if (checkLegalData(line,isLabel)==1) {/*we make sue the line is valid before reading the data. if its not we print an error */
    printf("ilegal data\n"); 
    return 1;
   }
   while(i<strlen(line)&&line[i]!='t') i++;
   i+=2;/*reach first char after .data word*/

   /*read dat numbers untill you reach end of line*/ 
   while(i<strlen(line)&&line[i]!='\n'){
    while(i<strlen(line)&&line[i]!=','&&line[i]!='\n'){
      if(line[i]!=' '&&line[i]!='\t') {
       wordNum[c]=line[i];/*add only the number that arent white spaces*/
       c++;
      }
      i++;
    }

    num=atoi(wordNum);/*convert string to int*/
    if(line[i]==','){  
     if(num>MAXNUM||num<MINNUM) {/*number to big or small*/
      printf("error in num\n");
      return 1;/*if we have an ilegal number*/
    }

    if(num<0){/*convrt to twosComplement if needed*/
      num=twosComplement(num);
    }

    /*create a data node and add it to the list*/
    newData = (struct Data*)malloc(sizeof(struct Data));
    newData->val=num;
    mask=A;
    newData->val=newData->val|mask;/*setting the ARE type*/
    newData->oldAddress=DC;/*it has an old address so if a label is pointing to it, it will be easy to find*/

    DC++;/*moving the data counter forward*/
    
   /*movig  th pointers of the linked list*/
   if (prevData==NULL) {
    headData=newData;
   }
   else {
    prevData->next=newData;
   }
   newData->next=NULL;
   prevData=newData;

   /*clean number for next number to come in*/
   c=0;
   strcpy(wordNum,"");
   i++;
  }/*big if*/

 }/*end of big while*/

 num=atoi(wordNum);/*convert number*/
 if(num>MAXNUM||num<MINNUM) {
     printf("error in num\n");
     return 1;/*if we have an ilegal number*/
 }

 if(num<0){
     num=twosComplement(num);
 }

 /*create the last number */
 newData = (struct Data*)malloc(sizeof(struct Data));
 newData->val=num;
 mask=A;
 newData->val=newData->val|mask;/*setting the ARE type*/
 newData->oldAddress=DC;

 DC++;/*moving the instruction counter forward*/
    
 /*movig  th pointers of the linked list*/
 if (prevData==NULL) {
   headData=newData;
 }
 else {
   prevData->next=newData;
 }
 newData->next=NULL;
 prevData=newData;
}/*end of data numbers input*/


 else {/*if we need to input a string*/
  while(i<strlen(line)&&line[i]!='\"') i++;/*reach the first " in line*/

  if(i==strlen(line)){/*if the line dosent have "*/
    printf("ilegal string\n"); 
    return 1;
  }
  i++;/*get first char after first "*/

  /*input chars to data list*/
  while (i<strlen(line)&&line[i]!='\"'){/*untill we reach the end of the string*/
    newData = (struct Data*)malloc(sizeof(struct Data));
    newData->val=line[i];
    mask=A;
    newData->val=newData->val|mask;/*setting the ARE type*/
    newData->oldAddress=DC;

    DC++;/*moving the instruction counter forward*/
    
    /*movig  th pointers of the linked list*/
    if (prevData==NULL) {
      headData=newData;
    }
    else {
      prevData->next=newData;
    }
    newData->next=NULL;
    prevData=newData;

    i++;
   } /*end of while*/
  
    /*add the 0 char to the end of the string*/
    newData = (struct Data*)malloc(sizeof(struct Data));
    newData->val='\0';
    mask=A;
    newData->val=newData->val|mask;/*setting the ARE type*/
    newData->oldAddress=DC;
    DC++;/*moving the instruction counter forward*/
    
    /*movig  th pointers of the linked list*/
    if (prevData==NULL) {
      headData=newData;
    }
    else {
      prevData->next=newData;
    }
    newData->next=NULL;
    prevData=newData;   

    if(i==strlen(line)){/*if we were out of bounds before we reached second " */
      printf("ilegal string\n"); 
      return 1;
    }
 }/*end ofstring adding*/

 return 0;
}



/*adds the extra words to the instruction list*/
int addWords(int addressTypeOne,int addressTypeTwo,char *firstPara,char *secondPara){
  struct Instruction* newInstruction;
  int num;
  int mask;
 
 /*add number in line to instruction list*/
 if(addressTypeOne==0){
           newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
           num=getDirectNum(firstPara);
           if(num==50000){/*if the number was too small or too big*/
            return 1;
           }
           mask=A;
           num=num|mask;
           newInstruction->val=num;
           newInstruction->valBase=getBaseAddress(IC);
           newInstruction->valOffset=getOffsetAddress(IC);

           IC++;/*moving the instruction counter forward*/    

           /*movig the pointers of the linked list*/
           prevInstruction->next=newInstruction;
           newInstruction->next=NULL;
           prevInstruction=newInstruction;
      }
      else if(addressTypeOne==1||addressTypeOne==2){/*we cant yet know the address so we set the val of the words but we 
                                                      make new elemnts to hold the values when we have them*/
           /*adding the first word that will have the base address*/
           newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
           newInstruction->valBase=getBaseAddress(IC);/*we dont know what the value of the word will be but we know what address it will have in the memory*/
           newInstruction->valOffset=getOffsetAddress(IC);

           IC++;/*moving the instruction counter forward*/              

           /*movig  th pointers of the linked list*/
           prevInstruction->next=newInstruction;
           newInstruction->next=NULL;
           prevInstruction=newInstruction;

           /*adding the second word that will have the offset address*/
           newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
           newInstruction->valBase=getBaseAddress(IC);
           newInstruction->valOffset=getOffsetAddress(IC);

           IC++;/*moving the instruction counter forward*/    

           /*movig  th pointers of the linked list*/
           prevInstruction->next=newInstruction;
           newInstruction->next=NULL;
           prevInstruction=newInstruction;
      }
      
     /*add second type of addressing type*/
     if(addressTypeTwo==0){
           newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
           num=getDirectNum(secondPara);
           if(num==50000){/*if the number was too small or too big*/
            return 1;
           }
           newInstruction->val=num;
           newInstruction->valBase=getBaseAddress(IC);
           newInstruction->valOffset=getOffsetAddress(IC);

           IC++;/*moving the instruction counter forward*/       

           /*movig the pointers of the linked list*/
           prevInstruction->next=newInstruction;
           newInstruction->next=NULL;
           prevInstruction=newInstruction;
      }
      else if(addressTypeTwo==1||addressTypeTwo==2){/*we cant yet know the address so we set the val of the words but we make 
                                                     new elemnts to hold the values when we have them*/
           /*adding the first word that will have the base address*/
           newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
           newInstruction->valBase=getBaseAddress(IC);
           newInstruction->valOffset=getOffsetAddress(IC);

           IC++;/*moving the instruction counter forward*/    

           /*movig  th pointers of the linked list*/
           prevInstruction->next=newInstruction;
           newInstruction->next=NULL;
           prevInstruction=newInstruction;

           /*adding the second word that will have the offset address*/
           newInstruction = (struct Instruction*)malloc(sizeof(struct Instruction));
           newInstruction->valBase=getBaseAddress(IC);
           newInstruction->valOffset=getOffsetAddress(IC);

           IC++;/*moving the instruction counter forward*/    

           /*movig  the pointers of the linked list*/
           prevInstruction->next=newInstruction;
           newInstruction->next=NULL;
           prevInstruction=newInstruction;
      }
return 0;

}

