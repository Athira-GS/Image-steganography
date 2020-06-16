#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<stdlib.h>
#include<string.h>
#include<math.h>
int  main(int argc,char **argv)
{

	struct _EncodeInfo *encInfo = malloc(sizeof(EncodeInfo));
	int return_value;
	encInfo->src_image_fname = argv[2];
	encInfo->secret_fname = argv[3];
	encInfo->stego_image_fname = argv[4];
	return_value = check_operation_type(argv);
	//printf("return_value of operation %d\n",return_value);
	if(return_value == 0)
	{
		return_value = read_and_validate_encode_args(argv, encInfo);
		//	printf("return_value of read & validate %d\n",return_value);

	}
	/*if return value is 1 call function for decoding*/
	else if (return_value==1)
	{
		decode(argv);
		return 0;
	}
	else
	{
		return 1;
	}


	if(return_value==0)
	{
		/*function call for opening files*/
		return_value = open_files(encInfo);
	}
	else
	{
		printf("error in read and validate function\n");
		return 1;
	}


	if(return_value==0)
	{
		/*function call for getting file size*/
		return_value=get_file_size(encInfo->fptr_secret,encInfo);


	}
	else
	{
		printf("error in opening files\n");
		return 1;
	}
	if(return_value==0)

	{
		/*function call for checking capacity*/
		return_value=check_capacity(encInfo);
	}
	else
	{
		printf("error in getting file size\n");
		return 1;
	}
	if(return_value==0)
	{
		/*function call for copying header file*/
		return_value=copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image);
	}
	else
	{
		printf("error in checking capacity\n");
		return 1;
	}
	if(return_value==0)
	{
		/*function call for encoding magic string*/

		return_value= encode_magic_string(encInfo);
	}
	else
	{
		printf("error in copying bmp header\n");
		return 1;
	}
	if(return_value==0)
	{
		/*function call for encoding file extension*/
		return_value=encode_secret_file_extn(encInfo);
	}
	else
	{
		printf("error in encoding magic string\n");
		return 1;
	}
	if(return_value==0)
	{
		/*function call for encoding secret file size*/
		return_value=encode_secret_file_size( encInfo);

	}
	else
	{
		printf("error in encoding secret file extension\n");
		return 1;
	}
	if(return_value==0)
	{
		/*function call for encoding secret file size*/
		return_value=encode_secret_file_data(encInfo);
	}
	else
	{
		printf("error in encoding secret file size\n");
		return 1;
	}
	if(return_value==0)
	{
		/*function call for copying remaining data*/
		return_value=copy_remaining_img_data(encInfo);
	}
	else
	{

		printf("error in encoding secret file data\n");
		return 1;
	}

	if(return_value==0)
	{
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		free(encInfo);


		printf("encoding done successfully\n");
		return 0;
	}
	else
	{
		printf("encoding failed");
		return 1;
	}




}

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
	printf("INFO : getting image size\n");
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_image, 18L, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
	printf("done\n");
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
	printf("INFO : assigning file pointers\n");
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	printf("src file name is = %s\n",encInfo->src_image_fname);
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
		printf("Error in src image opening\n");

		return e_failure;
	}

	// Secret file
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
		printf("Error in secret file opening\n");

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		printf("Error in stego image opening\n");
		return e_failure;
	}

	printf("done\n");
	// No failure return e_success
	return e_success;
}


