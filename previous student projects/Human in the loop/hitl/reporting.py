import sys
import json
import redis


if __name__ == '__main__':
    with open(sys.argv[1]) as f:
        config = json.load(f)

    host = config['redis_address']
    port = config['redis_port']
    redis_client = redis.Redis(host='localhost', port=6379)

    subscriber = redis_client.pubsub()
    subscriber.subscribe(config['notification'])

    print('Awaiting messages')
    for item in subscriber.listen():

        # if type(item['data']) is int:
        #     continue

        print(item['data'])
