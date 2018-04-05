/* On my honor, I have neither given nor received unauthorized aid on this assignment
- Shrivinayak Bhat
- ECE,University of Florida
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct node{
	char line[33];
	int count,taken;
	struct node* next;
};

char num_to_3bin[8][4] = {"000","001","010","011","100","101","110","111"};
char num_to_5bin[32][6] = {
"00000",
"00001",
"00010",
"00011",
"00100",
"00101",
"00110",
"00111",
"01000",
"01001",
"01010",
"01011",
"01100",
"01101",
"01110",
"01111",
"10000",
"10001",
"10010",
"10011",
"10100",
"10101",
"10110",
"10111",
"11000",
"11001",
"11010",
"11011",
"11100",
"11101",
"11110",
"11111"
};


enum Func{COMP,DECOMP};

void errork(char* err){
	printf("Error: %s\n", err);
	exit(1);
}


int hamming(char* a, char* b, int *away){
	*away = 0;
	int len1 = strlen(a), len2 = strlen(b),count=0,first;

	if(len1!=len2)
		return -1;
	for(int i=0;i<len1;i++){

		if(a[i]!=b[i]){
			if(count==0){
				count++;
				first = i;
			}
			else {
				count++;

				if(i-first==1)
					*away=0; //consecutive
				else if(abs(first-i)<=4)
					*away=2; //not consecutive but within diff=4
				else
					*away=3; // not consecutive, not within 4
			}

		}
	}
	return count;
}

void compress(){

	FILE *fin,*fout;
	
	fin = fopen("original.txt","r");
	if(fin==NULL)
		errork("Unable to open file original.txt");
	
	fout = fopen("cout.txt","w");
	if(fout==NULL)
		errork("Unable to open file cout.txt");


	char code[33*80];
	code[0] = '\0';
	char buff[33];
	int found=0,inp_count=0;
	struct node *head=NULL,*iter=NULL,*tmp=NULL,*unique=NULL,*iter2=NULL;

	while (1){
		if(!fgets(buff,33,fin))
			break;
		if(strcmp(buff,"\n")==0)
			continue;
		strtok(buff, "\n");
		tmp = (struct node *) malloc(sizeof(struct node));

		strcpy(tmp->line,buff);
		tmp->next = NULL;

		if(head==NULL){
			head = tmp;
			iter = tmp;
		}
		else{
			iter->next = tmp;
			iter = iter->next;
		}

		inp_count++;
	}


	fclose(fin);
	// fclose(fout);


	// printf("inp_count = %d\n", inp_count);
	iter = head;

	while(iter){

		if(unique==NULL){
			tmp = (struct node *) malloc(sizeof(struct node));
			strcpy(tmp->line,iter->line);
			tmp->next = NULL;
			tmp->count = 1;
			tmp->taken = 0;

			unique = tmp;
			iter2=unique;
		}
		else{
			tmp = unique;
			found = 0;
			while(tmp){
				if(strcmp(iter->line,tmp->line)==0){
					// printf("found match for %s", iter->line);
					tmp->count+=1;
					found = 1;
					break;
				}
			tmp = tmp->next;

			}

			if(found==0){
				tmp = (struct node *) malloc(sizeof(struct node));
				strcpy(tmp->line,iter->line);
				tmp->next = NULL;
				tmp->count = 1;
				tmp->taken = 0;

				iter2->next = tmp;
				iter2=iter2->next;
			
			}


		}
			iter = iter->next;


	}

	iter2 = unique;

	// while(iter2){
	// 	printf("code = %s, count=%d\n",iter2->line,iter2->count);
	// 	iter2 = iter2->next;
	// }

	//generating dictionary
	char dict[8][33];
	int dict_count=0;
	for (int i = 0; i < 8; i++){
		iter2 = unique;
		tmp = NULL;

		while(iter2){
			if(iter2->taken){
				iter2=iter2->next;
				continue;
			}
			if(tmp==NULL && iter2->taken!=1)
				tmp = iter2;
			else if(tmp->count < iter2->count)
					tmp = iter2;

			iter2 = iter2->next;
		}

		if(tmp==NULL)
			break;
		strcpy(dict[dict_count++],tmp->line);
		tmp->taken=1;
		
	}
	

	// start encoding
	inp_count = 0;
	int ham;
	char* temp = malloc(sizeof(char)*33);
	int flag=0,rle_count=0,away=0,diff;
	char prev[33];
	//buff[33]

	iter = head;
	while(iter){
		// printf("instruction: %s\n", iter->line);
		// RLE, Bitmask, dict_1, dict_2, direct

		// RLE
		if(iter!=head && strcmp(iter->line,prev)==0 &&rle_count!=5){
			strcpy(temp,"000");
			rle_count+=1;
			switch(rle_count){
				case 1:
					strcat(temp,"00");
					break;
				case 2:
					strcat(temp,"01");
					break;
				case 3:
					strcat(temp,"10");
					break;
				case 4:
					strcat(temp,"11");
					break;

			}
			flag = 1;
		}

		else{
			if(flag){
				strcat(code,temp);
				flag=0;
			}
			rle_count=0;

			ham = hamming(dict[0],iter->line,&diff);
			inp_count=0;
			away=diff;
			for(int i=1;i<dict_count;i++){
				diff=0;
				if(hamming(dict[i],iter->line,&diff)<ham){
					ham = hamming(dict[i],iter->line,&diff);
					away = diff;
					inp_count=i;
				}
			}

			// printf("ham = %d\naway=%d\n",ham,away);
			if(ham==0){
				//direct matching
				strcpy(temp,"101");
				temp = strcat(temp,num_to_3bin[inp_count]);

			}

			else if(ham == 1){
				//one bit mismatch
				int i;
				for(i=0;i<strlen(iter->line);i++){
					if(dict[inp_count][i]!=iter->line[i])
						break;
				}

				strcpy(temp,"010");
				strcat(temp,num_to_5bin[i]);
				strcat(temp,num_to_3bin[inp_count]);
			}
			else if(ham == 2 && away == 0){
				// two bit consecutive mismatch
				int i;
				for(i=0;i<strlen(iter->line);i++){
					if(dict[inp_count][i]!=iter->line[i])
						break;
				}

				strcpy(temp,"011");
				strcat(temp,num_to_5bin[i]);
				strcat(temp,num_to_3bin[inp_count]);	
			}
			else if(ham <= 4 && away==2){
				// bitmask
				int i;
				for(i=0;i<strlen(iter->line)-3;i++){
					if(dict[inp_count][i]!=iter->line[i])
						break;
				}

				strcpy(temp,"001");
				strcat(temp,num_to_5bin[i]);

				for(int j=0;j<4;j++)
					temp[8+j] = dict[inp_count][i+j]== iter->line[i+j] ? '0':'1';

				temp[12]='\0';
				strcat(temp,num_to_3bin[inp_count]);	
			}
			else if(ham == 2 && away==3){
				//2 bit mismatch anywhere
				int m1=-1,m2=-1;
				for(int i=0;i<strlen(iter->line);i++){
					if(dict[inp_count][i]!=iter->line[i]){
						if(m1==-1)
							m1=i;
						else{
							m2=i;
							break;
						}
					}
				}

				strcpy(temp,"100");
				strcat(temp,num_to_5bin[m1]);
				strcat(temp,num_to_5bin[m2]);
				strcat(temp,num_to_3bin[inp_count]);
			}
			else{
				strcpy(temp,"110");
				strcat(temp,iter->line);
			}

		}
		if(!flag)
			strcat(code,temp);
		 // printf("%s",temp );
		// printf("%s --> %s\n",iter->line,temp );
		strcpy(prev,iter->line);

		iter = iter->next;

	}

	int index=0;
	while(code[index]!='\0'){
		fwrite(&code[index++],1,1,fout);	
		if(index%32==0)
			fwrite("\n",1,1,fout);		
	}
	for(int i=0;i<32-(index%32);i++){
		fwrite("1",1,1,fout);	
	}
	fwrite("\n",1,1,fout);
	fwrite("xxxx\n",1,5,fout);

	for(int i=0;i<dict_count;i++){
		fwrite( &dict[i],1,32,fout);
		fwrite("\n",1,1,fout);
	}


	fclose(fout);

	// for(i =0;i<strlen(code);i+=32){
	// 	printf("%*.*s\n", 32, 32, &code[i]);
	// 	// printf("%s",code);
	// }
	// // printf("%s",&code[i-32]);
	// printf("xxxx\n");
	// for(int i=0;i<dict_count;i++)
		// printf("%s\n",dict[i]);

}

char char_neg(char c){
	if(c=='0')
		return '1';
	else
		return '0';
}
void decompress(){

	FILE *fin,*fout;
	fin = fopen("compressed.txt","r");
	if(fin==NULL)
		errork("Unable to open file compressed.txt");
	fout = fopen("dout.txt","w");
	if(fout==NULL)
		errork("Unable to open file dout.txt");

	char buff[33],dict[8][33],codestring[1000],*ptr,tbuff[32];
	codestring[0]='\0';
	int flag=0,dict_count=0,temp,temp1,temp2;
	
	while (1){
		if(!fgets(buff,33,fin))
			break;
		if(strlen(buff)==2)
			continue;
		if(strncmp(buff,"xxxx",4)==0){
			flag=1;
		}
		else if(flag){
			strcpy(dict[dict_count],buff);
			dict_count++;
		}
		else
			strcat(codestring,buff);
	}
	fclose(fin);
	// fprintf(fout,"codestring: %s\ndict=",codestring);
	// for(int i=0;i<dict_count;i++)
	// 	fprintf(fout,"%s\n",dict[i]);
	ptr = codestring;
	while(ptr!='\0'){

		if(strncmp(ptr,"000",3)==0){
			// fprintf(fout,"rle\n");
			strncpy(tbuff,ptr+3,2);
			tbuff[2]='\0';
			temp = (int)strtol(tbuff,NULL, 2);

			for(int i=0;i<=temp;i++)		
				fprintf(fout,"%s\n",buff);

			ptr+=5;
		}
		else if(strncmp(ptr,"001",3)==0){
			// fprintf(fout,"bitmask\n");
			strncpy(tbuff,ptr+3,5);
			tbuff[5]='\0';
			temp = (int)strtol(tbuff,NULL, 2);
			strncpy(tbuff,ptr+12,3);
			tbuff[3]='\0';
			temp1 = (int)strtol(tbuff,NULL, 2);

			strcpy(buff,dict[temp1]);
			for(int i=0;i<4;i++){
				if(ptr[8+i]=='1')
					buff[temp+i] = char_neg(buff[temp+i] );
			}
			fprintf(fout,"%s\n",buff);

			ptr+=15;

		}
		else if(strncmp(ptr,"010",3)==0){
			// fprintf(fout,"1 mismatch\n");

			strncpy(tbuff,ptr+3,5);
			tbuff[5]='\0';
			temp = (int)strtol(tbuff,NULL, 2);

			strncpy(tbuff,ptr+8,3);
			tbuff[3]='\0';
			temp1 = (int)strtol(tbuff,NULL, 2);
			strcpy(buff,dict[temp1]);
			buff[temp] = char_neg(buff[temp]);
			fprintf(fout,"%s\n",buff);

			ptr+=11;
		}
		else if(strncmp(ptr,"011",3)==0){
			// fprintf(fout,"2 consecutive mismatch\n");
			strncpy(tbuff,ptr+3,5);
			tbuff[5]='\0';
			temp = (int)strtol(tbuff,NULL, 2);
			strncpy(tbuff,ptr+8,3);
			tbuff[3]='\0';
			// fprintf(fout,"tbuff=%s\n", tbuff);
			temp1 = (int)strtol(tbuff,NULL, 2);

			// fprintf(fout,"temp = %d, temp1=%d%s\n",temp,temp1 );
			strcpy(buff,dict[temp1]);
			// fprintf(fout,"loaded from dict=%s\n",buff);
			buff[temp] = char_neg(buff[temp]);
			buff[temp+1] = char_neg(buff[temp+1]);
			fprintf(fout,"%s\n",buff);
			ptr+=11;

		}
		else if(strncmp(ptr,"100",3)==0){
			// fprintf(fout,"2 bit non consective\n");
			strncpy(tbuff,ptr+3,5);
			tbuff[5]='\0';
			temp = (int)strtol(tbuff,NULL, 2);
			strncpy(tbuff,ptr+8,5);
			tbuff[5]='\0';
			temp1 = (int)strtol(tbuff,NULL, 2);
			strncpy(tbuff,ptr+13,3);
			tbuff[3]='\0';
			temp2 = (int)strtol(tbuff,NULL, 2);

			strcpy(buff,dict[temp2]);
			buff[temp] = char_neg(buff[temp]);
			buff[temp1] = char_neg(buff[temp1]);
			fprintf(fout,"%s\n",buff);

			ptr+=16;
		}
		else if(strncmp(ptr,"101",3)==0){
			// fprintf(fout,"direct match\n");
			
			strncpy(tbuff,ptr+3,3);
			tbuff[3]='\0';
			temp = (int)strtol(tbuff,NULL, 2);

			strncpy(buff,dict[temp],32);
			buff[32]='\0';
			// printf("temp=%d buff=%s\n",temp,buff);
			fprintf(fout,"%s\n",buff);

			ptr+=6;
		}
		else if(strncmp(ptr,"110",3)==0){
			// fprintf(fout,"uncompressed\n");
			strncpy(buff,&ptr[3],32);
			buff[32]='\0';
			fprintf(fout,"%s\n",buff);
			ptr+=35;
		}
			else{
			// fprintf(fout,"unknown\n");
			break;
		}
	}

}

int main(int argc, char** arg){
	if(arg[1][0]=='2'){
		decompress();
	}
	else if(arg[1][0]=='1'){
		compress();
	}
	else
		printf("Invalid selection!\n");


	return 0;
} 