OperationType check_operation_type(char *argv[])
{
	printf("INFO : checking operation type\n");
	/*checking whether the operation is encoding or decoding*/
	if(argv[1] == NULL)
	{
		printf("operation is not mentioned");
	}
	else
	{
		//	printf("%s\n",argv[3]);
		if (!(strcmp(argv[1],"-e")))
		{
			printf("operation is encoding\n");
			return e_encode;

		}
		else if(!(strcmp(argv[1],"-d")))
		{
			printf("operation is decoding\n");
			return e_decode;

		}
		else
		{
			printf("invalid operation\n");
			return e_unsupported;
		}
	}
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	printf("INFO : validating arguments\n");
	/* checking whether the source file is present or not*/
	if(argv[2] == NULL)
	{
		printf("source image file is missing\n");
		return e_failure;
	}
	else
	{
		/*checking source file extension*/
		if((strstr(argv[2],".bmp")))
		{
			printf("input file is .bmp\n");

		}
		else
		{
			printf("input file is not .bmp\n");
			return e_failure;
		}
	}

	/*allocate memory for source image*/
	//encInfo->src_image_fname = malloc(strlen(argv[2]));
	/*checking secret file is present or not */
	if(argv[3] == NULL)
	{
		printf("secret file is missing\n");
		return e_failure;
	}
	else
	{

		/*allocating memory for secret file*/
		//encInfo->secret_fname = malloc(strlen(argv[3]));
		/*checking output file is present or not*/
		if(argv[4] == NULL)
		{
			/*copying the output image to the pointer*/
			strcpy(encInfo->stego_image_fname,"stegged_image.bmp");

			return e_failure;
		}
	}



	printf("done\n");
	return e_success;
}


/* Get file size */
uint get_file_size(FILE *fptr,EncodeInfo *encInfo)
{
	printf("INFO : getting file size\n");
	char ch;
	/*make the pointer to the initial position*/

	rewind(fptr);
	/*getting each character till the end of file*/

	while((ch=fgetc(fptr)!=EOF))
	{
		/*ftell will give the position of offset that is the file size and it is storing in a pointer*/
		encInfo->size_secret_file=(ftell(fptr))-1;
	}
	//	encInfo->size_secret_file = ftell(fptr);
	printf("secret file size=%ld\n",encInfo->size_secret_file);
	printf("done\n");
	return e_success;
}
/*checking capacity of secret file */
Status check_capacity(EncodeInfo *encInfo)
{
	printf("INFO : checking image capacity with the encoding data size\n");
	char ch;
	//    encInfo->fptr_secret = fopen(argv[3], "r");
	while( (ch = fgetc(encInfo->fptr_secret)) != EOF )
	{
		ftell(encInfo->fptr_secret);
	}
	/*checking whether the secret file is empty or not*/
	if( ftell(encInfo->fptr_secret) == 0)
	{
		printf("secret file is empty\n");
		return e_failure;
	}
	else
	{
		/*secret file extension size*/
		char var;   
		int i=0;
		int count = 0;
		/*finding the whole length of secret file*/
		int scrt_len = strlen( encInfo -> secret_fname);
		/*extension length will be the difference of whole length and length upto '.'*/
		while( ( *( (encInfo->secret_fname) + ( ++count ))) != '.');

		encInfo->str_len = scrt_len - count;

		printf("secret file extension %d\n", encInfo->str_len);
		/*the file extension is stored in an array*/
		while( ( *( (encInfo->secret_fname) + ( count ))) != '\0')
		{
			encInfo->extn_secret_file[i++]= *( (encInfo->secret_fname) + ( count++ ));
		}


		/*storing magic string in a variable */
		encInfo->magic_string='*';
		//	printf("%c",encInfo->magic_string);
		printf("enter the magic string * or #:");

		scanf("%c", &(encInfo->magic_string));
		/*total size should be the sum of size of magic string,size of extension,size of each character in extension,size of secret file and size of secret file data*/
		encInfo->total_encoding_data_size=(sizeof(encInfo->magic_string))*8+(sizeof(encInfo->str_len))*8+(sizeof(encInfo->size_secret_file))*8+((encInfo->str_len)*8)+((encInfo->size_secret_file)*8);
		//		printf(" size of str_len=%ld\n",sizeof(encInfo->str_len));
		//		printf(" size of secretfile=%ld\n",sizeof(encInfo->size_secret_file));

		//		printf(" str_len=%d\n",(encInfo->str_len));
		//		printf(" secret=%ld\n",(encInfo->size_secret_file));

		printf("total capacity of encoding data=%d\n",encInfo->total_encoding_data_size);
	}
	printf("done\n");
	return e_success;
}
/* copying header file*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	printf("INFO : copying header file\n");
	int i,c=0;
	char temp;
	/*make the source pointer and output pointer to the initial position*/
	rewind(fptr_src_image);
	rewind(fptr_stego_image);
	/*read 54 bytes from the source file and copy it to the output file */
	for(i=0;i<54;i++)
	{
		fread(&temp,sizeof(char),1,fptr_src_image);
		fwrite(&temp,1,sizeof(char),fptr_stego_image);

	}
	printf("done\n");
	return e_success;

}

