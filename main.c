#include <stdio.h>
#include <stdlib.h>

int main(int argc, char**argv){
    if (argc != 3){
        printf("Usage: \n\t%s <filename> <variable-name>\n",argv[0]);
        return 1;
    }
    
    unsigned char *buffer;
    unsigned long fileLen;
    FILE *file = fopen(argv[1], "rb");
    if (!file){
        fprintf(stderr, "Unable to open file %s", argv[1]);
        return 1;
    }
    
    //Get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

    //Allocate memory
    buffer = (unsigned char*)malloc(fileLen);
    if (!buffer){
        fprintf(stderr, "Memory error!");
        fclose(file);
        return 1;
    }

    //Read file contents into buffer
    fread(buffer, fileLen, 1, file);
    fclose(file);

    printf ("#include<stdlib.h>\n\n");
    printf ("#include<stdio.h>\n\n");
    printf ("static int EMBED_DATA_%s_FREED = 0;\n",argv[2]);
    printf ("static unsigned char * EMBED_DATA_%s;\n\n",argv[2]);

    printf ("void EMBED_DATA_%s_CLOSE(){\n",argv[2]);
    printf ("    if (!EMBED_DATA_%s_FREED){\n",argv[2]);
    printf ("        free(EMBED_DATA_%s);\n",argv[2]);
    printf ("        EMBED_DATA_%s_FREED= 1;\n",argv[2]);
    printf ("    }\n");
    printf ("}\n");
    
    printf ("unsigned long EMBED_DATA_%s_LENGTH(){\n",argv[2]);
    printf ("    return %lu;\n",fileLen);
    printf ("}\n");

    
    printf ("int EMBED_DATA_%s_INIT(){\n",argv[2]);
    printf ("    EMBED_DATA_%s = malloc(%lu);\n",argv[2],fileLen);
    printf ("    if (!EMBED_DATA_%s){\n",argv[2]);
    printf ("        return 0;\n");
    printf ("    }\n");
    
    printf ("    atexit(EMBED_DATA_%s_CLOSE);\n",argv[2]);
    
    char hexCode[] = "0123456789ABCDEF";
    unsigned long i=0;
    for (; i<fileLen; i++){
        printf ("    EMBED_DATA_%s[%lu] = 0x%c%c;\n",argv[2],i,
                    hexCode[buffer[i]/16],hexCode[buffer[i]%16]);
    }
    printf ("    return 1;\n");
    printf ("}\n");
    
    /*printf ("int main(){\n");
    printf ("    EMBED_DATA_%s_INIT();\n",argv[2]);
    printf ("    FILE* fp = fopen(\"test\",\"wb\");\n");
    printf ("    fwrite(EMBED_DATA_%s,EMBED_DATA_%s_LENGTH(),1,fp);\n",
                    argv[2],argv[2]);
    printf ("    fclose(fp);\n");
    printf ("    EMBED_DATA_%s_CLOSE();\n",argv[2]);
    printf ("    return 0;\n");
    printf ("}\n");*/

    free(buffer);
    return 0;
}
