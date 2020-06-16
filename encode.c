#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
//int main()
//{
uint file_extn( const char *ptr, EncodeInfo *encInfo);
int  main(int argc,char **argv)
{

	struct _EncodeInfo *encInfo=malloc(sizeof(EncodeInfo));
	int return_value;
	return_value=check_operation_type(argv);
	if(return_value==0)
	{
		return_value= read_and_validate_encode_args(argv,encInfo);

		printf("fun1");
	}
	if(return_value==0)
	{
		return_value=get_file_size(encInfo->fptr_secret,encInfo);
		printf("fun2");


	}
	if(return_value==0)

	{
		return_value=check_capacity(encInfo);
			printf("fun3");
	}
	if(return_value==0)
	{
		copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image);
		printf("fun4");
	}

	/*	extn_secret_file =file_extn(argv[3], encInfo);
		FILE *fptr;


	// char *argv[3];
	fptr=fopen(argv[3],"w");
	file_size=get_file_size(fptr);
	FILE *fptr_src_image;*/
	/*opening image file */
	/*	encInfo->fptr_src_image=fopen(argv[2],"w");
		image_size=get_image_size_for_bmp(fptr_image);
		if(file_size > image_size)
		{
		printf("size of file exceeds size of image");
		}       */
	/*opening output file*/
	/*	FILE *fp_out;
		fp_out=fopen(argv[4],"w++");
		if(fp_out == NULL)
		{
		printf("outputfile is not created");
		exit(1);
		} */
	/*calling function for copying header file */
	/*	FILE *fptr_src_image;
		fptr_src_image=fopen(argv[2],"w");
		FILE *fptr_dest_image;
		fptr_dest_image=fopen(argv[4],"w");     */
	//	copy_bmp_header(fptr_src_image, fptr_dest_image);
	/*calling function for magic_string*/
	// encode_magic_string( magic_string, EncodeInfo *encInfo)
	/*calling function for checking capacity*/
	//	FILE *fp_magic;
	//	fp_magic=fopen(argv[3],"w");
	//	check capacity(&encInfo);
}


//}
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}
/*
 * 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}


OperationType check_operation_type(char *argv[])
{
	//	printf("%s\n",argv[3]);
	if (strcasecmp(argv[1],"-e"))
	{
		printf("operation is not encoding");
		return e_failure;

	}
	else
	{
		printf("e\n");
		/* Read and validate Encode args from argv */
		// read_and_validate_encode_args( argv, encInfo);
		return e_success;

	}
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if(argv[2] == NULL)
	{
		printf("source image file is missing\n");
		return e_failure;
	}
	else
	{

		/*allocate memory for source image*/
		encInfo->src_image_fname=malloc(strlen(argv[2]));

		if(argv[3] == NULL)
		{
			printf("secret file is missing\n");
			return e_failure;
		}
		else
		{

			/*allocating memory for secret file*/
			encInfo->secret_fname=malloc(strlen(argv[3]));

			if(argv[4] == NULL)
			{
				printf("output file is missing\n");
				return e_failure;
			}
			else
			{
				encInfo->stego_image_fname=malloc(strlen(argv[4]));
			}
		}

	}
	char ch;
	encInfo->fptr_secret=fopen(argv[3],"r");
	while((ch=fgetc(encInfo->fptr_secret)!=EOF))
	{
		ftell(encInfo->fptr_secret);
	}
	if(ftell(encInfo->fptr_secret)==0)
	{
		printf("secret file is empty\n");
		return e_failure;
	}
	else
	{
		/*secret file extension size*/
		encInfo->secret_fname=argv[3];
		printf("sec_name %s\n", encInfo->secret_fname);
		char var;	
		int i;
		int count = 0;

		int length_extn=strlen(encInfo->secret_fname);

		//printf("1st char %c\n", encInfo->secret_fname[0]);
		/*for( i = 0; ((var = encInfo->secret_fname[i]) != '.'); i++)
		  {
		  count++;
		  }*/
		while((*((encInfo->secret_fname)+(++count))) != '.');

		//printf("count %d\n", count);
		encInfo->str_len=length_extn  - count;

		printf("secret file extension =%d",encInfo->str_len);

		return e_success;


	}
}


/* Get file size */
uint get_file_size(FILE *fptr,EncodeInfo *encInfo)
{
	char ch;

	rewind(fptr);

	while((ch=fgetc(fptr)!=EOF))
	{
		encInfo->size_secret_file=ftell(fptr);
	}
	//	encInfo->size_secret_file = ftell(fptr);
	printf("secret file size=%ld",encInfo->size_secret_file);
	return e_success;
}
/*checking capacity of secret file */
Status check_capacity(EncodeInfo *encInfo)
{
	//	encInfo->magic_string='*';
	//	printf("%c",encInfo->magic_string);
	printf("enter the magic string * or #:");

	//	scanf("%c", &(encInfo->magic_string));	
	//	encInfo->total_encoding_data_size=(sizeof(encInfo->magic_string))*8+(sizeof(encInfo->str_len))*8+(sizeof(encInfo->size_secret_file))*8+((encInfo->str_len)*8)+((encInfo->size_secret_file)*8);

	printf("total capacity of secret file=%d",encInfo->total_encoding_data_size);
	return e_success;
}




/*	
//	const char *ptr=argv[3];
int count=0,extn_length;
encInfo->src_image_fname = malloc(strlen(argv[2]) * sizeof(char));
strcpy(encInfo->src_image_fname,argv[2]);
printf("image file name %s",encInfo->src_image_fname);
if(strstr(argv[2],".bmp"))
{
if(argv[3] == NULL)
{
printf("secret.txt file is missing");
}
else
{
//check whether the secret.txt file is empty or not
FILE *fp;
char ch;
fp=fopen(argv[3],"r");
while((ch=fgetc(fp))!=EOF)
{
ftell(fp);
}
if(ftell(fp) == 0)
{
printf("secret file is empty");
}
else

{
while((*ptr)!='.')
{

count++;
}
printf("count =%d",count);

extn_length=strlen(ptr)-count;
printf("extn_length=%d",extn_length);



}
}
}*/



Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	printf("hihadh");
	int i,c=0;
	char temp;
	rewind(fptr_src_image);
	rewind(fptr_stego_image);
	for(i=0;i<54;i++)
	{
		fread(&temp,sizeof(char),1,fptr_src_image);
		fwrite(&temp,1,sizeof(char),fptr_stego_image);

	}

}










//Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
//{
/*to get the 55th position*/
/*int f_first=0;
  fseek(fp,54L,SEEK_SET);
  char star='*';
  int int_star=star;
//	printf("%d\n",int_star);
unsigned int mask=1;
mask <<= 5;
printf("binary value\n");
while(mask)
{
if(int_star & mask);
{
printf("1");
}
else
{
printf("0");
}
mask >>= 1;
}*/