/*encoding magic string*/
Status encode_magic_string(EncodeInfo *encInfo)

{
	printf("INFO : encoding magic signature\n");

	char ch;
	int i;
	/*mask should be 0x80 since we want to get the msb of magic string and it is in 8 byte*/
	unsigned int mask=0x80;
	for(i=0;i<8;i++)
	{
		/*read each byte from the source file and store it in a variable*/ 
		fread(&ch,sizeof(char),1,encInfo->fptr_src_image);
		/*clear the last bit of byte of the source image*/
		ch=ch & 0xfe;
		/*to get the msb of magic string*/
		if(encInfo->magic_string & mask)
		{
			/*do OR operation if we get the result as 1*/
			ch=ch | 1;
		}
		/*storing the byte in the output image*/
		fwrite(&ch,1,sizeof(char),encInfo->fptr_stego_image);
		/*right shift the mask to get next bit of magic string*/
		mask = mask >> 1;



	}
	printf("done\n");
	return e_success;


}
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
	printf("INFO : encoding secret file extension length\n");
	char ch;
	char *file_extn;
	file_extn[encInfo->str_len];
	int i=0;
	/*file extension length is integer value so 32 bit we want*/
	uint mask = 0x80000000;
	for(i=0;i<32;i++)
	{
		/*reading each characters from source image and storing it in a variable.*/
		fread(&ch,sizeof(char),1,encInfo->fptr_src_image);
		/*	clearing lsb of a byte from the source image*/
		ch=ch & 0xfe;
		/*to get msb of secret file extension*/

		if(encInfo->str_len & mask)
		{

			/*do OR operation if we get the result as 1*/
			ch=ch | 1;
		}

		/*storing the byte in the output image*/
		fwrite(&ch,1,sizeof(char),encInfo->fptr_stego_image);
		/*right shift the mask to get the next bit of file extension*/
		mask = mask >> 1;



	}
	/*to print the string from '.'*/
	file_extn = strstr(encInfo->secret_fname,".");
	//printf("extension : %s",file_extn);
	printf("done\n");
	printf("encoding file extension \n");
	uint mask_1;
	int j;
	printf("\nFile Extension = %s\n", file_extn);
	/*iterate till the extension length is reached*/
	for(i = 0;i < (encInfo->str_len);i++)
	{
		mask_1 = 0x80;
		for ( j = 0; j < 8; j++)
		{
			/*read each byte from source image and store it in a variable*/
			fread(&ch,sizeof(char),1,encInfo->fptr_src_image);
			/*	clearing lsb of a byte from the source image*/
			ch=ch & 0xfe;
			//printf("File_extn[%d] = %c\n", i, file_extn[i]);
			/* getting msb of file extension*/
			if(file_extn[i] & mask_1)
			{
				/*do OR operation if we get the result as 1*/
				ch = ch | 1;
			}

			/*storing the byte in the output image*/
			fwrite(&ch, 1, sizeof(char), encInfo->fptr_stego_image);
			mask_1 = mask_1 >> 1;
		}




	}
	printf("done\n");
	return e_success;
}
Status encode_secret_file_size(EncodeInfo *encInfo)
{
	printf("INFO : encoding file size\n");
	//	printf("%ld\n",encInfo->size_secret_file);
	char ch; 
	int i;
	unsigned int mask=0x80000000;
	for(i=0;i<32;i++)
	{
		/*read each byte from source image and store it into a variable*/	
		fread(&ch,sizeof(char),1,encInfo->fptr_src_image);
		/*	clearing lsb of a byte from the source image*/
		ch=ch & 0xfe;
		/*getting the msb of secret file size*/

		if(encInfo->size_secret_file & mask)
		{
			/*do OR operation if we get the result as 1*/
			ch=ch | 1;
		}

		/*storing the byte in the output image*/
		fwrite(&ch,1,sizeof(char),encInfo->fptr_stego_image);
		mask = mask >> 1;
	}
	printf("done\n");
	return e_success;
}
/*encoding secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	printf("INFO : encoding secret file data\n");
	/*make the pointer to the initial position*/
	rewind(encInfo->fptr_secret);
	char ch_secret,ch_src;
	int i;
	uint mask=0x80;
	int count=0;
	do
	{
		/*getting each byte of the secret data*/	
		ch_secret = fgetc(encInfo->fptr_secret);
		//printf("%c\n",ch_secret);




		for(i=0;i<8;i++)
		{
			fread(&ch_src,sizeof(char),1,encInfo->fptr_src_image);
			/*	clearing lsb of a byte from the source image*/
			ch_src=ch_src & 0xfe;

			if(ch_secret & mask)
			{
				/*do OR operation if we get the result as 1*/
				//	printf("1");
				ch_src=ch_src | 1;
			}

			/*storing the byte in the output image*/
			fwrite(&ch_src,1,sizeof(char),encInfo->fptr_stego_image);
			mask = mask >> 1;

		}
		mask=0x80;
		count++;

		//	printf("\n");


	}while(count < (encInfo->size_secret_file));





	printf("done\n");
	return e_success;

}

