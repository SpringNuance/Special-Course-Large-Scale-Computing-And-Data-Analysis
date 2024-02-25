# First interview
## Overview of the sun active regions data analysis pipeline
### Problem Statement
The purpose of the pipeline is to eventually automatically predict the active regions of the Sun several days before it happen. The benefit can be mitigate the damage and disruption due to the bad space weather effects cause by those active regions of the Sun.

### The workflow
The workflow is the data analysis process. The current process is:
1. Identify the quite sun periods and collect the associated data to serve as a baseline.
	1. The quite sun periods is identify by analyzing the magnetic magnitude plots.
	2. Then others data such as solar surface velocities and f-mode mass are collected.
2. Based on the data of the quite sun baseline, identify the active regions to build training samples for ML tools.
3. Built the ML tools to predict the active regions.

### The Pipeline
The pipeline consist of the following components:
1. The data storage
	1. An external database to get/query the required data
	2. An internal databse to stored the required data after querying from external database. The internal database also stores data products which are the results of the data analysis (time series of sun activity activity, time series of f-mode mass).
	3. The endpoint service to fetch the new data from the external server to the internal one if new data available.
2. The supercomputer
	1. Handle the computing tasks to produce the required data products. Data is transfer from internal database to compute and save the resulted data products back to the internal database.
3. The data products:
	1.	**Good enough** statistics/data products needed for the problem such as time series of sun activity activity, time series of f-mode mass, and more later on.

### A human-in-the-loop potential.
In the current pipeline, one step which the expert knowledge is needed is the evaluation of the **good enough** statistics/data products. Several examples are describe below:
1. In the magnetic magnitude plots, the experts need to look and evaluate which data points are from the quite regions. And the same process may be applied to the active ones.
2. The experts also needed to look at other plots to identify the differences between f-mode mass in quite and active regions as well.
Those can be potential areas for human-in-the-loop solutions.

### Mapping the usecases to User-Context-Task Decision Support Systems framework
- User: the researcher of the sun active regions prediction pipeline.
- Context: prediction of sun active regions, produce meaningful data points to use in the ML training later on.
- Task: Identify the properties of the active/quiet regions by evaluating different statistics/data products.