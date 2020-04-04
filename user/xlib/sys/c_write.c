/*
 * File Name: _write.c
 *
 *
 * BSD 3-Clause License
 * 
 * Copyright (c) 2019, nelsoncole
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#include <sys/sys.h>

#define EOF (-1)


size_t c_write (const void *buffer,size_t size, size_t count, FILE *fp)
{
	if(!fp) return (0);

	size_t i;
	int c;
	size_t rc = 0;

	unsigned char *cache = (unsigned char*) (fp->header.buffer);
	unsigned char *buf = (unsigned char*)buffer;
	unsigned int offset = 0;



	if ((fp->header.mode[0] == 's') && \
	(fp->header.mode[1] == 't') && (fp->header.mode[2] == 'd')) 
	{ 

		for(i=0;i < size*count;i++)
		{
			c = c_putc (*buf++, fp);
			if(c == EOF) return (rc/size);
			rc++;
		}

		return (rc/size);

	}

	if ((fp->header.mode[0] != 'w') && \
	(fp->header.mode[0] != 'a') && (fp->header.mode[1] != '+')) return (0);


	for(i=0;i < size*count;i++)
	{

		c = *buf++;
		if(fp->header.offset > fp->header.size) return (rc/size);

		offset = fp->header.offset%0x10000;
		if( ((offset == 0) && (fp->header.offset != 0)) || (!(fp->header.flag&0x10)) ) {

			c = c_putc (c,fp);
			//feof(fp);
			if(c == EOF) return (rc/size);

		} else {
			
			if (!(fp->header.flag&0x80)) 
			{
				// FIXME AddFAT(), 
				// isto devemos tratar na altura do flush(), 
				// para melhor compatibilidade.
				if(fp->header.size >= (fp->header.blocks*fp->header.bps*fp->header.count)) 
				{	// new block // FAT
					if(AddFAT(fp,(FAT_BPB *)fp->header.bpb,fp->block[fp->header.blocks-1])) {
						return (rc/size);
					}
				}	
			}

			*(unsigned char*)(cache + offset) = c;
			// Update offset
			fp->header.offset += 1;
			if(fp->header.offset > fp->header.size )fp->header.size += 1;
		}
		
		rc++;
	}

	return (rc/size);
}