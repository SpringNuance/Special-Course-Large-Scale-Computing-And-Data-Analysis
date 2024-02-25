# csE4002

This is a workspace foe CS-E4002 project.

## Dataset

The dataset that has been used to test this project was presented by [Prof. Maarit Käpylä](https://people.aalto.fi/maarit.kapyla) from her research group. 

The presented data was in HDFS format with measurements of various different components. 

The aim of the project was to test the usage of Spark on the HDFS dataset and whether it is particularly useful for this case or not. 

## Literature

1. [Analyzing astronomical data with Apache Spark](https://indico.cern.ch/event/735616/contributions/3033957/attachments/1687000/2713145/spark_at_lal.pdf)

2. [AXS: A framework for fast astronomical data processing based on Apache Spark](https://arxiv.org/ct?url=https%3A%2F%2Fdx.doi.org%2F10.3847%2F1538-3881%2Fab2384&v=895b3c5b)

3. [Accelerating Astronomical Discoveries With Apache Spark](https://www.slideshare.net/databricks/accelerating-astronomical-discoveries-with-apache-spark)

4. [Dask: Parallel Computation with Blocked algorithms and Task Scheduling](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.825.5314&rep=rep1&type=pdf)

5. [Better and faster hyperparameter optimization with Dask](http://conference.scipy.org/proceedings/scipy2019/pdfs/scott_sievert.pdf)

6. [A performance comparison of Dask and Apache Spark for data-intensive neuroimaging pipelines](https://arxiv.org/pdf/1907.13030.pdf)

### Useful Resources

1. [PySpark Documentation](https://spark.apache.org/docs/latest/api/python/)

2. [Dask Documentation](https://dask.org/)

3. [Dask ML](https://ml.dask.org/)

## Conclusion:

The project was hosted on CSC computing environment under various different configurations. However, it was found that Spark was not suitable for handling data which is stored in different arrays. It was however found out that if we use Dask with Python, it enables us to Parallelize arrays and hence the computation is a bit faster. Moreover, this enables us to use Fast Fourier Transform, Principal Componenet Analysis and other dimensionality reduction methods with ease. 