/*encoding remaining data*/
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
	printf("INFO : copying remaining image data\n");
	char ch;
	/*getting each byte from the source file till eof is reached*/
	while(!feof(encInfo->fptr_src_image))
	{
		ch=fgetc(encInfo->fptr_src_image);
		/*storing the byte in the output image*/
		fwrite(&ch,1,sizeof(char),encInfo->fptr_stego_image);
	}
	printf("done\n");
	return e_success;
}

//Decoding


Status decode(char **argv)
{
	printf("INFO : Decoding process started\n");
	/*allocate a memory for DcodeInfo*/
	struct _DecodeInfo *decInfo = malloc(sizeof(DecodeInfo));
	int return_value;
	decInfo->stego_image_fname = argv[2];

	return_value = read_and_validate_decode_args(argv, decInfo);
	if(return_value == 0)
	{
		/*function call for opening files*/
		return_value=open_dec_files(decInfo);

	}
	if(return_value ==0)
	{
		/*function call for decoding magic string*/
		return_value=decode_magic_string(decInfo);
	}
	if(return_value==0)
	{
		/*function call for decoding secret file extension*/
		return_value= decode_secret_file_extn(decInfo);
	}
	if(return_value == 0)
	{
		/*function call for decoding secret file size*/
		return_value=decode_secret_file_size(decInfo);

	}
	if(return_value==0)
	{
		/*function call for decoding secret file data*/
		return_value=decode_secret_file_data(decInfo);

	}

	if(return_value==0)
	{
		fclose(decInfo->fptr_stego_image);
		fclose(decInfo->fptr_decoded_output);
//		free(decInfo->output_file_name);
		free(decInfo->output_array);


		printf("decoding done successfully\n");
	}

}





