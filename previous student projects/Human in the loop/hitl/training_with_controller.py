import sys
import json
import logging

from training import train
from controller import Controller

FORMATTER = logging.Formatter(
    "%(asctime)s — %(name)s — %(thread)s — %(threadName)s — %(levelname)s — %(message)s"
)

logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

stream_handler = logging.StreamHandler(sys.stdout)
stream_handler.setLevel(logging.INFO)
stream_handler.setFormatter(FORMATTER)
logger.addHandler(stream_handler)

file_handler = logging.FileHandler(f'track.log')
file_handler.setLevel(logging.DEBUG)
file_handler.setFormatter(FORMATTER)
logger.addHandler(file_handler)


if __name__ == '__main__':
    with open('config/training_controller_config.json') as f:
        config = json.load(f)

    train_controller = Controller(config, train)
    logger.info('Running training with controller')
    train_controller.run(logger, num_threads=config['num_threads'])
