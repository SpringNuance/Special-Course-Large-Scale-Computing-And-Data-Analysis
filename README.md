# Special Seminar on Large-scale Computing and Data Analysis

## Overview

This is a special couse on Large-Scale Computing and Data Analysis. The learning goal is to familiariaze the students with high-performance computing tools in use in present-day supercomputing facilities, and to practical applications that they can be utilized on. During this project-oriented course, the students are working hands on with pre-existing tools on challenging problems and data sets in a real supercomputing environment. In addition to working on a specific tool/application individually/in teams, the students will learn from other tools and applications through demonstrations by other students/teams.
>[Curriculum course description](coursedescription.md)

>[Aalto Sisu Registration](https://sisu.aalto.fi/student/courseunit/otm-280d93db-728b-40f9-a12e-94e539497b0c)

## Plan and Assessment
A preliminary list of topics has been collected below  and students can also propose their own topic. A supercomputing environment will be set up for the course participants (hosted at [CSC](https://research.csc.fi)). Depending on the selection of topics, the students can either work individually or in teams.

After introductory lecture(s) on how the environment works, the instructors of the topic will set up weekly meetings with the student (teams), and give materials and instructions on the project (introduction to the tool, documentations, papers, demonstration of the usage). After introductory session(s), the weekly meetings will serve as project update sessions. In the end of the course, the student (teams) are expected to give demonstrations to other student (teams) on their project.

<!--- MJK OLD
Participation in 80% of the sessions and a successful demonstration session to other students is required to pass the course. A session participation missed can be compensated by writing a learning diary in the form of a short email (or equivalent) to the instructor.
--->
Participation in the seminar sessions (min 80%), activity at CSC (consuming BUs in the service relevant for the topic), a max. 20-min. final presentation to other students, participation in others' final presentations are required to pass the course. 

## Schedule and Material
Date    | Topics |
--------|--------------|
20.04.2022   | [Course Management](lectures/Course_management.pdf), Topic Presentations

## Preliminary topics for 2022

#### Topic:  TBD
  > TBD
  > Supervisor: TBD

#### Topic:  Distributed Machine Learning Training in HPC: Best practices with CSC HPC and current state-of-the-art tools
> Supervisor: Hong-Linh Truong

  * Examine state-of-the-art tool for distribute machine learning frameworks (such as [Horovod](https://github.com/horovod/horovod)) and their dependencies on underlying distributed computing frameworks, such as OpenMPI and batch job management
  * Study and perform distributed ML training with CSC resources
  * Report the best practices

#### Topic:  Cross cloud-HPC workflow management
> Supervisor: Hong-Linh Truong
* Examine the way to involve different task/job workflow management in cloud systems and HPC
* Study the issue of integration, portability and monitoring of cross cloud-hpc task execution
* Study, test and perform cross cloud-HPC task execution and management (data transfers and computation)
* Report best practices with real-world clouds (such as CSC and Google) and HPC systems (CSC)


#### Topic: Using and/or optimizing a stencil-based application using a CUDA-MPI library with a domain-specific language on graphics processing units (GPUs)
 > Supervisor: Maarit J. Käpylä
  - Use an existing magnetized plasma simulation implementation, and study the problematics of communication bound systems and ways to go around this problem and/or
  - Use the code in Intel/AMD based systems (Puhti/Mahti) and study and compare the performance, scale-up and energy-efficiency or
  - Bring in your own problem; Pre-requisites: your problem has a repetitive scheme to update array elements in 3D. Game of life in 3D would be one example.
  - Reading:  http://dx.doi.org/10.1016/j.cpc.2017.03.011, http://dx.doi.org/10.1016/j.cpc.2017.03.011, https://doi.org/10.1016/j.parco.2022.102904, http://urn.fi/URN:NBN:fi:aalto-201906233993
  - Code is available from https://bitbucket.org/jpekkila/astaroth/src/master/

#### Topic: Coloring large graphs with supercomputers
> Supervisor: Jukka Suomela
* _Background:_ [Graph coloring](https://en.wikipedia.org/wiki/Graph_coloring) is a computationally hard problem, but it can be solved in practice with the help of e.g. modern [SAT solvers](https://en.wikipedia.org/wiki/SAT_solver). However, many SAT solvers are single-threaded, and it is very hard to predict how long it takes to solve an instance.
* _Task:_ Develop a tool where you can give a collection of graphs (in some suitable machine-readable form), and it uses SAT solvers on Puhti, Mahti, and/or cPouta to find a coloring for each graph as efficiently as possible. Experiment with different encodings and different solvers, including parallel solvers. Try out also very large graphs, with up to 100M nodes. Explore tradeoffs between the use of CPU time vs. wall-clock time vs. memory usage. Report your findings.

#### Topic: Large-Scale Networked Federated Learning
> Supervisor: Alex Jung
* _Background:_ Many machine learning (ML) applications involve network structured data. As a case in point, consider an image that consists of millions of 
pixels. We can think of an image as a network whose nodes represent rectangular image patches. Neighbouring image patches often have similar 
statistical properties (as they might depict different parts of the same object). Networked federated learning (NFL) is a recent ML paradigm that studies 
distributed optimization algorithms that learn separate (tailored) models for the nodes of a data network. This project studies the practical implementation 
of distributed optimization  algorithms using the supercomputer service of CSC. 
* _Task:_ Familiarize yourself with the use of Matlab on the CSC supercomputer Puhti (https://docs.csc.fi/apps/matlab/). Familarize yourself with the basic task 
of image segmentation. Implement algorithm 1 of 
N. Tran, H. Ambos and A. Jung, "Classifying Partially Labeled Networked Data VIA Logistic Network Lasso," ICASSP 2020 - 2020 IEEE International Conference on Acoustics, Speech and Signal Processing (ICASSP), 2020, pp. 3832-3836, doi: 10.1109/ICASSP40776.2020.9054408. using Matlab on Puhti. Push the implementation towards  being able to do frame-wise image 
segmentation on a 2 hour high-definition movie. 

#### Topic: The Volume, Velocity, and Variety of data challenge: HPC to the rescue!
> Supervisor: Jose-Ramon Herrero, UPC, BarcelonaTech
* _Background:_ One of the biggest challenges of the current big data landscape is our inability to process vast amounts of information in a reasonable time. In this work, we explore and compare two distributed computing frameworks implemented on commodity cluster architectures: MPI/OpenMP that is high-performance oriented and exploits multi-machine/multi- core infrastructures, and Apache Spark on Hadoop which targets iterative algorithms through in-memory computing. We use Cloud Platform service to create virtual machine clusters, run the frameworks, and evaluate two supervised machine learning algorithms: KNN and Pegasos SVM. We make experiments with a fluid dynamic simulation data sets and inspect the performance a different frameworks.
* _Tasks:_ Choose a dataset with the supervisor, decide which (minimally two) frameworks you wish to explore in detail, and compare their performace and other properties in CSC environment.
* _References:_ Jorge L. Reyes-Ortiz, Luca Oneto, Davide Anguita, Big Data Analytics in the Cloud: Spark on Hadoop vs MPI/OpenMP on Beowulf,
Procedia Computer Science, Volume 53, 2015, doi.org/10.1016/j.procs.2015.07.286; Alshahrani, S., Al Shehri, W., Almalki, J., Alghamdi, A.M., Alammari, A.M. Accelerating Spark-Based Applications with MPI and OpenACC Hindawi Complexity Volume 2021, A.ID 9943289, doi.org/10.1155/2021/9943289;
Timothée Dubuc, Frederic Stahl and Etienne B. Roesch: Mapping the Big Data Landscape: Technologies, Platforms and Paradigms for Real-Time Analytics of Data Streams, IEEE Access, 2021. 10.1109/ACCESS.2020.3046132

## Application and Dataset Samples

The list of [application and dataset samples](appdata.md) will be updated.

## Previous editions
* [2021 Seminar](2021/README.md)
## Previous student projects
* [Data Compression](https://bitbucket.org/jpekkila/cs-e4002-data-compression-in-hpc/src/master/)
* [Human-in-the-loop](https://github.com/ngndn/large-scale-computing-data-analysis)
* [Data Movement](https://github.com/vinhng10/sativa)
* [Data Transformation](https://version.aalto.fi/gitlab/kapoorr1/cse4002)
