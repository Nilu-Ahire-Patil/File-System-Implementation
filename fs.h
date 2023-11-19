#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>

#define MAXFILENAME 80
#define MAXEXTENSIONNAME 20
#define MAXFILESIZE 1 //MB
#define MAXINODE 10 //maxFile
#define MAXBLOCKSIZE 512 //Bytes
#define TOTALCAPACITY MAXFILESIZE*1024*1024 //totalSorageCapacity
#define MAXBLOCK (TOTALCAPACITY/MAXBLOCKSIZE)  //65536

typedef struct superblock
{
    int num_inodes;
    int num_blocks;
    int block_size;
    int total_capacity;
    int remain_capacity;
}superblock;

typedef struct inode
{
    int size;
    int first_block;
    int is_empty;
    char name[MAXFILENAME];
    //char ext[MAXEXTENSIONNAME];
}inode;

typedef struct disk_block
{
    int next_block_num;
    int is_empty;
    char data[MAXBLOCKSIZE];
}disk_block;



void create_fs(); //initilise new file system
void mount_fs(char* file_name); //load a file syatem
void sync_fs(char* file_name); //write the file system

//rteurn alocate file number
int allocate_file(char name[8]);
void set_filesize(int filenum ,int size);
void write_byte(int filenum ,int pos, char data);
void put_file(char* file_name);
void print_fs(); //print out info about file system
void ls();


superblock sb;
inode *inodes;
disk_block *dbs;


int find_empty_inode()
{
    int i = 0;
    while(i < sb.num_inodes)
    {
        if(inodes[i].is_empty)
        {
            inodes[i].is_empty = 0;
            return i;
        }
        i++;
    }
    return -1;
}

int find_empty_block()
{
    int i = 0;
    while(i < sb.num_blocks)
    {
        if(dbs[i].is_empty)
        {
            dbs[i].is_empty = 0;
            return i;
        }
            
        i++;
    }
    return -1;
}

int get_block_num(int filenum,int togo)
{
    int blocknumber = inodes[filenum].first_block;
    while(togo>0)
    {
        int next_block = dbs[blocknumber].next_block_num;
        if(next_block == -1)
        {
            next_block = find_empty_block();
            dbs[blocknumber].next_block_num = next_block;
            blocknumber = next_block;
        }
        togo--;
    }
    return blocknumber;
}

//initilise new file system
void create_fs()
{
    sb.num_inodes = MAXINODE;
    sb.num_blocks = MAXBLOCK;
    sb.block_size = MAXBLOCKSIZE;
    sb.total_capacity = TOTALCAPACITY;//sb.num_blocks* sb.block_size;
    sb.remain_capacity = TOTALCAPACITY;
    int i = 0;
    inodes =(inode*) malloc(sizeof(inode) * sb.num_inodes);
    while (i< sb.num_inodes)//init nodes
    {
        inodes[i].size = -1;
        inodes[i].is_empty = 1;
        inodes[i].first_block = -1;
        //strcpy(inodes[i].ext,"NULL");
        strcpy(inodes[i].name,"NULL");
        i++;
    }
    
    dbs =(disk_block*) malloc(sizeof(disk_block) * sb.num_blocks);
    i = 0;
    while (i< sb.num_blocks)//init nodes
    {
        dbs[i].is_empty = 1;
        dbs[i].next_block_num = -1;
        i++;
    }
}

//load a file syatem
void mount_fs(char* file_name)
{
    FILE *file;
    file = fopen(file_name,"r");
    fread(&sb,sizeof(superblock), 1,file);
    
    inodes = (inode*) malloc(sizeof(inode) * sb.num_inodes);
    dbs = (disk_block*) malloc(sizeof(disk_block) * sb.num_blocks);
    
    fread(inodes,sizeof(inode),sb.num_inodes, file);
    fread(dbs, sizeof(disk_block), sb.num_blocks, file);

    fclose(file);
}

//write the file system
void sync_fs(char* file_name)
{
    FILE *file;
    file = fopen(file_name,"w+");

    fwrite(&sb,sizeof(superblock), 1,file);
    // fread(inodes,sizeof(inode),sb.num_inodes, file);
    // fread(dbs, sizeof(disk_block), sb.num_blocks, file);
    int i = 0;
    while (i< sb.num_inodes) //write inode
    {
       fwrite(&inodes[i],sizeof(inode), 1, file);
        i++;
    }
    
    i = 0;
    while (i< sb.num_blocks) //write dbs
    {
        fwrite(&dbs[i], sizeof(disk_block), 1, file);
        i++;
    }
    fclose(file);
    printf("Data File Created / Updated  Successfully\n");
}


