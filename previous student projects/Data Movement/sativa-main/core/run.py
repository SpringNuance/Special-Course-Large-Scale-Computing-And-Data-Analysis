import argparse
import json
import shutil
from pathlib import Path
from multiprocessing import Pool

from experiment import Experiment, SubExperiment
from utils import get_file_of_size, get_parameters_dicts


if __name__ == "__main__":
    ###########################################################################
    ### EXPERIMENT SETUP                                                    ###
    ###########################################################################

    # Parse argument:
    parser = argparse.ArgumentParser()

    parser.add_argument("-c", "--cfg", required=True,
                        help="Path to the experiment config file.")

    args = parser.parse_args()

    # Read experiment config file:
    with open(args.cfg, "r") as f:
        config = json.load(f)

    ###########################################################################
    ### EXPERIMENT RUN                                                      ###
    ###########################################################################

    # Get parameter iterator:
    parameters = get_parameters_dicts(
        file_size=config["file_sizes"],
        file_split_size=config["file_split_sizes"],
        segment_size=config["segment_sizes"],
        thread=config["threads"],
        core=config["cores"]
    )

    # Run experiment on each parameter setting:
    for parameter in parameters:
        print(f"Run experiment with {parameter} ...")

        # Get or create file of the specified size for experimenting:
        file = get_file_of_size(parameter["file_size"], config["data_dir"])

        try:
            # Run experiment:
            experiment = Experiment(
                config["db"], file, config["version"], config["bucket"],
                config["cluster"], config["node"], config["tool"],
                parameter["file_split_size"], parameter["segment_size"],
                parameter["thread"], parameter["core"]
            )
            results = experiment.run()
        except Exception as e:
            print(e)
            print("Retrying ... ")
            experiment.delete_bucket()
            # Run experiment:
            experiment = Experiment(
                config["db"], file, config["version"], config["bucket"],
                config["cluster"], config["node"], config["tool"],
                parameter["file_split_size"], parameter["segment_size"],
                parameter["thread"], parameter["core"]
            )
            results = experiment.run()
        finally:
            # Delete the uploaded bucket in Allas to avoid conflicts with other
            # transfers. Also, remove data to avoid exceeding disk storage quota:
            experiment.delete_bucket()
            shutil.rmtree(config["data_dir"])
            print("Finished experiment.\n")
