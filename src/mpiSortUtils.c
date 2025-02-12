/*
   This file is part of mpiSORT
   
   Copyright Institut Curie 2020
   
   This software is a computer program whose purpose is to sort SAM file.
   
   You can use, modify and/ or redistribute the software under the terms of license (see the LICENSE file for more details).
   
   The software is distributed in the hope that it will be useful, but "AS IS" WITHOUT ANY WARRANTY OF ANY KIND. Users are therefore encouraged to test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data. 
   
   The fact that you are presently reading this means that you have had knowledge of the license and that you accept its terms.
*/

/*
   Module:
     mpiSortUtils.c

   Authors:
    Frederic Jarlier, 	Institut Curie
	Nicolas Joly, 		Institut Pasteur
	Nicolas Fedy,		Institut Curie
	Leonor Sirotti,	 	Institut Curie
	Thomas Magalhaes,	Institut Curie
	Paul Paganiban,		Institut Curie
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
 * function used in mpiSort.c
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

void get_coordinates_and_offset_source_and_size_and_free_reads(int rank, int *local_read_rank, size_t *coordinates,
		size_t* offset, int* size, Read* data_chr, int local_readNum){

   	size_t j;
   	Read* chr = data_chr;
   	Read* to_free = chr;

   	//we initialize offset source and size_source
   	for(j = 0; j < local_readNum; j++){
   		coordinates[j] = 0;
   		size[j] = 0;
   		offset[j] = 0;
   		local_read_rank[j]=rank;
   	}

   	//first we are going to read in the source file
   	for(j = 0; j < local_readNum; j++){
   		//offset is the read size
   		coordinates[j] = chr->coord;
   		offset[j] = chr->offset_source_file;
   		size[j] = (int)chr->offset; //read length
   		to_free = chr;
		chr = chr->next;

		if (to_free) free(to_free);

   	}

}


size_t init_coordinates_and_size(int rank, int *local_reads_rank, size_t *local_reads_index,
		size_t* coordinates, int* size, Read* data_chr, int local_readNum)
{
	size_t dataSize = 0;
	size_t j;
	Read* chr = data_chr;

	//we initialize offset source and size_source
	for(j = 0; j < local_readNum; j++){
		size[j] = 0;
		coordinates[j] = 0;
	}

	//first we are going to read in the source file
	for(j = 0; j < local_readNum; j++){
		//offset is the read size
		coordinates[j] = chr->coord;
		size[j] = (int)chr->offset; //read length
		local_reads_rank[j] = rank;
		local_reads_index[j] = j;
		dataSize += chr->offset;

		chr = chr->next;
	}

	return dataSize;
}

size_t init_coordinates_and_size2(int rank, int *local_reads_rank,
		size_t* coordinates, int* size, Read* data_chr, int local_readNum)
{
	size_t dataSize = 0;
	size_t j;
	Read* chr = data_chr;

	//we initialize offset source and size_source
	for(j = 0; j < local_readNum; j++){
		size[j] = 0;
		coordinates[j] = 0;
	}

	//first we are going to read in the source file
	for(j = 0; j < local_readNum; j++){
		//offset is the read size
		coordinates[j] = chr->coord;
		size[j] = (int)chr->offset; //read length
		local_reads_rank[j] = rank;
		dataSize += chr->offset;

		chr = chr->next;
	}

	return dataSize;
}


void chosen_split_rank_gather_size_t(MPI_Comm split_comm, int rank, int num_proc, int master, size_t size, size_t *size_per_jobs,
		size_t *start_size_per_job, size_t *all_data, size_t *data, size_t start_index)
{
	MPI_Status status;
	size_t j, k;
	//size_t *temp_buf = malloc(sizeof(size_t));


	if (rank == master){
		// we copy element for rank master_2
		// we eliminate zeros from the
		// beginning of the vector
		size_t st = start_size_per_job[master];
		for (k = 0; k < size_per_jobs[master]; k++){
			all_data[st] = data[k+start_index];
			st++;
		}

		for(j = 0; j < num_proc; j++){

			if (j != master && size_per_jobs[j] != 0){

				/*
				temp_buf  = realloc(temp_buf, size_per_jobs[j]*sizeof(size_t));
				assert( temp_buf !=0);
				*/

				size_t temp_buf[size_per_jobs[j]];
				temp_buf[size_per_jobs[j]] = 0;
				assert(temp_buf !=0 );
				MPI_Recv(temp_buf, size_per_jobs[j], MPI_LONG_LONG_INT, j, 0, split_comm, &status);

				size_t st = start_size_per_job[j];
				for (k = 0; k < size_per_jobs[j]; k++){
					all_data[st] = temp_buf[k];
					st++;
				}

			}
		}
	}
	else{
		MPI_Send(data, size, MPI_LONG_LONG_INT, master,  0, split_comm);
	}

}
