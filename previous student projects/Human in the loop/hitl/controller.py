import json
import time
import threading

from collections.abc import Callable
from typing import Optional

import redis


class Controller:
    def __init__(self, config: dict,
                 function: Optional[Callable] = None,
                 function_params: Optional[dict] = None):

        self.redis_address = config['redis_address']
        self.redis_port = config['redis_port']
        self.source = config['source']
        self.target = config['target']
        self.notification = config['notification']
        self.process_name = config['process_name']

        self.redis_client = redis.Redis(
            host=self.redis_address,
            port=self.redis_port
        )

        self.function = function
        self.function_params = function_params

    def run(self, logger, num_threads: int = 1):
        subscriber = self.redis_client.pubsub()
        subscriber.subscribe(self.source)

        logger.info('Awaiting parameters')
        for item in subscriber.listen():

            if type(item['data']) is int:
                continue

            logger.info(f'Execute {self.process_name}')
            self.function_params = json.loads(item['data'])

            threads = list()

            for i in range(num_threads):
                thread_name = f'{self.process_name} thread {i}'
                logger.info(f'Main: Starting {thread_name}')

                thread = threading.Thread(
                    target=self.execute,
                    args=(thread_name,),
                    name=thread_name
                )

                threads.append(thread)
                thread.start()

            for index, thread in enumerate(threads):
                logger.info(f'Main: before joining {thread.name} thread')
                thread.join()
                logger.info(f'Main: thread {thread.name} done')

    def execute(self, thread_name):
        start_time = time.time()
        function_returns = self.function(**self.function_params)
        end_time = time.time()
        duration = end_time - start_time
        result = {
            'routine_name': self.process_name,
            'thread_name': thread_name,
            'function_params': self.function_params,
            'result': function_returns,
            'start_time': start_time,
            'end_time': end_time,
            'duration': duration
        }

        self._message_publish(result)

    def _message_publish(self, result):
        message = json.dumps(result)

        # Publish the result
        self.redis_client.publish(self.target, message=message)

        # Publish the notification
        self.redis_client.publish(self.notification, message=message)
