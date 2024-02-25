import sys
import json
import redis


if __name__ == '__main__':
    with open(sys.argv[1]) as f:
        config = json.load(f)

    host = config['redis_address']
    port = config['redis_port']
    redis_client = redis.Redis(host='localhost', port=6379)

    # Load the config file
    routine_config_file_path = config['routine_config_file']
    with open(routine_config_file_path) as f:
        routine_config = json.load(f)

    message = json.dumps(routine_config)
    channel = config['source']
    redis_client.publish(channel=channel, message=message)