void put_file(char* file_name)
{
    FILE* f;
    int count = 0;
    char c;
    assert(file_name != NULL);
    int fn = allocate_file(file_name);
    assert(fn != -1);
    if ((f = fopen(file_name, "r")) != NULL)
    {   int i = 0;
        while ((c = fgetc(f)) != EOF)
        {
            write_byte(fn,i++,c);
            count++;
        }
        inodes[fn].size = count;
        sb.remain_capacity -= count;
        fclose(f);
        printf("\nfile %s with size %d  Added Successsfuly\n",file_name,count);
    }
}

void ls()
{
    int i = 0;
    printf("\t\t Size\tName\n");
    while(!inodes[i].is_empty)
    {
        printf("\t\t%d\t%s\n",inodes[i].size, inodes[i].name);
        i++;
    }
}

//print out info about file system
void print_fs()
{
    printf("\n\tSuperBlock info\n");
    printf("\t\tnum inodes  %d\n",sb.num_inodes);
    printf("\t\tnum blocks  %d\n",sb.num_blocks);
    printf("\t\tblock size  %d\n",sb.block_size);
    printf("\t\ttotal space %d\n",sb.total_capacity);
    printf("\t\tempty space %d\n",sb.remain_capacity);
    

    printf("\n\tinodes\n");
    int i = 0;
    printf("\t\tSize\tfirst block\tName\n");
    while(i < sb.num_inodes)
    {
        printf("\t\t %d\t%d\t\t%s\n",inodes[i].size,inodes[i].first_block, inodes[i].name);
        i++;
    }
    printf("\n\tBlocks\n");
    i = 0;
    printf("\t\tblock\tnext block\n");
    while(i < 20)//sb.num_blocks)
    {
        printf("\t\t%d\t%d\n",i,dbs[i].next_block_num);
        i++;
    }
}


int allocate_file(char name[MAXFILENAME])
{
    int in = find_empty_inode();
    assert(in != -1);
    int block = find_empty_block();
    assert(block != -1);
    inodes[in].first_block = block;
    dbs[block].next_block_num = -1;
    strcpy(inodes[in].name,name);
    return in;
}

void set_filesize(int filenum ,int size)
{
    int total_require_block = size/MAXBLOCKSIZE;
    int blocknumber = inodes[filenum].first_block;
    while (total_require_block > 1)   
    {
        int next_num = dbs[blocknumber].next_block_num;
        if(next_num == -1)
        {
            int empty_block = find_empty_block();
            dbs[blocknumber].next_block_num = empty_block;
            dbs[empty_block].next_block_num = -1;            
        }
        blocknumber = dbs[blocknumber].next_block_num;
        total_require_block--;
    }
    dbs[blocknumber].next_block_num = -1;
}

void write_byte(int filenum ,int pos, char data)//write byte
{
    int relative_block = pos / MAXBLOCKSIZE;
    int bn = get_block_num(filenum, relative_block);
    //calculatet offset in the block
    int offset = pos % MAXBLOCKSIZE;
    //write the data
    dbs[bn].data[offset] = (data);
}

void help()
{
    printf("Commands:");
    printf("\n\tfile ls\n\t\tList Files");
    printf("\n\tfile ls [DataFileName]\n\t\tList Files fom DataFile");
    printf("\n\tfile help\n\t\tHelp ");
    printf("\n\tfile [filename]\n\t\tUpload File Temperory");
    //printf("\n\tfile load [DataFileName]\n\t\tLoads DataFile");
    printf("\n\tfile create [DataFileName]\n\t\tCreate New DataFile");
    printf("\n\tfile add [filename] in [DataFileName]\n\t\tInsert File in Exexting Data file");
}


int file(int argc, char* argv[])
{
    create_fs();    
    switch (argc)
    {
    case 1: help();
        break;
    case 2: if(!strcmp(argv[1],"ls"))
                ls();
            if(!strcmp(argv[1],"help"))
                help();
            else
                put_file(argv[1]);
        break;
    case 3: if(!strcmp(argv[1],"create"))
                sync_fs(argv[2]);
            if(!strcmp(argv[1],"ls"))
            {
                mount_fs(argv[2]);
                ls();
            }
        break;
    case 5: if(!strcmp(argv[1],"add") && !strcmp(argv[3],"in"))
        {
            mount_fs(argv[4]);
            put_file(argv[2]);
            sync_fs(argv[4]);
        }
        break;
    default:printf("invalid Command\n");
        help();
        break;
    }
    printf("\n");
    // print_fs();
    return 0;
}