/*function defenitions for decoding*/

Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo)
{
	printf("INFO : validating arguments\n");
	/*checking whether the input file is provided or not*/
	if(argv[2] == NULL)
	{
		printf("stegged image is missing\n");
	}
	else
	{
		/*checking the source file extension*/
		if((strstr(argv[2],".bmp")))

		{
			printf("stegged image is .bmp\n");



		}
		else
		{
			return e_failure;

		}
	}
	if(argv[3] == NULL)
	{
		/*allocate a memory for output file name*/
		decInfo->output_file_name=malloc(14);
		strcpy(decInfo->output_file_name,"decoded_output");

		//decInfo->output_file_name="decoded_output" ;
		printf("%s\n",decInfo->output_file_name);

	}
	else
	{
		decInfo->output_file_name=argv[3];
	}


	printf("done\n");
	return e_success;

}
/*opening files*/
Status open_dec_files(DecodeInfo *decInfo)
{
	printf("INFO : assigning file pointers\n");
	// Stego Image file
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
	printf("%s\n",decInfo->stego_image_fname);
	printf("done\n");
	return e_success;

}
/*decoding magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
	printf("INFO : decoding magic signature\n");
	char ch;
	int i;
	int sum=0;
	/*make the pointer to the initial position*/
	rewind(decInfo->fptr_stego_image);
	/*make the pointer to the 55th position*/
	fseek(decInfo->fptr_stego_image,54L,SEEK_SET);
	for(i=7;i>=0;i--)
	{
		/*reading each character from the stego image and store it in a variable*/
		fread(&ch,sizeof(char),1,decInfo->fptr_stego_image);
		ch=ch & 1;
		if ( ch ==1 )
		{

			/*converting the binary value to desimal */

			sum=sum+(ch * (pow(2,i)));
		}
	}
	printf("sum = %c\n",sum);

	printf("done\n");
	return e_success;
}
/*decoding secret file extension*/
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	printf("INFO : decoding secret file extension length\n");
	char ch;
	int sum=0,i;
	for(i=31;i>=0;i--)
	{

		/*reading each character from the stego image and store it in a variable*/
		fread(&ch,sizeof(char),1,decInfo->fptr_stego_image);
		ch = ch & 1;
		//	printf("%d ",ch);
		if ( ch == 1 )
		{
			sum = sum + ( ch * (pow(2, i)));
		}
	}
	printf("Extension length = %d\n", sum);
	decInfo->decode_extn_len = sum;


	printf("done\n");
	printf("INFO : decoding secret file extension\n");
	int j;
	//To get file extension
	for(j = 1; j <= (decInfo->decode_extn_len); j++)
	{
		char sum = 0;
		for(i = 7; i >= 0; i--)
		{   
			/*reading each character from the stego image and store it in a variable*/
			fread(&ch, sizeof(char), 1, decInfo->fptr_stego_image);
			//ch = ch & 0x01;
			if ( ch & 0x01)
			{

				sum = sum + (pow(2,i));
			}
		}   
		//printf("%c\n",sum);
		/*storing the sum into an array*/
		*((decInfo->file_extn)+(j-1))=sum;
	}


	printf("done\n");
	return e_success;

}
/*decoding secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	printf("INFO : decoding secret file size\n");
	char ch;
	int sum=0,i;
	for(i=31;i>=0;i--)
	{

		/*reading each character from the stego image and store it in a variable*/
		fread(&ch,sizeof(char),1,decInfo->fptr_stego_image);
		ch = ch & 1;
		//  printf("%d ",ch);
		if ( ch == 1 )
		{
			sum = sum + ( ch * (pow(2, i)));
		}
	}
	printf("size of data= %d\n", sum);
	/*storing the secret file size into a pointer*/
	decInfo->decode_data_size=sum;
	
	printf("done\n");
	return e_success;
}



/*decoding secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	printf("INFO : decoding secret file data\n");
	char ch;
	int i,j;
	/*allocate memory for output array*/
	decInfo->output_array=malloc(decInfo->decode_data_size-1);

	printf("%d\n",decInfo->decode_data_size);
	for(j = 1; j <=(decInfo->decode_data_size); j++)
	{

		char sum = 0;
		for(i = 7; i >= 0; i--)
		{
			/*reading each character from the stego image and store it in a variable*/
			fread(&ch, sizeof(char), 1, decInfo->fptr_stego_image);
			//ch = ch & 0x01;
			if ( ch & 0x01)
			{

				sum = sum + (pow(2,i));

			}


		}
		*((decInfo->output_array)+(j-1))=sum;

	}
	printf("done\n");

	printf("INFO : copying secret file data to the output file \n");
	/*create an array with length equal sum of  string length of output file name and extension length*/ 
	char output_file_name_with_ext[strlen(decInfo->output_file_name)+(decInfo->decode_extn_len)];
	
	/*copy the output file name to that array*/
	strcpy(output_file_name_with_ext,(decInfo->output_file_name));
	
	/*concatenate output file name and extension*/
	strcat(output_file_name_with_ext,decInfo->file_extn);
	
	printf("%s\n",output_file_name_with_ext);

	/*open the output file and write into it*/
	decInfo->fptr_decoded_output=fopen(output_file_name_with_ext,"w");
	
	for(i=1;i<(decInfo->decode_data_size);i++)
	{
		/* store the contents of secret file  into the output file pointer*/
		fwrite(((decInfo->output_array)+(i-1)),1,sizeof(char),decInfo->fptr_decoded_output);
	}
	printf("done\n");
	return e_success;

}























