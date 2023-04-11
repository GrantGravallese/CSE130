/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/
/*
https://www.ibm.com/docs/en/i/7.3?topic=apis-large-file-support
https://www.ibm.com/docs/en/i/7.2?topic=ssw_ibm_i_72/apis/open64.htm
https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-lseek-change-offset-file
https://www.linuxquestions.org/questions/programming-9/why-do-i-get-implicit-declaration-of-function-%27open%27-error-on-linux-7541699/
https://www.gnu.org/software/libc/manual/html_node/Opening-and-Closing-Files.html
https://stackoverflow.com/questions/16516476/c-fopen-vs-open
https://stackoverflow.com/questions/90261696/get-large-file-size-in-c
https://man7.org/linux/man-pages/man2/lseek.2.html
https://stackoverflow.com/questions/124169/how-do-you-get-a-directory-listing-in-c
https://linux.die.net/man/3/open
https://www.mkssoftware.com/docs/man3/open.3.asp
https://www.gnu.org/software/libc/manual/html_node/Permission-Bits.html
https://ss64.com/bash/chmod.html
https://stackoverflow.com/questions/249291645/why-do-i-get-implicit-declaration-of-function-open64-openat-openat64-warni
https://stackoverflow.com/questions/20111017/open-system-call-working-differently
https://stackoverflow.com/questions/5245306/wrapper-for-open-and-open64-and-see-that-system-calls-by-vi-uses-open64
https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
https://stackoverflow.com/questions/197160919/read-write-from-file-descriptor-at-offset
https://www.microchip.com/forums/m566054.aspx
https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
https://stackoverflow.com/questions/1164024216/how-to-properly-use-scandir-in-c/21617054
https://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html
https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
https://man7.org/linux/man-pages/man0/dirent.h.0p.html
https://stackoverflow.com/questions/49832785/why-is-lseekfd1-0-seek-end-equal-file-size-1
https://pubs.opengroup.org/onlinepubs/009696799/functions/lseek.html
https://support.sas.com/documentation/onlinedoc/ccompiler/doc700/html/lr1/z2031150.htm
https://stackoverflow.com/questions/56503813/is-read-write-small-size-of-data-performance-problematic
https://www.gnu.org/software/libc/manual/html_node/File-Position-Primitive.html
https://www.geeksforgeeks.org/c-program-copy-contents-one-file-another-file/
https://stackoverflow.com/questions/48388212/what-is-the-written-out-word-for-o-excl
https://www.gnu.org/software/libc/manual/html_node/Open_002dtime-Flags.html
https://stackoverflow.com/questions/1658476/c-fopen-vs-open
https://stackoverflow.com/questions/26582920/mmap-memcpy-to-copy-file-from-a-to-b
https://www.linuxquestions.org/questions/programming-9/i-have-a-problem-using-lseek-on-my-file-862968/
https://man7.org/linux/man-pages/man3/scandir.3.html

*/


#include <stddef.h>
#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> 
#include <dirent.h> 
#include <features.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdbool.h>

/*
 * Extended ASCII box drawing characters:
 * 
 * The following code:
 * 
 * printf("CSE130\n");
 * printf("%s%s Assignments\n", TEE, HOR);
 * printf("%s  %s%s Assignment 1\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 2\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 3\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 4\n", VER, TEE, HOR);
 * printf("%s  %s%s Assignment 5\n", VER, TEE, HOR);
 * printf("%s%s Labs\n", ELB, HOR);
 * printf("   %s%s Lab 1\n", TEE, HOR);
 * printf("   %s%s Lab 2\n", TEE, HOR);
 * printf("   %s%s Lab 3\n", ELB, HOR);
 * printf();
 * 
 * Shows this tree:
 * 
 * CSE130
 * ├─ Assignments
 * │  ├─ Assignment 1
 * │  ├─ Assignment 2
 * │  ├─ Assignment 3
 * │  ├─ Assignment 4
 * │  ├─ Assignment 5
 * └─ Labs
 *    ├─ Lab 1
 *    ├─ Lab 2
 *    └─ Lab 3
 */
#define TEE "\u251C"  // ├ 
#define HOR "\u2500"  // ─ 
#define VER "\u2502"  // │
#define ELB "\u2514"  // └
#define _LARGEFILE64_SOURCE
#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#define _FILE_OFFSET_BITS = 64
/*
 * Read at most SIZE bytes from FNAME starting at FOFFSET into BUF starting 
 * at BOFFSET.
 *
 * RETURN number of bytes read from FNAME into BUF, -1 on error.
 */
