#! /bin/bash

#PBS -N MPISORT_MYSAM_4_JOBS
#PBS -l	nodes=2:ppn=2:mem=${MEM}     	# Ask 2 nodes and 2 jobs per node
#PBS -l walltime=24:00:00
#PBS -o STDOUT_FILE.%j.o
#PBS -e STDERR_FILE.%j.e

type mpiSORT

if [[ ! $? -eq 0 ]]
then
    echo "ERROR: mpiSORT not found in your PATH"
    exit 1
fi

output_dir=${HOME}/mpiSORTExample

if [[ ! -d ${output_dir} ]]
then
    mkdir -p ${output_dir}
    echo "INFO: the directory ${output_dir} has been created to store the results generated by mpiSORTExample"
fi

cd ${PBS_O_WORKDIR}

mpirun -n 4 mpiSORT data/HCC1187C_70K_READS.sam ${output_dir} -p -q 0 2> ${output_dir}/mpiSORT.log

echo "INFO: mpiSORT.log is available in ${output_dir}"

