/******************************************************************************/
/**
@file		convertdata.c
@author		Ramon Lawrence
@brief		Converts text data to block-based binary file.
@copyright	Copyright 2021
			The University of British Columbia,
			Ramon Lawrence
@par Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

@par 1.Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.

@par 2.Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

@par 3.Neither the name of the copyright holder nor the names of its contributors
	may be used to endorse or promote products derived from this software without
	specific prior written permission.

@par THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
/******************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * Univ. Washington environment data
 */
int
convert_test_data(
        FILE *infile,
        FILE *outfile)
{
    printf("Converting data.\n");

    int16_t pageSize = 512;
    int16_t headerSize = 16;
    int16_t countOffset = 4;
    int16_t recordSize = 16;
    int16_t recordsPerPage = 31;
    int16_t count = 0;
    int32_t total1 = 0, total2 = 0;
    int8_t* buffer = malloc((size_t) pageSize);
    char line[256];
    char record[16];
    time_t time, prevTime = 0;
    time_t sumDiffs = 0;

    while (fgets(line, sizeof(line), infile))
    {        
        if (line[0] == '#')
            continue;

        // printf("%s\n", line);
        struct tm cal = {};
        int jd1, jd2, dir;
        double pres, tair, tdew, spd;
        sscanf(line, "%d-%d-%d %d:%d %d.%d %lf %lf %lf %d %lf", &(cal.tm_year), &(cal.tm_mon), &(cal.tm_mday), &(cal.tm_hour), &(cal.tm_min), &jd1, &jd2, &pres, &tair, &tdew, &dir, &spd);     

        cal.tm_year -= 1900;
        cal.tm_mon--;
        cal.tm_sec = 0;
        cal.tm_isdst = -1;        
        //strptime(line, "%Y/%m/%d/%H", &cal);        
        time = mktime(&cal);
        // printf("UNIX time: %lu\n", time);
        if (time < prevTime)
        {
            printf("UNIX time: %lu  Diff: %lu\n", time, (time-prevTime));           
            printf("%s\n", line);
        }
        
        if (prevTime != 0)
        {           
            sumDiffs += time - prevTime;  
        }   
        else
        {
            printf("Time first record: %lu\n", time);  
        }
        prevTime = time;

        /* Convert record to binary form */
        memcpy(record, &time, 4);
        /* 2 byte key for temperature */
        /*
        int16_t itair = (int16_t) (tair*10);
        int16_t itdew = (int16_t) (tdew*10);
        memcpy( (void*) (record+4), &itair, 2);
        memcpy( (void*) (record+6), &itdew, 2);
        */
        /* 4 byte key for temperature */
        int32_t itair = (int32_t) (tair*10);
        // int16_t itdew = (int16_t) (tdew*10);
        memcpy( (void*) (record+4), &itair, 4);        

        int32_t ipres = (int32_t) (pres*10);
        int32_t ispd = (int32_t) (spd*10);
        
        memcpy( (void*) (record+8), &ipres, 4);
        memcpy( (void*) (record+12), &ispd, 4);
        
        /* Add record to block */        
        if (count == recordsPerPage)
        {
            /* Block full. Write out page */
            *((int16_t*) (buffer + countOffset)) = count;
            fwrite(buffer, pageSize, 1, outfile);
            count = 0;
        }    
        memcpy( (void*) (buffer+headerSize+count*recordSize), record, recordSize);
        count++;    
        total1++;    
    }

    /* Write out last page */
    if (count > 0)
    {
        /* Write out last page */
        *((int16_t*) (buffer + countOffset)) = count;
        fwrite(buffer, pageSize, 1, outfile);        
    }   
    fflush(outfile);
    
    printf("Time last record: %lu\n", time);  
      
    /* Verify the data written */
    fseek(outfile, 0, SEEK_SET);

    while (1)
    {
        /* Read page */
        if (0 == fread(buffer, pageSize, 1, outfile))
            break;
                  
        /* Process all records on page */
        int16_t count = *((int16_t*) (buffer+countOffset));
        /* printf("Block: %d Count: %d\n", *((int16_t*) buffer), count); */            
        for (int j=0; j < count; j++)
        {	
            void *buf = (buffer + headerSize + j*recordSize);				
            /* Print record */
            /* 2 byte key for temperature */
            /*
            printf("%lu %d %d %d %d\n", *((uint32_t*) buf), *((int16_t*) (buf+4)), *((int16_t*) (buf+6)), *((int32_t*) (buf+8)), *((int32_t*) (buf+12)));
            */
            /* 4 byte key for temperature */            
            // printf("%lu %d %d %d\n", *((uint32_t*) buf), *((int32_t*) (buf+4)), *((int32_t*) (buf+8)), *((int32_t*) (buf+12)));            
            total2++;
        }
    }                            
    
    printf("Total records read: %d  written: %d\n", total1, total2);
    printf("Total time diffs: %lu  Per record diffs: %lu\n", sumDiffs, sumDiffs/total1);

    free(buffer);    
    
    return 0;
}

/**
 * Main function to run tests
 */ 
int main()
{
	/* Open input text file*/
    FILE *infile;
    // infile = fopen("data/seatac_data_100K.txt", "r");
    infile = fopen("data/uwa_data_only_2000_500K.txt", "r");
    if (NULL == infile) 
    {
        printf("Error: Failed to open input file!\n");
        return -1;
    }

    /* Open output binary file */
    FILE *outfile;
    // outfile = fopen("data/sea100K.bin", "w+b");
    outfile = fopen("data/uwa500K.bin", "w+b");
    if (NULL == outfile) 
    {
        printf("Error: Failed to open output file!\n");
        return -1;
    }
    
    convert_test_data(infile, outfile);

    return 0;
}