int fileman_read(
    const char *fname, 
    const size_t foffset, 
    const char *buf, 
    const size_t boffset, 
    const size_t size) 
{
  //open the file using open64 in order to handle large file sizes???, as in the message shown by the grader
  //unsure if this is neccessary, will ask in OH

  //The grading system uses significantly larger files than the teaching
  //servers. If your code does not handle large files well, you may fail
  //some tests. You will also fail tests if you fail to close any of the
  //files you opened.

  //call using RDOLY and the S flags for reading, check if open worked, return -1 if it didn't
  const int myFile = open( fname, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH );
  if( myFile == -1 )
    return -1;

  //seek through the file, return -1 if unable to seek
  //taken from https://www.linuxquestions.org/questions/programming-9/i-have-a-problem-using-lseek-on-my-file-862968/
  //and https://stackoverflow.com/questions/19780919/read-write-from-file-descriptor-at-offset
  if( lseek( myFile, foffset, SEEK_CUR ) == -1 ){
    close( myFile );
    return -1;
  }
  
  //add boffset to the buffer to "tape record" through the file
  buf = buf + boffset;

  //read the file according to the buffer + boffset value and the size, return -1 if it fails
  const int readSize = read( myFile, ( void** )buf, size );
  if( readSize == -1 ){
    close( myFile );
    return -1;
  }

  //close the file
  close( myFile );

  //return the readSize
  return readSize;
}

/*
 * Create FNAME and write SIZE bytes from BUF starting at BOFFSET into FNAME
 * starting at FOFFSET.
 * 
 * RETURN number of bytes from BUF written to FNAME, -1 on error or if FILE 
 * already exists
 */
int fileman_write(
    const char *fname, 
    const size_t foffset, 
    const char *buf, 
    const size_t boffset, 
    const size_t size) 
{

  //call O_CREAT and O_EXCL to check if file exists, if so then throw an error. Also use the S flags for reading and writing
  const int myFile = open( fname, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR | S_IRWXG | S_IRWXO );
  //check if open worked, return -1 if it didn't
  if( myFile == -1 )
    return -1;

  //seek through the file, return -1 if unable to seek, taken from https://www.linuxquestions.org/questions/programming-9/i-have-a-problem-using-lseek-on-my-file-862968/
  //and https://stackoverflow.com/questions/19780919/read-write-from-file-descriptor-at-offset
  if( lseek( myFile, foffset, SEEK_CUR ) == -1 ){
    close( myFile );
    return -1;
  }
  
  //add boffset to the buffer to "tape record" through the file
  buf = buf + boffset;

  //write to the file according to the buffer + boffset value and the size, return -1 if it fails
  const int writeSize = write( myFile, ( void** )buf, size );
  if( writeSize == -1 ){
    close( myFile );
    return -1;
  }
  
  //close the file
  close ( myFile );

  //return the writeSize
  return writeSize;

}

/*
 * Append SIZE bytes from BUF to existing FNAME.
 * 
 * RETURN number of bytes from BUF appended to FNAME, -1 on error or if FNAME
 * does not exist
 */
int fileman_append(const char *fname, const char *buf, const size_t size) 
{
  //open the file using the same methodology found for fileman_read, but allow to write as well
  const int myFile = open( fname, O_RDWR | O_APPEND, S_IRWXU | S_IXUSR | S_IRWXO );
  //do a check to see if we should return -1
  if( myFile == -1 )
    return -1;

  //lseek, taken from https://support.sas.com/documentation/onlinedoc/ccompiler/doc700/html/lr1/z2031150.htm
  //and https://stackoverflow.com/questions/19780919/read-write-from-file-descriptor-at-offset
  if( lseek( myFile, 0L, SEEK_END ) == -1 ){
    close( myFile );
    return -1;
  }

  //set appendSize, as done in fileman_write, this time without incrementing the buffer
  const int appendSize = write( myFile, ( void** )buf, size );
  if( appendSize == -1 ){
    close( myFile );
    return -1;
  }

  //close the file
  close( myFile );

  //return the append size
  return appendSize;
}

