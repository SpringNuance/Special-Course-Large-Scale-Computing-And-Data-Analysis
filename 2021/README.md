# Special Course on Large-scale Computing and Data Analysis

## Overview

This is a special couse on Large-Scale Computing and Data Analysis. The learning goal is to familiariaze the students with high-performance computing tools in use in present-day supercomputing facilities, and to practical applications that they can be utilized on. During this project-oriented course, the students are working hands on with pre-existing tools on challenging problems and data sets in a real supercomputing environment. In addition to working on a specific tool/application individually/in teams, the students will learn from other tools and applications through demonstrations by other students/teams.
>[Curriculum course description](coursedescription.md)

## Plan and Assessment
A preliminary list of topics has been collected below  and students can also propose their own topic. A supercomputing environment will be set up for the course participants (hosted at [CSC](https://research.csc.fi)). Depending on the selection of topics, the students can either work individually or in teams.

After introductory lecture(s) on how the environment works, the instructors of the topic will set up weekly meetings with the student (teams), and give materials and instructions on the project (introduction to the tool, documentations, papers, demonstration of the usage). After introductory session(s), the weekly meetings will serve as project update sessions. In the end of the course, the student (teams) are expected to give demonstrations to other student (teams) on their project.

Participation in 80% of the sessions and a successful demonstration session to other students is required to pass the course. A session participation missed can be compensated by writing a learning diary in the form of a short email (or equivalent) to the instructor.

## Schedule and Material
Date    | Topics |
--------|--------------|
23.04.2021   | [Course Management](lectures/Course_management.pdf), Topic Presentation

## Preliminary topics for 2021

#### Topic: Evaluating function-as-a-service models in HPC for large-scale data analysis
  > Evaluating [funcX](https://dl.acm.org/doi/pdf/10.1145/3295500.3356164) for large-scale data analysis
  > Supervisor: Linh Truong
  - Links:
    - [funcX](https://funcx.org/)
    - Papers about funcX: https://funcx.org/publications.html
    - Target systems to be evaluated: CSC cloud machines (Rahti, Allas) and supercomputing/HPC (Mahti, Puhti)
    - Evaluation tasks: data movement and data analytics/computation,  workflows and pipelines support.
#### Topic: Human-in-the-loop for connecting ML pipelines and workflow-based data analysis in large-scale computing
  >Evaluating, designing and developing connectors to human-based analytics/decisions in ML/data analysis workflows
  > Supervisor: Linh Truong
- Links:
  - Workflows and interfaces to humans: https://link.springer.com/chapter/10.1007/978-3-642-32820-6_79
  - Workflows: https://airflow.apache.org/, https://www.kubeflow.org/, https://argoproj.github.io/

#### Topic: GPU-based containers Deployment and Management for ML Training in HPC
   >Explore how to manage a virtual cluster of GPU-based containers atop an HPC for ML tasks
   > Supervisor: Linh Truong
   - Links:
     - Examine the best ways to manage GPU containers for distributed computing (e.g.,  practical work with [Singularity containers](https://docs.csc.fi/support/tutorials/gpu-ml/)
     - Evaluate these ways for running [MLPerf HPC Training](https://mlcommons.org/en/training-hpc-07/) in CSC Puhti

#### Topic: Evaluating patterns for optimizing data locality in HPC Nodes large-scale data analysis/ML tasks
  >Tasks executed in an HPC node need access data but accessing data from the shared file system (e.g., Lustre) might be slow. Patterns for data coupling and movement between HPC storage/outside cloud and local nodes will be evaluated and suggested (e.g. with different HPC systems in Aalto/CSC)
  > Supervisor: Linh Truong
  - Link:
    - Examine data movement techniques between/within HPC/Cloud: reactive data movement, scheduled movement or incremental data movement
    - Build examples of data movement tasks and couple them into the data analytics
    - Using CSC Allas/Cloud storage and CSC Supercomputing/HPC
    - Application/data: potentially ML/e-science (dependent on student experiences)
    - The Lustre file systems in CSC:
      - https://docs.csc.fi/computing/lustre/
      - https://docs.csc.fi/computing/disk/
      - [Evaluation of active storage strategies for the lustre parallel file system](https://dl.acm.org/doi/10.1145/1362622.1362660)
      - [Analysis of Six Distributed File Systems](https://hal.inria.fr/hal-00789086/)
      - [Best Practices and Lessons Learned from Deploying and Operating Large-Scale Data-Centric Parallel File Systems](https://ieeexplore.ieee.org/abstract/document/7013005)
    - Cloud Storage
      - https://docs.csc.fi/data/Allas/
      - https://docs.openstack.org/swift/latest/api/object_api_v1_overview.html
      - [Is cloud storage ready? Performance comparison of representative IP-based storage systems](https://www.sciencedirect.com/science/article/pii/S0164121218300025)
      - [Investigating an Open Source Cloud Storage Infrastructure for CERN-specific Data Analysis](https://ieeexplore.ieee.org/document/6310879)
    - Data movement:
      - https://engineering.purdue.edu/dcsl/publications/papers/2021/sonic_atc21.pdf

#### Topic: Building&/|using&/|optimizing a stencil-based application using a CUDA-MPI library with a domain-specific language on graphics processing units (GPUs)
  > see https://bitbucket.org/jpekkila/astaroth/src/master/
  > Use an existing magnetized plasma simulation implementation, and study the problematics of communication bound systems and ways to go around this problem.
  > Bring in your own problem; Pre-requisites: your problem has a repetitive scheme to update array elements in 3D. Game of life in 3D would be one example.
  > Supervisor: Maarit J. Käpylä

#### Topic: Analysing large-scale real-world data in supercomputing environments using modern analytics engines (such as Apache Spark)
  > One option is to use pre-generated binary data blobs from a mystery simulation model containing a hidden feature; the aim of the project is to reveal the hidden feature.
  > You can also bring in your own data, or generate your own data during the project, e.g. by using the GPU simulation tool mentioned above.
  > Supervisor: Maarit J. Käpylä

## Application and Dataset Samples

The list of [application and dataset samples](appdata.md) will be updated.

## Student Project
* [Data Compression](https://bitbucket.org/jpekkila/cs-e4002-data-compression-in-hpc/src/master/)
* [Human-in-the-loop](https://github.com/ngndn/large-scale-computing-data-analysis)
* [Data Movement](https://github.com/vinhng10/sativa)
* [Data Transformation](https://version.aalto.fi/gitlab/kapoorr1/cse4002)
