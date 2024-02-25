import json
import redis
import random


def training_algorithm(hyper_parameter):
    print(f'Training with hyperparameter={hyper_parameter}')
    result = hyper_parameter*2 + random.uniform(-1, 5)
    with open('../results/results.csv', 'a') as f:
        f.write(f'{hyper_parameter}, {result}\n')
    print('Training complete. Waiting for instruction.')


def main():
    redis_client = redis.Redis(host='localhost', port=6379)
    subscriber = redis_client.pubsub()
    subscriber.subscribe('hyperparameter')
    print('Waiting for instruction')
    for item in subscriber.listen():

        if type(item['data']) is int:
            continue

        message = json.loads(item['data'])
        hyper_parameter = message.get('hyperparameter', None)
        if hyper_parameter:
            training_algorithm(int(hyper_parameter))


if __name__ == '__main__':
    main()