/*
 * Copy existing file FSRC to new file FDEST.
 *
 * Do not assume anything about the size of FSRC. 
 * 
 * RETURN number of bytes from FSRC written to FDEST, -1 on error, or if FSRC 
 * does not exists, or if SDEST already exists.
 */
//https://stackoverflow.com/questions/20111017/open-system-call-working-differently
//was hugely helpful 
int fileman_copy(const char *fsrc, const char *fdest) 
{
  //open fileRead and fileWrite using RDWR, as well as O_CREAT for fileWrite, check if the open failed
  const int fileRead = open( fsrc, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH );
  if( fileRead == -1 )
    return -1;

  //call O_CREAT and O_EXCL to check if file exists, if so then throw an error
  //also use RDWR, obviously
  const int fileWrite = open( fdest, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IXUSR | S_IRWXO );
  if( fileWrite == -1 ){
    close( fileRead );
    return -1;
  }
  //calloc for the buffer, according to BUFSIZ
  //BUFSIZ taken from https://stackoverflow.com/questions/56503813/is-read-write-small-size-of-data-performance-problematic
  char** buffer = calloc( 1, BUFSIZ );

  //method for copying file over using writeSize, readSize, and totalSize discussed in terms of pseudocode with Lauren Chambers, cruzid laalcham
  //preset the read size using read, call with the fsrc, buffer, and the buffer size
  int writeSize = 0;
  int readSize = read( fileRead, buffer, BUFSIZ );
  int totalSize = 0;

  //while readsize > 0, write to the fdest using the current readSize, read from the fsrc, add to total size
  //looks kinda weird, but makes sense because on the last pass through, readSize will be zero so it will do nothing
  while( readSize > 0 ){
    writeSize = write( fileWrite, buffer, readSize );
    readSize = read( fileRead, buffer, BUFSIZ );
    totalSize = totalSize + writeSize;
  }

  //close the files
  close( fileRead );
  close( fileWrite );

  //free the buffer
  free( buffer );

  //return the total size
  return totalSize;
    

}

/*
 * Print a hierachival directory view starting at DNAME to file descriptor FD 
 * as shown in an example below where DNAME == 'data.dir'
 *
 *   data.dir
 *       blbcbuvjjko
 *           lgvoz
 *               jfwbv
 *                   jqlbbb
 *                   yfgwpvax
 *           tcx
 *               jbjfwbv
 *                   demvlgq
 *                   us
 *               zss
 *                   jfwbv
 *                       ahfamnz
 *       vkhqmgwsgd
 *           agmugje
 *               surxeb
 *                   dyjxfseur
 *                   wy
 *           tcx
 */

//fd is the file descriptor for the file you want to write to 
//code taken from manpage for scandir, heavily based off of code example
//worked closely with tanuj to get the pseudocode for this function 
//cleaned up comments at prompting of Dr. Harrison, code should be readable


static void writeMeDir( const int fd, const char* dname, int level ){
  //write level times the white space, then the dname to the file
  for( int j = 0; j < level; j++ ){
    write( fd, "    ", sizeof("    ") -1 );
  }
  write( fd, dname, strlen( dname ));
  write( fd, "\n", sizeof( "\n" ) - 1 );

}
static void fancyDir( const int fd, const char *dname, int level, bool *flags, const char *buff ){


  //write to the file
  writeMeDir( fd,dname,level );

  //declare dirent struct
  struct dirent **namelist;
  int n;
  
  //call scandir as in scandir() manpage
  n = scandir( buff, &namelist, NULL, alphasort );

  //start at i=2, in order to avoid '.' and '..' files
  for( int i = 2; i < n; i++ ){

    //calloc in order to not have junk memory, size of char, buffer bufsize size to be safe
    char *buffer = ( char * ) calloc(BUFSIZ, BUFSIZ*sizeof( char ));
    strcpy( buffer, buff );
    strcat( buffer, "/" );
    strcat( buffer, namelist[i]->d_name );

    //make thing to free in order to not have memory problems
    void* thingToFree;
    if((( thingToFree = opendir( buffer )) != NULL )){
      //recurse with the new file path
      free( thingToFree );
      fancyDir( fd, namelist[i]->d_name, level + 1, NULL, buffer );
    }
    else{
        //free the thing and write
        free( thingToFree );
        writeMeDir( fd, namelist[i]->d_name, level + 1 );
    }
    //free the buffer
    free( buffer );

  }
  //free the elements of namelist in order to not have memory problems
  for( int i = 0; i < n; i++ ){
    free( namelist[i] );
  }

  //free the namelist
  free( namelist );

}
  
