import time
import json
import random


# Blackbox function which takes a long time to run
def train(alpha, beta, gamma):
    result = {
        'x': alpha + random.randint(1, 100),
        'y': beta - random.randint(1, 100),
        'z': gamma + random.randint(25, 75),
    }

    time.sleep(10)

    # Something run here and produce the result
    return result


if __name__ == '__main__':
    # Input
    with open('config/training_config.json') as f:
        config = json.load(f)

    # Main process
    print('Training')
    training_result = train(**config)
    print('Complete')

    # Output
    with open('results/results.csv', 'w') as fw:
        json.dump(training_result, fw)
