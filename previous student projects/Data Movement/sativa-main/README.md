# Sativa - Data Transfer Tools Benchmarking
The aim of this project is to benchmarking the performance of three data transfer tools supported in CSC supercomputing 
and cloud environment. These tools include swift, s3cmd, and rclone. From there derive the best practices in using these
tools and draw direction for future development.


# Background
High-performance computing application in supercomputing environment can generate a huge amount of data. There exists a 
demand for moving those generated data to third party storage for further post-processing and analysis.  

The issue is that the amount of time required to transfer the data can be very long, and might be vary depending on the
tool used and the specified parameters.


# Setup
The current implementation works like grid search, that is, using each tool on a set of different sets of parameters and 
record their performance for post analysis.

There are two way to run this project: [interative job](https://docs.csc.fi/computing/running/interactive-usage/) 
and [batch job](https://docs.csc.fi/computing/running/getting-started/). Please follow the links for detailed instructions. 
At the moment, it's recommended to go with the interactive job for better control. However, the trade-off is that only limited
resources can be reserved for the interative job (maximum 8 CPU cores). Batch job has more resource capacity.

After setting up the job, establish connection with Allas from CSC Puhti/Mahti. Each
tool will have different set of commands. Instructions can be found [here](https://docs.csc.fi/data/Allas/).

From there, specify parameters for the experiment in [configs/config.json](https://github.com/vinhng10/sativa/blob/main/configs/config.json):
- db: path to the experiment database
- version: version of the experiment
- bucket: name of the bucket to save data on Allas
- cluster: name of CSC supercomputing cluster
- node: type of node to run the experiment
- tool: tool used for data transfer
- data_dir: path to the data file to transfer
- file_sizes: list of sizes to experiment transfering
- file_split_sizes: list of sizes to split the file
- segment_sizes: list of sizes of a segment transfered by the tool
- threads: list of number of threads to run experiment
- cores: list of number of cores to run experiment

The final step is run experiment by:
```bash
python3 core/run.py
```

Additional Information:
- [core/utils.py](https://github.com/vinhng10/sativa/blob/main/core/utils.py) contains functional API to run swift, s3cmd, and rclone tools.  

- [core/experiment.py](https://github.com/vinhng10/sativa/blob/main/core/experiment.py) contains:
  - **SubExperiment**: class to run a given tool on a single file and record the time taken to complete the data transfer.
  - **Experiment**: class to split a file into chunks and use **SubExperiment** on each chunk, then record the total time
    to transfer all data chunks. 

- [core/run.py](https://github.com/vinhng10/sativa/blob/main/core/run.py) is the main script to launch the experiment. It first read the configuration file and launch experiments
  according to the config file.

The results obtained so far were recorded in [db/prod.db](https://github.com/vinhng10/sativa/blob/main/db/prod.db) 
and [db/prod_2.db](https://github.com/vinhng10/sativa/blob/main/db/prod_2.db). Further experiments can record results
to these databases.


# Project Plan
#### Week 1:
- Choose a background theme and a specific problem for the project.
- Learn about data movement problem in cloud and HPC environment in general.
- Review current data locality and movement techniques in cloud and HPC.
- Review related tools suitable for implementing the project.

#### Week 2 & 3:
- Setup tools for development and experiment.
- Implement functions/features/pipelines/components for evaluating data locality and   
- Run experiment and record result.
- Extend the project if possible.

#### Week 4:
- Finalize experiment and implement demo.
- Finalize github repository with full documentation.
- Prepare representation