void fileman_dir( const int fd, const char *dname ) { 
  fancyDir( fd, dname, 0, NULL, dname );
}


/*
 * Print a hierachival directory tree view starting at DNAME to file descriptor 
 * FD as shown in an example below where DNAME == 'data.dir'.
 * 
 * Use the extended ASCII box drawing characters TEE, HOR, VER, and ELB.
 *
 *   data.dir
 *   ├── blbcbuvjjko
 *   │   ├── lgvoz
 *   │   │   └── jfwbv
 *   │   │       ├── jqlbbb
 *   │   │       └── yfgwpvax
 *   │   └── tcx
 *   │       ├── jbjfwbv
 *   │       │   ├── demvlgq
 *   │       │   └── us
 *   │       └── zss
 *   │           └── jfwbv
 *   │               └── ahfamnz
 *   └── vkhqmgwsgd
 *       ├── agmugje
 *       │   └── surxeb
 *       │       ├── dyjxfseur
 *       │       └── wy
 *       └── tcx
 */
//use an array of flags to pass down, 
//every time the function calls itself again, add another flag to this array
//pass down the depth, continually increase the level and amount of flags
//check the amount of flags against the amount of files still to print in the list
//given by prof. Harrison in Office Hours & Secret Sauce

//write( fd, "    ", sizeof("    ") -1 );
//write( fd, "├── ", sizeof("├── ") -1 );
//write( fd, "└── ", sizeof("└── ") -1 );
//write( fd, "│   ", sizeof("│   ") -1 );

//add flags
static void writeMeTree( const int fd, const char* dname, int level, int loopNum, int nNum, bool *flags){
  //write level times the white space, then the dname to the file
  for( int j = 0; j < level; j++ ){
    if( j == level - 1 ){
      // here is where to write the └── and the ├──  chars
      if( flags[j] == false ){ //add an && flag == something
        write( fd, "├── ", sizeof("├── ") -1 );
      }
      else{
        write( fd, "└── ", sizeof("└── ") -1 );
      }
    }
    else{
      //here is where to write the whitespace or the | chars
      if( flags[j] == false ){
        write( fd, "│   ", sizeof("│   ") -1 );
      }
      else{
        write( fd, "    ", sizeof("    ") -1 );
      }
    }
  }
  write( fd, dname, strlen( dname ));
  write( fd, "\n", sizeof( "\n" ) - 1 );
}
static void fancyTree( const int fd, const char *dname, int level, bool *flags, const char* buff ){
  
  //write to the file
  writeMeTree( fd, dname, level, 0, 0, flags );

  //declare dirent struct
  struct dirent **namelist;
  int n;
  
  //call scandir as in scandir() manpage
  n = scandir( buff, &namelist, NULL, alphasort );

  //start at i=2, in order to avoid '.' and '..' files

  for( int i = 2; i < n; i++ ){

    //encode the flag list
    flags[level] = ( i == n-1 );

    //calloc in order to not have junk memory, size of char, buffer bufsize size to be safe
    char *buffer = ( char * ) calloc( BUFSIZ, BUFSIZ*sizeof( char ));
    strcpy( buffer, buff );
    strcat( buffer, "/" );
    strcat( buffer, namelist[i]->d_name );

    //make thing to free in order to not have memory problems
    void* thingToFree;
    if((( thingToFree = opendir( buffer )) != NULL )){
      //recurse with the new file path
      free( thingToFree );
      fancyTree( fd, namelist[i]->d_name, level + 1, flags, buffer );
    }
    else{
        //free the thing and write
        free( thingToFree );
        writeMeTree( fd, namelist[i]->d_name, level + 1, i, n, flags);
    }
    //free the buffer
    free( buffer );

  }
  //free the elements of namelist in order to not have memory problems
  for( int i = 0; i < n; i++ ){
    free( namelist[i] );
  }

  //free the namelist
  free( namelist );

}

void fileman_tree( const int fd, const char *dname){
  bool *myFlags = ( bool * ) calloc(10, sizeof( bool ) );
  fancyTree( fd, dname, 0, myFlags, dname );
  free( myFlags );
}
